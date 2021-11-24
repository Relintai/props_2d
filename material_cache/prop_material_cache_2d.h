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

#ifndef PROP_MATERIAL_CACHE_2D_H
#define PROP_MATERIAL_CACHE_2D_H

#include "core/version.h"

#if VERSION_MAJOR > 3
#include "core/io/resource.h"
#include "core/math/color.h"
#include "core/templates/vector.h"
#else
#include "core/color.h"
#include "core/resource.h"
#include "core/vector.h"
#endif

#include "core/math/rect2.h"
#include "scene/resources/material.h"
#include "core/os/mutex.h"

class PropData2D;

class PropMaterialCache2D : public Resource {
	GDCLASS(PropMaterialCache2D, Resource)

public:
	bool get_initialized();
	void set_initialized(const bool value);

	bool mutex_locked();
	void mutex_lock();
	void mutex_unlock();

	int get_ref_count();
	void set_ref_count(const int value);
	void inc_ref_count();
	void dec_ref_count();

	Ref<Material> material_get(const int index);
	Ref<Material> material_lod_get(const int index);
	void material_add(const Ref<Material> &value);
	void material_set(const int index, const Ref<Material> &value);
	void material_remove(const int index);
	int material_get_num() const;
	void materials_clear();

	Vector<Variant> materials_get();
	void materials_set(const Vector<Variant> &materials);

	virtual void texture_add(const Ref<Texture> &texture);
	virtual void texture_remove(const Ref<Texture> &texture);
	virtual void texture_remove_index(const int index);
	virtual void textures_clear();
	virtual int texture_count();
	virtual Ref<Texture> texture_get(const int index);
	virtual Ref<AtlasTexture> texture_get_atlas(const int index);
	virtual Ref<AtlasTexture> texture_get_atlas_tex(const Ref<Texture> &texture);
	virtual Rect2 texture_get_uv_rect(const Ref<Texture> &texture);

	void prop_add_textures(const Ref<PropData2D> &prop);
	void prop_remove_textures(const Ref<PropData2D> &prop);

	virtual void refresh_rects();

	virtual void initial_setup_default();

	void setup_material_albedo(Ref<Texture> texture);

	PropMaterialCache2D();
	~PropMaterialCache2D();

protected:
	static void _bind_methods();

	bool _locked;
	bool _initialized;

	Vector<Ref<Material>> _materials;
	Vector<Ref<Texture>> _textures;

	int _ref_count;

	Mutex _mutex;
};

#endif
