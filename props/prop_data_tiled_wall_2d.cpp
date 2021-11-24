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

#include "prop_data_tiled_wall_2d.h"

#include "../tiled_wall/tiled_wall_2d.h"
#include "../tiled_wall/tiled_wall_data_2d.h"

#include "prop_data_2d.h"

int PropDataTiledWall2D::get_width() const {
	return _width;
}
void PropDataTiledWall2D::set_width(const int value) {
	_width = value;
}

int PropDataTiledWall2D::get_heigth() const {
	return _height;
}
void PropDataTiledWall2D::set_heigth(const int value) {
	_height = value;
}

Ref<TiledWallData2D> PropDataTiledWall2D::get_data() {
	return _data;
}
void PropDataTiledWall2D::set_data(const Ref<TiledWallData2D> &data) {
	_data = data;
}

bool PropDataTiledWall2D::get_collision() const {
	return _collision;
}
void PropDataTiledWall2D::set_collision(const int value) {
	_collision = value;
}

bool PropDataTiledWall2D::_processor_handles(Node *node) {
	TiledWall2D *t = Object::cast_to<TiledWall2D>(node);

	return t;
}

void PropDataTiledWall2D::_processor_process(Ref<PropData2D> prop_data, Node *node, const Transform &transform) {
	TiledWall2D *t = Object::cast_to<TiledWall2D>(node);

	ERR_FAIL_COND(!t);

	Ref<PropDataTiledWall2D> tw;
	tw.instance();

	tw->set_width(t->get_width());
	tw->set_heigth(t->get_heigth());
	tw->set_data(t->get_data());
	tw->set_collision(t->get_collision());
	tw->set_transform(transform * t->get_transform());
	prop_data->add_prop(tw);
}

Node *PropDataTiledWall2D::_processor_get_node_for(const Transform &transform) {
	TiledWall2D *t = memnew(TiledWall2D);

	t->set_width(get_width());
	t->set_heigth(get_heigth());
	t->set_collision(get_collision());
	t->set_data(get_data());
	t->set_transform(get_transform());

	return t;
}

PropDataTiledWall2D::PropDataTiledWall2D() {
	_width = 1;
	_height = 1;
	_collision = true;
}
PropDataTiledWall2D::~PropDataTiledWall2D() {
	_data.unref();
}

void PropDataTiledWall2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_width"), &PropDataTiledWall2D::get_width);
	ClassDB::bind_method(D_METHOD("set_width", "value"), &PropDataTiledWall2D::set_width);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), "set_width", "get_width");

	ClassDB::bind_method(D_METHOD("get_heigth"), &PropDataTiledWall2D::get_heigth);
	ClassDB::bind_method(D_METHOD("set_heigth", "value"), &PropDataTiledWall2D::set_heigth);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "heigth"), "set_heigth", "get_heigth");

	ClassDB::bind_method(D_METHOD("get_data"), &PropDataTiledWall2D::get_data);
	ClassDB::bind_method(D_METHOD("set_data", "value"), &PropDataTiledWall2D::set_data);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "data", PROPERTY_HINT_RESOURCE_TYPE, "TiledWall2DData"), "set_data", "get_data");

	ClassDB::bind_method(D_METHOD("get_collision"), &PropDataTiledWall2D::get_collision);
	ClassDB::bind_method(D_METHOD("set_collision", "value"), &PropDataTiledWall2D::set_collision);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collision"), "set_collision", "get_collision");
}
