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

#include "prop_2d_data_scene.h"

#include "../prop_2d_scene_instance.h"
#include "prop_2d_data.h"

Ref<PackedScene> Prop2DDataScene::get_scene() {
	return _scene;
}
void Prop2DDataScene::set_scene(const Ref<PackedScene> &value) {
	_scene = value;
}

bool Prop2DDataScene::get_snap_to_mesh() {
	return _snap_to_mesh;
}
void Prop2DDataScene::set_snap_to_mesh(bool value) {
	_snap_to_mesh = value;
}

Vector3 Prop2DDataScene::get_snap_axis() {
	return _snap_axis;
}
void Prop2DDataScene::set_snap_axis(Vector3 value) {
	_snap_axis = value;
}

bool Prop2DDataScene::_processor_handles(Node *node) {
	Prop2DSceneInstance *i = Object::cast_to<Prop2DSceneInstance>(node);

	return i;
}

void Prop2DDataScene::_processor_process(Ref<Prop2DData> prop_data, Node *node, const Transform2D &transform) {
	Prop2DSceneInstance *i = Object::cast_to<Prop2DSceneInstance>(node);

	ERR_FAIL_COND(!i);

	Ref<Prop2DDataScene> l;
	l.instance();
	l->set_scene(i->get_scene());
	//l->set_transform(transform * i->get_transform());
	prop_data->add_prop(l);
}

Node *Prop2DDataScene::_processor_get_node_for(const Transform2D &transform) {
	Prop2DSceneInstance *i = memnew(Prop2DSceneInstance);

	i->set_scene(get_scene());
	//i->set_transform(get_transform());

	return i;
}

Prop2DDataScene::Prop2DDataScene() {
	_snap_to_mesh = true;
	_snap_axis = Vector3(0, 1, 0);
}
Prop2DDataScene::~Prop2DDataScene() {
	if (_scene.is_valid())
		_scene.unref();
}

void Prop2DDataScene::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_scene"), &Prop2DDataScene::get_scene);
	ClassDB::bind_method(D_METHOD("set_scene", "value"), &Prop2DDataScene::set_scene);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_scene", "get_scene");

	ClassDB::bind_method(D_METHOD("get_snap_to_mesh"), &Prop2DDataScene::get_snap_to_mesh);
	ClassDB::bind_method(D_METHOD("set_snap_to_mesh", "value"), &Prop2DDataScene::set_snap_to_mesh);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "snap_to_mesh"), "set_snap_to_mesh", "get_snap_to_mesh");

	ClassDB::bind_method(D_METHOD("get_snap_axis"), &Prop2DDataScene::get_snap_axis);
	ClassDB::bind_method(D_METHOD("set_snap_axis", "value"), &Prop2DDataScene::set_snap_axis);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "snap_axis"), "set_snap_axis", "get_snap_axis");
}
