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

#include "prop_data_2d.h"

#include "prop_data_entry_2d.h"
#include "prop_data_light_2d.h"
#include "prop_data_prop_2d.h"

#if VERSION_MAJOR < 4
#include "servers/physics_server.h"
#else
#include "servers/physics_server_3d.h"

#define Shape Shape3D
#endif

int PropData2D::get_id() const {
	return _id;
}
void PropData2D::set_id(const int value) {
	_id = value;
}

bool PropData2D::get_snap_to_mesh() const {
	return _snap_to_mesh;
}
void PropData2D::set_snap_to_mesh(const bool value) {
	_snap_to_mesh = value;
}

Vector3 PropData2D::get_snap_axis() const {
	return _snap_axis;
}
void PropData2D::set_snap_axis(const Vector3 &value) {
	_snap_axis = value;
}

Ref<PropDataEntry2D> PropData2D::get_prop(const int index) const {
	ERR_FAIL_INDEX_V(index, _props.size(), Ref<PropDataEntry2D>());

	return _props.get(index);
}
void PropData2D::set_prop(const int index, const Ref<PropDataEntry2D> prop) {
	ERR_FAIL_INDEX(index, _props.size());

	_props.set(index, prop);
}
void PropData2D::add_prop(const Ref<PropDataEntry2D> prop) {
	_props.push_back(prop);
}
void PropData2D::remove_prop(const int index) {
	ERR_FAIL_INDEX(index, _props.size());

	_props.remove(index);
}

int PropData2D::get_prop_count() const {
	return _props.size();
}

Vector<Variant> PropData2D::get_props() {
	Vector<Variant> r;
	for (int i = 0; i < _props.size(); i++) {
#if VERSION_MAJOR < 4
		r.push_back(_props[i].get_ref_ptr());
#else
		r.push_back(_props[i]);
#endif
	}
	return r;
}
void PropData2D::set_props(const Vector<Variant> &props) {
	_props.clear();
	for (int i = 0; i < props.size(); i++) {
		Ref<PropDataEntry2D> prop = Ref<PropDataEntry2D>(props[i]);

		_props.push_back(prop);
	}
}

#if TEXTURE_PACKER_PRESENT
void PropData2D::add_textures_into(Ref<TexturePacker> texture_packer) {
	ERR_FAIL_COND(!texture_packer.is_valid());

	for (int i = 0; i < _props.size(); ++i) {
		Ref<PropDataEntry2D> entry = _props.get(i);

		if (entry.is_valid()) {
			entry->add_textures_into(texture_packer);
		}
	}
}
#endif

bool PropData2D::get_is_room() const {
	return _is_room;
}
void PropData2D::set_is_room(const bool value) {
	_is_room = value;
}

PoolVector3Array PropData2D::get_room_bounds() {
	return _room_bounds;
}
void PropData2D::set_room_bounds(const PoolVector3Array &bounds) {
	_room_bounds = bounds;
}

void PropData2D::copy_from(const Ref<PropData2D> &prop_data) {
	_id = prop_data->_id;
	_snap_to_mesh = prop_data->_snap_to_mesh;
	_snap_axis = prop_data->_snap_axis;

	_props.clear();

	for (int i = 0; i < prop_data->_props.size(); ++i) {
		_props.push_back(prop_data->_props[i]);
	}

	_is_room = prop_data->_is_room;
	_room_bounds = prop_data->_room_bounds;

	emit_changed();
}

PropData2D::PropData2D() {
	_id = 0;
	_snap_to_mesh = false;
	_is_room = false;
	_snap_axis = Vector3(0, -1, 0);
}
PropData2D::~PropData2D() {
	_props.clear();
}

void PropData2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_snap_to_mesh"), &PropData2D::get_snap_to_mesh);
	ClassDB::bind_method(D_METHOD("set_snap_to_mesh", "value"), &PropData2D::set_snap_to_mesh);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "snap_to_mesh"), "set_snap_to_mesh", "get_snap_to_mesh");

	ClassDB::bind_method(D_METHOD("get_snap_axis"), &PropData2D::get_snap_axis);
	ClassDB::bind_method(D_METHOD("set_snap_axis", "value"), &PropData2D::set_snap_axis);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "snap_axis"), "set_snap_axis", "get_snap_axis");

	ClassDB::bind_method(D_METHOD("get_prop", "index"), &PropData2D::get_prop);
	ClassDB::bind_method(D_METHOD("set_prop", "index", "spell"), &PropData2D::set_prop);
	ClassDB::bind_method(D_METHOD("add_prop", "prop"), &PropData2D::add_prop);
	ClassDB::bind_method(D_METHOD("remove_prop", "index"), &PropData2D::remove_prop);

	ClassDB::bind_method(D_METHOD("get_prop_count"), &PropData2D::get_prop_count);

	ClassDB::bind_method(D_METHOD("get_props"), &PropData2D::get_props);
	ClassDB::bind_method(D_METHOD("set_props", "props"), &PropData2D::set_props);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "props", PROPERTY_HINT_NONE, "17/17:PropDataEntry2D", PROPERTY_USAGE_DEFAULT, "PropDataEntry2D"), "set_props", "get_props");

#if TEXTURE_PACKER_PRESENT
	ClassDB::bind_method(D_METHOD("add_textures_into", "texture_packer"), &PropData2D::add_textures_into);
#endif

	ClassDB::bind_method(D_METHOD("get_is_room"), &PropData2D::get_is_room);
	ClassDB::bind_method(D_METHOD("set_is_room", "value"), &PropData2D::set_is_room);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_room"), "set_is_room", "get_is_room");

	ClassDB::bind_method(D_METHOD("get_room_bounds"), &PropData2D::get_room_bounds);
	ClassDB::bind_method(D_METHOD("set_room_bounds", "value"), &PropData2D::set_room_bounds);
	ADD_PROPERTY(PropertyInfo(Variant::POOL_VECTOR3_ARRAY, "room_bounds"), "set_room_bounds", "get_room_bounds");

	ClassDB::bind_method(D_METHOD("copy_from", "prop_data"), &PropData2D::copy_from);
}
