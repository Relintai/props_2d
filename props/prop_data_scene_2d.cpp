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

#include "prop_data_scene_2d.h"

#include "../prop_scene_instance_2d.h"
#include "prop_data_2d.h"

Ref<PackedScene> PropDataScene2D::get_scene() {
	return _scene;
}
void PropDataScene2D::set_scene(const Ref<PackedScene> &value) {
	_scene = value;
}

bool PropDataScene2D::get_snap_to_mesh() {
	return _snap_to_mesh;
}
void PropDataScene2D::set_snap_to_mesh(bool value) {
	_snap_to_mesh = value;
}

Vector3 PropDataScene2D::get_snap_axis() {
	return _snap_axis;
}
void PropDataScene2D::set_snap_axis(Vector3 value) {
	_snap_axis = value;
}

bool PropDataScene2D::_processor_handles(Node *node) {
	PropSceneInstance2D *i = Object::cast_to<PropSceneInstance2D>(node);

	return i;
}

void PropDataScene2D::_processor_process(Ref<PropData2D> prop_data, Node *node, const Transform &transform) {
	PropSceneInstance2D *i = Object::cast_to<PropSceneInstance2D>(node);

	ERR_FAIL_COND(!i);

	Ref<PropDataScene2D> l;
	l.instance();
	l->set_scene(i->get_scene());
	l->set_transform(transform * i->get_transform());
	prop_data->add_prop(l);
}

Node *PropDataScene2D::_processor_get_node_for(const Transform &transform) {
	PropSceneInstance2D *i = memnew(PropSceneInstance2D);

	i->set_scene(get_scene());
	i->set_transform(get_transform());

	return i;
}

PropDataScene2D::PropDataScene2D() {
	_snap_to_mesh = true;
	_snap_axis = Vector3(0, 1, 0);
}
PropDataScene2D::~PropDataScene2D() {
	if (_scene.is_valid())
		_scene.unref();
}

void PropDataScene2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_scene"), &PropDataScene2D::get_scene);
	ClassDB::bind_method(D_METHOD("set_scene", "value"), &PropDataScene2D::set_scene);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_scene", "get_scene");

	ClassDB::bind_method(D_METHOD("get_snap_to_mesh"), &PropDataScene2D::get_snap_to_mesh);
	ClassDB::bind_method(D_METHOD("set_snap_to_mesh", "value"), &PropDataScene2D::set_snap_to_mesh);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "snap_to_mesh"), "set_snap_to_mesh", "get_snap_to_mesh");

	ClassDB::bind_method(D_METHOD("get_snap_axis"), &PropDataScene2D::get_snap_axis);
	ClassDB::bind_method(D_METHOD("set_snap_axis", "value"), &PropDataScene2D::set_snap_axis);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "snap_axis"), "set_snap_axis", "get_snap_axis");
}
