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

#include "prop_light_2d.h"

Vector3 PropLight2D::get_position() {
	return _position;
}
void PropLight2D::set_position(const Vector3 &pos)  {
	_position = pos;
}

Color PropLight2D::get_color() const {
	return _color;
}
void PropLight2D::set_color(const Color &color) {
	_color = color;
}

float PropLight2D::get_size() const {
	return _size;
}
void PropLight2D::set_size(const float size) {
	_size = size;
}

PropLight2D::PropLight2D() {
	_size = 0;
}

PropLight2D::~PropLight2D() {
}

void PropLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_position"), &PropLight2D::get_position);
	ClassDB::bind_method(D_METHOD("set_position"), &PropLight2D::set_position);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "world_position"), "set_position", "get_position");

	ClassDB::bind_method(D_METHOD("get_color"), &PropLight2D::get_color);
	ClassDB::bind_method(D_METHOD("set_color"), &PropLight2D::set_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");

	ClassDB::bind_method(D_METHOD("get_size"), &PropLight2D::get_size);
	ClassDB::bind_method(D_METHOD("set_size"), &PropLight2D::set_size);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "size"), "set_size", "get_size");
}
