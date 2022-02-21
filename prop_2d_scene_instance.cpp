#include "prop_2d_scene_instance.h"

#include "core/version.h"

#if VERSION_MAJOR > 3
#include "core/config/engine.h"
#else
#include "core/engine.h"
#endif

Ref<PackedScene> Prop2DSceneInstance::get_scene() {
	return _scene;
}
void Prop2DSceneInstance::set_scene(const Ref<PackedScene> &data) {
	if (_scene == data)
		return;

	_scene = data;

	build();
}

bool Prop2DSceneInstance::get_snap_to_mesh() const {
	return _snap_to_mesh;
}
void Prop2DSceneInstance::set_snap_to_mesh(const bool value) {
	_snap_to_mesh = value;
}

Vector3 Prop2DSceneInstance::get_snap_axis() const {
	return _snap_axis;
}
void Prop2DSceneInstance::set_snap_axis(const Vector3 &value) {
	_snap_axis = value;
}

void Prop2DSceneInstance::build() {
	if (!is_inside_tree()) {
		return;
	}

	for (int i = 0; i < get_child_count(); ++i) {
		Node *n = get_child(i);

		//this way we won't delete the user's nodes
		if (n->get_owner() == NULL) {
			n->queue_delete();
		}
	}

	if (!_scene.is_valid())
		return;

	Node *n = _scene->instance();

	add_child(n);

	//if (Engine::get_singleton()->is_editor_hint())
	//	n->set_owner(get_tree()->get_edited_scene_root());
}

Prop2DSceneInstance::Prop2DSceneInstance() {
	_snap_to_mesh = false;
	_snap_axis = Vector3(0, -1, 0);
}
Prop2DSceneInstance::~Prop2DSceneInstance() {
	_scene.unref();
}

void Prop2DSceneInstance::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			build();
		}
	}
}

void Prop2DSceneInstance::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_scene"), &Prop2DSceneInstance::get_scene);
	ClassDB::bind_method(D_METHOD("set_scene", "value"), &Prop2DSceneInstance::set_scene);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_scene", "get_scene");

	ClassDB::bind_method(D_METHOD("get_snap_to_mesh"), &Prop2DSceneInstance::get_snap_to_mesh);
	ClassDB::bind_method(D_METHOD("set_snap_to_mesh", "value"), &Prop2DSceneInstance::set_snap_to_mesh);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "snap_to_mesh"), "set_snap_to_mesh", "get_snap_to_mesh");

	ClassDB::bind_method(D_METHOD("get_snap_axis"), &Prop2DSceneInstance::get_snap_axis);
	ClassDB::bind_method(D_METHOD("set_snap_axis", "value"), &Prop2DSceneInstance::set_snap_axis);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "snap_axis"), "set_snap_axis", "get_snap_axis");

	ClassDB::bind_method(D_METHOD("build"), &Prop2DSceneInstance::build);
}
