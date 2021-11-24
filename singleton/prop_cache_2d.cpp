/*
Copyright (c) 2019-2021 Péter Magyar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "prop_cache_2d.h"

#include "../props/prop_data_2d.h"
#include "../props/prop_data_entry_2d.h"

#include "core/version.h"

#if VERSION_MAJOR > 3
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#else
#include "core/engine.h"
#include "core/project_settings.h"
#endif

#include "../jobs/prop_texture_job_2d.h"

#if THREAD_POOL_PRESENT
#include "../../thread_pool/thread_pool.h"
#endif

#include "../material_cache/prop_material_cache_2d.h"
#include "../tiled_wall/tiled_wall_data_2d.h"

#include "core/hashfuncs.h"

#if VERSION_MAJOR > 3

#define VARIANT_ARRAY_GET(arr)             \
	Vector<Variant> r;                     \
	for (int i = 0; i < arr.size(); i++) { \
		r.push_back(arr[i]);               \
	}                                      \
	return r;

#else

#define VARIANT_ARRAY_GET(arr)             \
	Vector<Variant> r;                     \
	for (int i = 0; i < arr.size(); i++) { \
		r.push_back(arr[i].get_ref_ptr()); \
	}                                      \
	return r;

#endif

PropCache2D *PropCache2D::_instance;

PropCache2D *PropCache2D::get_singleton() {
	return _instance;
}

StringName PropCache2D::get_default_prop_material_cache_class() {
	return _default_prop_material_cache_class;
}
void PropCache2D::set_default_prop_material_cache_class(const StringName &cls_name) {
	_default_prop_material_cache_class = cls_name;
}

#ifdef TEXTURE_PACKER_PRESENT
int PropCache2D::get_texture_flags() const {
	return _texture_flags;
}
void PropCache2D::set_texture_flags(const int flags) {
	_texture_flags = flags;
}

int PropCache2D::get_max_atlas_size() const {
	return _max_atlas_size;
}
void PropCache2D::set_max_atlas_size(const int size) {
	_max_atlas_size = size;
}

bool PropCache2D::get_keep_original_atlases() const {
	return _keep_original_atlases;
}
void PropCache2D::set_keep_original_atlases(const bool value) {
	_keep_original_atlases = value;
}

Color PropCache2D::get_background_color() const {
	return _background_color;
}
void PropCache2D::set_background_color(const Color &color) {
	_background_color = color;
}

int PropCache2D::get_margin() const {
	return _margin;
}
void PropCache2D::set_margin(const int margin) {
	_margin = margin;
}
#endif

PoolStringArray PropCache2D::material_paths_get() const {
	return _material_paths;
}
void PropCache2D::material_paths_set(const PoolStringArray &value) {
	_material_paths = value;
}

void PropCache2D::material_add(const Ref<Material> &value) {
	ERR_FAIL_COND(!value.is_valid());

	_materials.push_back(value);
}

Ref<Material> PropCache2D::material_get(const int index) {
	ERR_FAIL_INDEX_V(index, _materials.size(), Ref<Material>());

	return _materials[index];
}

void PropCache2D::material_set(const int index, const Ref<Material> &value) {
	ERR_FAIL_INDEX(index, _materials.size());

	_materials.set(index, value);
}

void PropCache2D::material_remove(const int index) {
	_materials.remove(index);
}

int PropCache2D::material_get_num() const {
	return _materials.size();
}

void PropCache2D::materials_clear() {
	_materials.clear();
}

void PropCache2D::materials_load() {
	_materials.clear();

	for (int i = 0; i < _material_paths.size(); ++i) {
		StringName path = _material_paths[i];

		ERR_CONTINUE(path == "");

		Ref<Material> d = load_resource(path, "Material");

		ERR_CONTINUE(!d.is_valid());

		_materials.push_back(d);
	}
}

void PropCache2D::ensure_materials_loaded() {
	if (_materials.size() != _material_paths.size()) {
		materials_load();
	}
}

Vector<Variant> PropCache2D::materials_get() {
	VARIANT_ARRAY_GET(_materials);
}

void PropCache2D::materials_set(const Vector<Variant> &materials) {
	_materials.clear();

	for (int i = 0; i < materials.size(); i++) {
		Ref<Material> material = Ref<Material>(materials[i]);

		_materials.push_back(material);
	}
}

Ref<PropMaterialCache2D> PropCache2D::material_cache_get(const Ref<PropData2D> &prop) {
	ERR_FAIL_COND_V(!prop.is_valid(), Ref<PropMaterialCache2D>());

	//get pointer's value as uint64
	uint64_t k = make_uint64_t<const PropData2D *>(*prop);

	_material_cache_mutex.lock();

	if (_material_cache.has(k)) {
		Ref<PropMaterialCache2D> m = _material_cache[k];

		m->inc_ref_count();

		_material_cache_mutex.unlock();

		return m;
	}

	PropMaterialCache2D *p = Object::cast_to<PropMaterialCache2D>(ClassDB::instance(_default_prop_material_cache_class));

	if (!p) {
		ERR_PRINT("Can't instance the given PropMaterialCache2D! class_name: " + String(_default_prop_material_cache_class));
	}

	Ref<PropMaterialCache2D> m(p);

	_material_cache[k] = m;

	_material_cache_mutex.unlock();

	return m;
}
void PropCache2D::material_cache_unref(const Ref<PropData2D> &prop) {
	//get pointer's value as uint64
	uint64_t k = make_uint64_t<const PropData2D *>(*prop);

	_material_cache_mutex.lock();

	if (!_material_cache.has(k)) {
		_material_cache_mutex.unlock();

		ERR_PRINT("PropCache2D::material_cache_unref: can't find cache!");

		return;
	}

	Ref<PropMaterialCache2D> m = _material_cache[k];

	m->dec_ref_count();
	if (m->get_ref_count() <= 0) {
		_material_cache.erase(k);
	}

	_material_cache_mutex.unlock();
}

Ref<PropMaterialCache2D> PropCache2D::tiled_wall_material_cache_get(const Ref<TiledWallData2D> &twd) {
	ERR_FAIL_COND_V(!twd.is_valid(), Ref<PropMaterialCache2D>());

	//get pointer's value as uint64
	uint64_t k = make_uint64_t<const TiledWallData2D *>(*twd);

	_tiled_wall_material_cache_mutex.lock();

	if (_tiled_wall_material_cache.has(k)) {
		Ref<PropMaterialCache2D> m = _tiled_wall_material_cache[k];

		m->inc_ref_count();

		_tiled_wall_material_cache_mutex.unlock();

		return m;
	}

	PropMaterialCache2D *p = Object::cast_to<PropMaterialCache2D>(ClassDB::instance(_default_prop_material_cache_class));

	if (!p) {
		ERR_PRINT("Can't instance the given PropMaterialCache2D! class_name: " + String(_default_prop_material_cache_class));
	}

	Ref<PropMaterialCache2D> m(p);

	_tiled_wall_material_cache[k] = m;

	_tiled_wall_material_cache_mutex.unlock();

	return m;
}
void PropCache2D::tiled_wall_material_cache_unref(const Ref<TiledWallData2D> &twd) {
	//get pointer's value as uint64
	uint64_t k = make_uint64_t<const TiledWallData2D *>(*twd);

	_tiled_wall_material_cache_mutex.lock();

	if (!_tiled_wall_material_cache.has(k)) {
		_tiled_wall_material_cache_mutex.unlock();

		ERR_PRINT("PropCache2D::material_cache_unref: can't find cache!");

		return;
	}

	Ref<PropMaterialCache2D> m = _tiled_wall_material_cache[k];

	m->dec_ref_count();
	if (m->get_ref_count() <= 0) {
		_tiled_wall_material_cache.erase(k);
	}

	_tiled_wall_material_cache_mutex.unlock();
}

Ref<PropMaterialCache2D> PropCache2D::material_cache_custom_key_get(const uint64_t key) {
	_custom_keyed_material_cache_mutex.lock();

	if (_custom_keyed_material_cache.has(key)) {
		Ref<PropMaterialCache2D> m = _custom_keyed_material_cache[key];

		m->inc_ref_count();

		_custom_keyed_material_cache_mutex.unlock();

		return m;
	}

	PropMaterialCache2D *p = Object::cast_to<PropMaterialCache2D>(ClassDB::instance(_default_prop_material_cache_class));

	if (!p) {
		ERR_PRINT("Can't instance the given PropMaterialCache2D! class_name: " + String(_default_prop_material_cache_class));
	}

	Ref<PropMaterialCache2D> m(p);

	_custom_keyed_material_cache[key] = m;

	_custom_keyed_material_cache_mutex.unlock();

	return m;
}
void PropCache2D::material_cache_custom_key_unref(const uint64_t key) {
	_custom_keyed_material_cache_mutex.lock();

	if (!_material_cache.has(key)) {
		_custom_keyed_material_cache_mutex.unlock();

		ERR_PRINT("PropCache2D::material_cache_custom_key_unref: can't find cache!");

		return;
	}

	Ref<PropMaterialCache2D> m = _custom_keyed_material_cache[key];

	m->dec_ref_count();
	if (m->get_ref_count() <= 0) {
		_custom_keyed_material_cache.erase(key);
	}

	_custom_keyed_material_cache_mutex.unlock();
}

Ref<Resource> PropCache2D::load_resource(const String &path, const String &type_hint) {
	_ResourceLoader *rl = _ResourceLoader::get_singleton();

#if VERSION_MAJOR < 4
	Ref<ResourceInteractiveLoader> resl = rl->load_interactive(path, type_hint);

	ERR_FAIL_COND_V(!resl.is_valid(), Ref<Resource>());

	resl->wait();

	return resl->get_resource();
#else
	return rl->load(path, type_hint);
#endif
}

PropCache2D::PropCache2D() {
	_instance = this;

#if TEXTURE_PACKER_PRESENT
	_default_prop_material_cache_class = GLOBAL_DEF("props/default_prop_material_cache_class", "PropMaterialCache2DPCM");
#else
	_default_prop_material_cache_class = GLOBAL_DEF("props/default_prop_material_cache_class", "PropMaterialCache2D");
#endif

#ifdef TEXTURE_PACKER_PRESENT
#if VERSION_MAJOR < 4
	_texture_flags = GLOBAL_DEF("props/texture_flags", Texture::FLAG_MIPMAPS | Texture::FLAG_FILTER);
#else
	_texture_flags = GLOBAL_DEF("props/texture_flags", 0);
#endif

	_max_atlas_size = GLOBAL_DEF("props/max_atlas_size", 1024);
	_keep_original_atlases = GLOBAL_DEF("props/keep_original_atlases", false);
	_background_color = GLOBAL_DEF("props/background_color", Color());
	_margin = GLOBAL_DEF("props/margin", 0);
#endif

	_material_paths = GLOBAL_DEF("props/material_paths", PoolStringArray());
}

PropCache2D::~PropCache2D() {
	_instance = NULL;
}

void PropCache2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_default_prop_material_cache_class"), &PropCache2D::get_default_prop_material_cache_class);
	ClassDB::bind_method(D_METHOD("set_default_prop_material_cache_class", "cls_name"), &PropCache2D::set_default_prop_material_cache_class);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "default_prop_material_cache_class"), "set_default_prop_material_cache_class", "get_default_prop_material_cache_class");

#ifdef TEXTURE_PACKER_PRESENT
	ClassDB::bind_method(D_METHOD("get_texture_flags"), &PropCache2D::get_texture_flags);
	ClassDB::bind_method(D_METHOD("set_texture_flags", "flags"), &PropCache2D::set_texture_flags);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "texture_flags", PROPERTY_HINT_FLAGS, "Mipmaps,Repeat,Filter,Anisotropic Linear,Convert to Linear,Mirrored Repeat,Video Surface"), "set_texture_flags", "get_texture_flags");

	ClassDB::bind_method(D_METHOD("get_max_atlas_size"), &PropCache2D::get_max_atlas_size);
	ClassDB::bind_method(D_METHOD("set_max_atlas_size", "size"), &PropCache2D::set_max_atlas_size);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_atlas_size"), "set_max_atlas_size", "get_max_atlas_size");

	ClassDB::bind_method(D_METHOD("get_keep_original_atlases"), &PropCache2D::get_keep_original_atlases);
	ClassDB::bind_method(D_METHOD("set_keep_original_atlases", "value"), &PropCache2D::set_keep_original_atlases);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "keep_original_atlases"), "set_keep_original_atlases", "get_keep_original_atlases");

	ClassDB::bind_method(D_METHOD("get_background_color"), &PropCache2D::get_background_color);
	ClassDB::bind_method(D_METHOD("set_background_color", "color"), &PropCache2D::set_background_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "background_color"), "set_background_color", "get_background_color");

	ClassDB::bind_method(D_METHOD("get_margin"), &PropCache2D::get_margin);
	ClassDB::bind_method(D_METHOD("set_margin", "size"), &PropCache2D::set_margin);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "margin"), "set_margin", "get_margin");
#endif

	ClassDB::bind_method(D_METHOD("material_paths_get"), &PropCache2D::material_paths_get);
	ClassDB::bind_method(D_METHOD("material_paths_set", "value"), &PropCache2D::material_paths_set);
	ADD_PROPERTY(PropertyInfo(Variant::POOL_STRING_ARRAY, "material_paths"), "material_paths_set", "material_paths_get");

	ClassDB::bind_method(D_METHOD("material_add", "value"), &PropCache2D::material_add);
	ClassDB::bind_method(D_METHOD("material_get", "index"), &PropCache2D::material_get);
	ClassDB::bind_method(D_METHOD("material_set", "index", "value"), &PropCache2D::material_set);
	ClassDB::bind_method(D_METHOD("material_remove", "index"), &PropCache2D::material_remove);
	ClassDB::bind_method(D_METHOD("material_get_num"), &PropCache2D::material_get_num);
	ClassDB::bind_method(D_METHOD("materials_clear"), &PropCache2D::materials_clear);
	ClassDB::bind_method(D_METHOD("materials_load"), &PropCache2D::materials_load);
	ClassDB::bind_method(D_METHOD("ensure_materials_loaded"), &PropCache2D::ensure_materials_loaded);

	ClassDB::bind_method(D_METHOD("materials_get"), &PropCache2D::materials_get);
	ClassDB::bind_method(D_METHOD("materials_set"), &PropCache2D::materials_set);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "materials", PROPERTY_HINT_NONE, "17/17:Material", PROPERTY_USAGE_DEFAULT, "Material"), "materials_set", "materials_get");

	ClassDB::bind_method(D_METHOD("material_cache_get", "prop"), &PropCache2D::material_cache_get);
	ClassDB::bind_method(D_METHOD("material_cache_unref", "prop"), &PropCache2D::material_cache_unref);

	ClassDB::bind_method(D_METHOD("tiled_wall_material_cache_get", "twd"), &PropCache2D::tiled_wall_material_cache_get);
	ClassDB::bind_method(D_METHOD("tiled_wall_material_cache_unref", "twd"), &PropCache2D::tiled_wall_material_cache_unref);

	ClassDB::bind_method(D_METHOD("material_cache_custom_key_get", "key"), &PropCache2D::material_cache_custom_key_get);
	ClassDB::bind_method(D_METHOD("material_cache_custom_key_unref", "key"), &PropCache2D::material_cache_custom_key_unref);

	ClassDB::bind_method(D_METHOD("load_resource", "path", "type_hint"), &PropCache2D::load_resource, "");
}
