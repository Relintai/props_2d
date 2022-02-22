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

#include "prop_2d_data_entry.h"

#include "prop_2d_data.h"

#include "core/version.h"

#if VERSION_MAJOR < 4
#include "scene/3d/spatial.h"
#else
#include "scene/3d/node_3d.h"
#define Spatial Node3D
#endif

#if TEXTURE_PACKER_PRESENT
#include "../../texture_packer/texture_packer.h"
#endif


#include "../prop_2d_mesher.h"

Transform Prop2DDataEntry::get_transform() const {
	return _transform;
}
void Prop2DDataEntry::set_transform(const Transform &value) {
	_transform = value;
}

Transform2D Prop2DDataEntry::get_transform_2d() const {
	return _transform_2d;
}
void Prop2DDataEntry::set_transform_2d(const Transform2D &value) {
	_transform_2d = value;
}

#if TEXTURE_PACKER_PRESENT
void Prop2DDataEntry::add_textures_into(Ref<TexturePacker> texture_packer) {
	if (has_method("_add_textures_into"))
		call("_add_textures_into", texture_packer);
}
#endif

bool Prop2DDataEntry::processor_handles(Node *node) {
	return call("_processor_handles", node);
}
void Prop2DDataEntry::processor_process(Ref<Prop2DData> prop_data, Node *node, const Transform2D &transform) {
	call("_processor_process", prop_data, node, transform);
}
Node *Prop2DDataEntry::processor_get_node_for(const Transform2D &transform) {
	return call("_processor_get_node_for", transform);
}
bool Prop2DDataEntry::processor_evaluate_children() {
	return call("_processor_evaluate_children");
}

bool Prop2DDataEntry::_processor_handles(Node *node) {
	return false;
}
void Prop2DDataEntry::_processor_process(Ref<Prop2DData> prop_data, Node *node, const Transform2D &transform) {
}
Node *Prop2DDataEntry::_processor_get_node_for(const Transform2D &transform) {
	return NULL;
}
bool Prop2DDataEntry::_processor_evaluate_children() {
	return true;
}

Prop2DDataEntry::Prop2DDataEntry() {
}
Prop2DDataEntry::~Prop2DDataEntry() {
}

void Prop2DDataEntry::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_transform"), &Prop2DDataEntry::get_transform);
	ClassDB::bind_method(D_METHOD("set_transform", "value"), &Prop2DDataEntry::set_transform);
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM, "transform"), "set_transform", "get_transform");

	ClassDB::bind_method(D_METHOD("get_transform_2d"), &Prop2DDataEntry::get_transform_2d);
	ClassDB::bind_method(D_METHOD("set_transform_2d", "value"), &Prop2DDataEntry::set_transform_2d);
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM2D, "transform_2d"), "set_transform_2d", "get_transform_2d");

#if TEXTURE_PACKER_PRESENT
	BIND_VMETHOD(MethodInfo("_add_textures_into", PropertyInfo(Variant::OBJECT, "texture_packer", PROPERTY_HINT_RESOURCE_TYPE, "TexturePacker")));

	ClassDB::bind_method(D_METHOD("add_textures_into", "texture_packer"), &Prop2DDataEntry::add_textures_into);
#endif

	BIND_VMETHOD(MethodInfo(PropertyInfo(Variant::BOOL, "handles"), "_processor_handles"));
	BIND_VMETHOD(MethodInfo("_processor_process",
			PropertyInfo(Variant::OBJECT, "prop_data", PROPERTY_HINT_RESOURCE_TYPE, "Prop2DData"),
			PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "Node"),
			PropertyInfo(Variant::TRANSFORM, "transform")));

	BIND_VMETHOD(MethodInfo(PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "Node"), "_processor_get_node_for",
			PropertyInfo(Variant::TRANSFORM, "transform")));

	BIND_VMETHOD(MethodInfo(PropertyInfo(Variant::BOOL, "evaluate"), "_processor_evaluate_children"));

	ClassDB::bind_method(D_METHOD("processor_handles", "node"), &Prop2DDataEntry::processor_handles);
	ClassDB::bind_method(D_METHOD("processor_process", "prop_data", "node", "transform"), &Prop2DDataEntry::processor_process);
	ClassDB::bind_method(D_METHOD("processor_get_node_for", "prop_data"), &Prop2DDataEntry::processor_get_node_for);
	ClassDB::bind_method(D_METHOD("processor_evaluate_children"), &Prop2DDataEntry::processor_evaluate_children);

	ClassDB::bind_method(D_METHOD("_processor_handles", "node"), &Prop2DDataEntry::_processor_handles);
	ClassDB::bind_method(D_METHOD("_processor_process", "prop_data", "node", "transform"), &Prop2DDataEntry::_processor_process);
	ClassDB::bind_method(D_METHOD("_processor_get_node_for", "transform"), &Prop2DDataEntry::_processor_get_node_for);
	ClassDB::bind_method(D_METHOD("_processor_evaluate_children"), &Prop2DDataEntry::_processor_evaluate_children);
}
