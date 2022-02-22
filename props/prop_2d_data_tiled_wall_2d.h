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

#ifndef PROP_2D_DATA_TILED_WALL_H
#define PROP_2D_DATA_TILED_WALL_H

#include "core/math/vector3.h"
#include "prop_2d_data_entry.h"

class TiledWall2DData;

class Prop2DDataTiledWall2D : public Prop2DDataEntry {
	GDCLASS(Prop2DDataTiledWall2D, Prop2DDataEntry);

public:
	int get_width() const;
	void set_width(const int value);

	int get_heigth() const;
	void set_heigth(const int value);

	Ref<TiledWall2DData> get_data();
	void set_data(const Ref<TiledWall2DData> &data);

	bool get_collision() const;
	void set_collision(const int value);

	bool _processor_handles(Node *node);
	void _processor_process(Ref<Prop2DData> prop_data, Node *node, const Transform2D &transform);
	Node *_processor_get_node_for(const Transform2D &transform);

	Prop2DDataTiledWall2D();
	~Prop2DDataTiledWall2D();

protected:
	static void _bind_methods();

private:
	int _width;
	int _height;
	bool _collision;

	Ref<TiledWall2DData> _data;
};

#endif
