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

#include "prop_2d_data_prop.h"

#include "../prop_2d_instance.h"
#include "prop_2d_data.h"

Ref<Prop2DData> Prop2DDataProp2D::get_prop() const {
	return _prop;
}
void Prop2DDataProp2D::set_prop(const Ref<Prop2DData> value) {
	_prop = value;
}

#if TEXTURE_PACKER_PRESENT
void Prop2DDataProp2D::_add_textures_into(Ref<TexturePacker> texture_packer) {
	if (get_prop().is_valid()) {
		get_prop()->add_textures_into(texture_packer);
	}
}
#endif

bool Prop2DDataProp2D::_processor_handles(Node *node) {
	Prop2DInstance *i = Object::cast_to<Prop2DInstance>(node);

	return i;
}

void Prop2DDataProp2D::_processor_process(Ref<Prop2DData> prop_data, Node *node, const Transform2D &transform, Ref<Prop2DDataEntry> entry) {
	Prop2DInstance *i = Object::cast_to<Prop2DInstance>(node);

	ERR_FAIL_COND(!i);

	Ref<Prop2DDataProp2D> l;
	l.instance();
	l->set_prop(i->get_prop_data());
	//l->set_transform(transform * i->get_transform());
	prop_data->add_prop(l);
}

Node *Prop2DDataProp2D::_processor_get_node_for(const Transform2D &transform, Node *node) {
	Prop2DInstance *i = memnew(Prop2DInstance);

	i->set_prop_data(get_prop());
	//i->set_transform(get_transform());

	return i;
}

Prop2DDataProp2D::Prop2DDataProp2D() {
	_snap_to_mesh = false;
	_snap_axis = Vector3(0, 1, 0);
}
Prop2DDataProp2D::~Prop2DDataProp2D() {
	if (_prop.is_valid())
		_prop.unref();
}

void Prop2DDataProp2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_prop"), &Prop2DDataProp2D::get_prop);
	ClassDB::bind_method(D_METHOD("set_prop", "value"), &Prop2DDataProp2D::set_prop);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "prop", PROPERTY_HINT_RESOURCE_TYPE, "Prop2DData"), "set_prop", "get_prop");

#if TEXTURE_PACKER_PRESENT
	ClassDB::bind_method(D_METHOD("_add_textures_into", "texture_packer"), &Prop2DDataProp2D::_add_textures_into);
#endif
}
