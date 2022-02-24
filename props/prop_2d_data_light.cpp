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

#include "prop_2d_data_light.h"

#include "prop_2d_data.h"

#if VERSION_MAJOR < 4
#include "scene/3d/light.h"
#else
#include "scene/3d/light_3d.h"
#define OmniLight OmniLight3D
#define Light Light3D
#endif

Color Prop2DDataLight::get_light_color() const {
	return _light_color;
}
void Prop2DDataLight::set_light_color(const Color value) {
	_light_color = value;
}

int Prop2DDataLight::get_light_size() const {
	return _light_size;
}
void Prop2DDataLight::set_light_size(const int value) {
	_light_size = value;
}

bool Prop2DDataLight::_processor_handles(Node *node) {
	OmniLight *i = Object::cast_to<OmniLight>(node);

	return i;
}

void Prop2DDataLight::_processor_process(Ref<Prop2DData> prop_data, Node *node, const Transform2D &transform, Ref<Prop2DDataEntry> entry) {
	OmniLight *i = Object::cast_to<OmniLight>(node);

	ERR_FAIL_COND(!i);

	Ref<Prop2DDataLight> l;
	l.instance();
	l->set_light_color(i->get_color());
	l->set_light_size(i->get_param(Light::PARAM_RANGE));
	//l->set_transform(transform * i->get_transform());
	prop_data->add_prop(l);
}

Node *Prop2DDataLight::_processor_get_node_for(const Transform2D &transform, Node *node) {
	OmniLight *i = memnew(OmniLight);

	i->set_color(get_light_color());
	i->set_param(Light::PARAM_RANGE, get_light_size());
	//i->set_transform(get_transform());

	return i;
}

Prop2DDataLight::Prop2DDataLight() {
	_light_size = 0;
}
Prop2DDataLight::~Prop2DDataLight() {
}

void Prop2DDataLight::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_light_color"), &Prop2DDataLight::get_light_color);
	ClassDB::bind_method(D_METHOD("set_light_color", "value"), &Prop2DDataLight::set_light_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "light_color"), "set_light_color", "get_light_color");

	ClassDB::bind_method(D_METHOD("get_light_size"), &Prop2DDataLight::get_light_size);
	ClassDB::bind_method(D_METHOD("set_light_size", "value"), &Prop2DDataLight::set_light_size);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "light_size"), "set_light_size", "get_light_size");
}
