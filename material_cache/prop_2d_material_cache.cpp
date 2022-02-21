/*
Copyright (c) 2019-2022 Péter Magyar

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

#include "prop_2d_material_cache.h"

#include "../props/prop_2d_data.h"
#include "../props/prop_2d_data_prop.h"
#include "../props/prop_2d_data_tiled_wall_2d.h"
#include "../singleton/prop_2d_cache.h"
#include "../tiled_wall/tiled_wall_2d_data.h"

#if MESH_DATA_RESOURCE_PRESENT
//define PROPS_PRESENT, so things compile. That module's scsub will define this too while compiling,
//but not when included from here.
#define PROPS_2D_PRESENT 1
#include "../../mesh_data_resource/props_2d/prop_2d_data_mesh_data.h"
#endif

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

bool Prop2DMaterialCache::get_initialized() {
	return _initialized;
}
void Prop2DMaterialCache::set_initialized(const bool value) {
	_initialized = value;
}

bool Prop2DMaterialCache::mutex_locked() {
	return _locked;
}
void Prop2DMaterialCache::mutex_lock() {
	_mutex.lock();
}
void Prop2DMaterialCache::mutex_unlock() {
	_mutex.unlock();
}

int Prop2DMaterialCache::get_ref_count() {
	return _ref_count;
}
void Prop2DMaterialCache::set_ref_count(const int value) {
	_ref_count = value;
}
void Prop2DMaterialCache::inc_ref_count() {
	_ref_count += 1;
}
void Prop2DMaterialCache::dec_ref_count() {
	_ref_count -= 1;
}

//Materials
Ref<Material> Prop2DMaterialCache::material_get(const int index) {
	ERR_FAIL_INDEX_V(index, _materials.size(), Ref<Material>(NULL));

	return _materials[index];
}

Ref<Material> Prop2DMaterialCache::material_lod_get(const int index) {
	ERR_FAIL_COND_V(_materials.size() == 0, Ref<Material>(NULL));

	if (index < 0) {
		return _materials[0];
	}

	if (index >= _materials.size()) {
		return _materials[_materials.size() - 1];
	}

	return _materials[index];
}

void Prop2DMaterialCache::material_add(const Ref<Material> &value) {
	ERR_FAIL_COND(!value.is_valid());

	_materials.push_back(value);
}

void Prop2DMaterialCache::material_set(const int index, const Ref<Material> &value) {
	ERR_FAIL_INDEX(index, _materials.size());

	_materials.set(index, value);
}

void Prop2DMaterialCache::material_remove(const int index) {
	_materials.remove(index);
}

int Prop2DMaterialCache::material_get_num() const {
	return _materials.size();
}

void Prop2DMaterialCache::materials_clear() {
	_materials.clear();
}

Vector<Variant> Prop2DMaterialCache::materials_get() {
	VARIANT_ARRAY_GET(_materials);
}

void Prop2DMaterialCache::materials_set(const Vector<Variant> &materials) {
	_materials.clear();

	for (int i = 0; i < materials.size(); i++) {
		Ref<Material> material = Ref<Material>(materials[i]);

		_materials.push_back(material);
	}
}

void Prop2DMaterialCache::texture_add(const Ref<Texture> &texture) {
	_textures.push_back(texture);
}
void Prop2DMaterialCache::texture_remove(const Ref<Texture> &texture) {
	for (int i = 0; i < _textures.size(); ++i) {
		if (_textures[i] == texture) {
			_textures.remove(i);
			return;
		}
	}
}
void Prop2DMaterialCache::texture_remove_index(const int index) {
	ERR_FAIL_INDEX(index, _textures.size());

	_textures.remove(index);
}
void Prop2DMaterialCache::textures_clear() {
	_textures.clear();
}
int Prop2DMaterialCache::texture_count() {
	return _textures.size();
}
Ref<Texture> Prop2DMaterialCache::texture_get(const int index) {
	ERR_FAIL_INDEX_V(index, _textures.size(), Ref<Texture>());

	return _textures[index];
}
Ref<AtlasTexture> Prop2DMaterialCache::texture_get_atlas(const int index) {
	ERR_FAIL_INDEX_V(index, _textures.size(), Ref<AtlasTexture>());

	return texture_get_atlas_tex(_textures[index]);
}
Ref<AtlasTexture> Prop2DMaterialCache::texture_get_atlas_tex(const Ref<Texture> &texture) {
	return Ref<AtlasTexture>();
}
Rect2 Prop2DMaterialCache::texture_get_uv_rect(const Ref<Texture> &texture) {
	return Rect2(0, 0, 1, 1);
}

void Prop2DMaterialCache::prop_add_textures(const Ref<Prop2DData> &prop) {
	if (!prop.is_valid()) {
		return;
	}

	for (int i = 0; i < prop->get_prop_count(); ++i) {
#if MESH_DATA_RESOURCE_PRESENT
		Ref<Prop2D2DDataMeshData> pdm = prop->get_prop(i);

		if (pdm.is_valid()) {
			Ref<Texture> tex = pdm->get_texture();

			if (!tex.is_valid())
				continue;

			texture_add(tex);

			continue;
		}
#endif

		Ref<Prop2DDataTiledWall2D> pdtw = prop->get_prop(i);

		if (pdtw.is_valid()) {
			Ref<TiledWall2DData> twd = pdtw->get_data();

			if (!twd.is_valid())
				continue;

			twd->setup_cache(Ref<Prop2DMaterialCache>(this));

			continue;
		}

		Ref<Prop2DDataProp2D> pdp = prop->get_prop(i);

		if (pdp.is_valid()) {
			prop_add_textures(pdp->get_prop());
		}
	}
}
void Prop2DMaterialCache::prop_remove_textures(const Ref<Prop2DData> &prop) {
	if (!prop.is_valid()) {
		return;
	}

	for (int i = 0; i < prop->get_prop_count(); ++i) {
#if MESH_DATA_RESOURCE_PRESENT
		Ref<Prop2D2DDataMeshData> pdm = prop->get_prop(i);

		if (pdm.is_valid()) {
			Ref<Texture> tex = pdm->get_texture();

			if (!tex.is_valid())
				continue;

			texture_remove(tex);
		}
#endif

		Ref<Prop2DDataTiledWall2D> pdtw = prop->get_prop(i);

		if (pdtw.is_valid()) {
			Ref<TiledWall2DData> twd = pdtw->get_data();

			if (!twd.is_valid())
				continue;

			for (int j = 0; j < twd->get_texture_count(); ++j) {
				const Ref<Texture> &tex = twd->get_texture(j);

				if (tex.is_valid()) {
					texture_remove(tex);
				}
			}

			for (int j = 0; j < twd->get_flavour_texture_count(); ++j) {
				const Ref<Texture> &tex = twd->get_flavour_texture(j);

				if (tex.is_valid()) {
					texture_remove(tex);
				}
			}

			continue;
		}

		Ref<Prop2DDataProp2D> pdp = prop->get_prop(i);

		if (pdp.is_valid()) {
			prop_remove_textures(pdp);
		}
	}
}

void Prop2DMaterialCache::refresh_rects() {
	_initialized = true;
}

void Prop2DMaterialCache::initial_setup_default() {
	//Note: call only on the main thread! Shader->duplicate() can crash if done from an another thread!

	Prop2DCache *pc = Prop2DCache::get_singleton();

	pc->ensure_materials_loaded();

	int matc = pc->material_get_num();
	for (int i = 0; i < matc; ++i) {
		Ref<Material> m = pc->material_get(i);

		ERR_CONTINUE(!m.is_valid());

		Ref<Material> md = m->duplicate();

		_materials.push_back(md);
	}
}

void Prop2DMaterialCache::setup_material_albedo(Ref<Texture> texture) {
	if (has_method("_setup_material_albedo"))
		call("_setup_material_albedo", texture);
}

Prop2DMaterialCache::Prop2DMaterialCache() {
	_ref_count = 0;
	_initialized = false;
	_locked = false;
}

Prop2DMaterialCache::~Prop2DMaterialCache() {
	_materials.clear();
}

void Prop2DMaterialCache::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_initialized"), &Prop2DMaterialCache::get_initialized);
	ClassDB::bind_method(D_METHOD("set_initialized", "value"), &Prop2DMaterialCache::set_initialized);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "initialized"), "set_initialized", "get_initialized");

	ClassDB::bind_method(D_METHOD("mutex_locked"), &Prop2DMaterialCache::mutex_locked);
	ClassDB::bind_method(D_METHOD("mutex_lock"), &Prop2DMaterialCache::mutex_lock);
	ClassDB::bind_method(D_METHOD("mutex_unlock"), &Prop2DMaterialCache::mutex_unlock);

	ClassDB::bind_method(D_METHOD("get_ref_count"), &Prop2DMaterialCache::get_ref_count);
	ClassDB::bind_method(D_METHOD("set_ref_count", "value"), &Prop2DMaterialCache::set_ref_count);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mat_ref_count"), "set_ref_count", "get_ref_count");
	ClassDB::bind_method(D_METHOD("inc_ref_count"), &Prop2DMaterialCache::inc_ref_count);
	ClassDB::bind_method(D_METHOD("dec_ref_count"), &Prop2DMaterialCache::dec_ref_count);

	BIND_VMETHOD(MethodInfo("_setup_material_albedo", PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture")));

	ClassDB::bind_method(D_METHOD("material_get", "index"), &Prop2DMaterialCache::material_get);
	ClassDB::bind_method(D_METHOD("material_lod_get", "index"), &Prop2DMaterialCache::material_lod_get);
	ClassDB::bind_method(D_METHOD("material_add", "value"), &Prop2DMaterialCache::material_add);
	ClassDB::bind_method(D_METHOD("material_set", "index", "value"), &Prop2DMaterialCache::material_set);
	ClassDB::bind_method(D_METHOD("material_remove", "index"), &Prop2DMaterialCache::material_remove);
	ClassDB::bind_method(D_METHOD("material_get_num"), &Prop2DMaterialCache::material_get_num);
	ClassDB::bind_method(D_METHOD("materials_clear"), &Prop2DMaterialCache::materials_clear);

	ClassDB::bind_method(D_METHOD("materials_get"), &Prop2DMaterialCache::materials_get);
	ClassDB::bind_method(D_METHOD("materials_set"), &Prop2DMaterialCache::materials_set);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "materials", PROPERTY_HINT_NONE, "17/17:Material", PROPERTY_USAGE_DEFAULT, "Material"), "materials_set", "materials_get");

	ClassDB::bind_method(D_METHOD("texture_add", "texture"), &Prop2DMaterialCache::texture_add);
	ClassDB::bind_method(D_METHOD("texture_remove", "texture"), &Prop2DMaterialCache::texture_remove);
	ClassDB::bind_method(D_METHOD("texture_remove_index", "index"), &Prop2DMaterialCache::texture_remove_index);
	ClassDB::bind_method(D_METHOD("textures_clear"), &Prop2DMaterialCache::textures_clear);
	ClassDB::bind_method(D_METHOD("texture_count"), &Prop2DMaterialCache::texture_count);
	ClassDB::bind_method(D_METHOD("texture_get", "index"), &Prop2DMaterialCache::texture_get);
	ClassDB::bind_method(D_METHOD("texture_get_atlas", "index"), &Prop2DMaterialCache::texture_get_atlas);
	ClassDB::bind_method(D_METHOD("texture_get_atlas_tex", "index"), &Prop2DMaterialCache::texture_get_atlas_tex);
	ClassDB::bind_method(D_METHOD("texture_get_uv_rect", "texture"), &Prop2DMaterialCache::texture_get_uv_rect);

	ClassDB::bind_method(D_METHOD("prop_add_textures", "prop"), &Prop2DMaterialCache::prop_add_textures);
	ClassDB::bind_method(D_METHOD("prop_remove_textures", "prop"), &Prop2DMaterialCache::prop_remove_textures);

	ClassDB::bind_method(D_METHOD("refresh_rects"), &Prop2DMaterialCache::refresh_rects);

	ClassDB::bind_method(D_METHOD("setup_material_albedo", "texture"), &Prop2DMaterialCache::setup_material_albedo);
}
