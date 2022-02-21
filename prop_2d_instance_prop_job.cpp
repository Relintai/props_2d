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

#include "prop_2d_instance_prop_job.h"

#include "core/version.h"

#if VERSION_MAYOR > 3
#define GET_WORLD get_world_3d
#else
#define GET_WORLD get_world
#endif

#include "jobs/prop_2d_mesher_job_step.h"
#include "lights/prop_2d_light.h"
#include "material_cache/prop_2d_material_cache.h"
#include "prop_2d_instance.h"
#include "prop_2d_instance_merger.h"
#include "prop_2d_mesher.h"
#include "scene/resources/shape.h"
#include "singleton/prop_2d_cache.h"

#ifdef MESH_DATA_RESOURCE_PRESENT
#include "../mesh_data_resource/mesh_data_resource.h"
#endif

#ifdef MESH_UTILS_PRESENT
#include "../mesh_utils/fast_quadratic_mesh_simplifier.h"
#endif

#if TEXTURE_PACKER_PRESENT
#include "../texture_packer/texture_packer.h"
#endif

#if MESH_DATA_RESOURCE_PRESENT
//define PROPS_PRESENT, so things compile. That module's scsub will define this too while compiling,
//but not when included from here.
#define PROPS_2D_PRESENT 1
#include "../mesh_data_resource/props_2d/prop_2d_data_mesh_data.h"
#endif

#include "props/prop_2d_data_tiled_wall_2d.h"
#include "tiled_wall/tiled_wall_2d_data.h"

Ref<Prop2DMaterialCache> Prop2DInstanceProp2DJob::get_material_cache() {
	return _material_cache;
}
void Prop2DInstanceProp2DJob::set_material_cache(const Ref<Prop2DMaterialCache> &cache) {
	_material_cache = cache;
}

Ref<Prop2DMesherJobStep> Prop2DInstanceProp2DJob::get_jobs_step(int index) const {
	ERR_FAIL_INDEX_V(index, _job_steps.size(), Ref<Prop2DMesherJobStep>());

	return _job_steps.get(index);
}
void Prop2DInstanceProp2DJob::set_jobs_step(int index, const Ref<Prop2DMesherJobStep> &step) {
	ERR_FAIL_INDEX(index, _job_steps.size());

	_job_steps.set(index, step);
}
void Prop2DInstanceProp2DJob::remove_jobs_step(const int index) {
	ERR_FAIL_INDEX(index, _job_steps.size());

	_job_steps.remove(index);
}
void Prop2DInstanceProp2DJob::add_jobs_step(const Ref<Prop2DMesherJobStep> &step) {
	_job_steps.push_back(step);
}
int Prop2DInstanceProp2DJob::get_jobs_step_count() const {
	return _job_steps.size();
}

void Prop2DInstanceProp2DJob::add_collision_shape(const Ref<Shape> &shape, const Transform &transform, const bool owns_shape) {
	CollisionShapeEntry e;

	e.shape = shape;
	e.transform = transform;
	e.owns_shape = owns_shape;

	_collision_shapes.push_back(e);
}
void Prop2DInstanceProp2DJob::clear_collision_shapes() {
	_collision_shapes.clear();
}

Prop2DInstanceMerger *Prop2DInstanceProp2DJob::get_prop_instace() {
	return _prop_instace;
}
void Prop2DInstanceProp2DJob::set_prop_instace(Prop2DInstanceMerger *prop) {
	_prop_instace = prop;
	_instance = prop;
}
void Prop2DInstanceProp2DJob::set_prop_instace_bind(Node *prop) {
	set_prop_instace(Object::cast_to<Prop2DInstanceMerger>(prop));
}

Ref<Prop2DMesher> Prop2DInstanceProp2DJob::get_prop_mesher() const {
	return _prop_mesher;
}
void Prop2DInstanceProp2DJob::set_prop_mesher(const Ref<Prop2DMesher> &mesher) {
	_prop_mesher = mesher;
}

#if MESH_DATA_RESOURCE_PRESENT
void Prop2DInstanceProp2DJob::add_mesh(const Ref<Prop2D2DDataMeshData> &mesh_data, const Transform &base_transform) {
	PMDREntry e;
	e.mesh_data = mesh_data;
	e.base_transform = base_transform;

	_prop_mesh_datas.push_back(e);
}

void Prop2DInstanceProp2DJob::clear_meshes() {
	_prop_mesh_datas.clear();
}
#endif

void Prop2DInstanceProp2DJob::add_tiled_wall(const Ref<Prop2DDataTiledWall2D> &data, const Transform &base_transform) {
	PTWEntry e;
	e.data = data;
	e.base_transform = base_transform;

	_prop_tiled_wall_datas.push_back(e);
}

void Prop2DInstanceProp2DJob::clear_tiled_walls() {
	_prop_tiled_wall_datas.clear();
}

void Prop2DInstanceProp2DJob::add_light(const Ref<Prop2DLight> &light) {
	_prop_mesher->add_light(light);
}
void Prop2DInstanceProp2DJob::clear_lights() {
	_prop_mesher->clear_lights();
}

void Prop2DInstanceProp2DJob::_physics_process(float delta) {
	if (_phase == 0)
		phase_physics_process();
}

void Prop2DInstanceProp2DJob::_execute_phase() {
	if (!_material_cache.is_valid()) {
		ERR_PRINT("!Prop2DInstanceProp2DJob::_execute_phase(): !_material_cache.is_valid()");
		//reset_meshes();
		set_complete(true); //So threadpool knows it's done
		finished();
	}

#ifdef MESH_DATA_RESOURCE_PRESENT
	if (_prop_mesh_datas.size() == 0 && _prop_tiled_wall_datas.size() == 0) {
		//reset_meshes();
		set_complete(true);
		finished();
		return;
	}
#endif

	if (_phase == 1) {
		phase_setup_cache();
	} else if (_phase == 2) {
		phase_prop();
	} else if (_phase == 3) {
		phase_steps();
	} else if (_phase > 3) {
		set_complete(true); //So threadpool knows it's done
		finished();
		ERR_FAIL_MSG("Prop2DInstanceProp2DJob: _phase is too high!");
	}
}

void Prop2DInstanceProp2DJob::_reset() {
	Prop2DInstanceJob::_reset();

	_build_done = false;
	_phase = 0;

	_current_mesh = 0;
	_current_job_step = 0;

	reset_stages();

	if (_prop_mesher.is_valid()) {
		_prop_mesher->reset();
	}

	_prop_tiled_wall_datas.clear();

	_prop_mesh_datas.clear();
	clear_collision_shapes();

	set_build_phase_type(BUILD_PHASE_TYPE_PHYSICS_PROCESS);
}

void Prop2DInstanceProp2DJob::phase_physics_process() {
	//TODO this should only update the differences
	//for (int i = 0; i < _prop_instace->collider_get_num(); ++i) {
	//	PhysicsServer::get_singleton()->free(_prop_instace->collider_body_get(i));
	//}

	_prop_instace->free_colliders();
	_prop_instace->colliders_clear();

	for (int i = 0; i < _collision_shapes.size(); ++i) {
		CollisionShapeEntry &e = _collision_shapes.write[i];

		if (!e.shape.is_valid()) {
			continue;
		}

		RID body = PhysicsServer::get_singleton()->body_create(PhysicsServer::BODY_MODE_STATIC);

		PhysicsServer::get_singleton()->body_add_shape(body, e.shape->get_rid());

		//TODO store the layer mask somewhere
		PhysicsServer::get_singleton()->body_set_collision_layer(body, _prop_instace->get_collision_layer());
		PhysicsServer::get_singleton()->body_set_collision_mask(body, _prop_instace->get_collision_mask());

		if (_prop_instace->is_inside_tree() && _prop_instace->is_inside_world()) {
			Ref<World> world = _prop_instace->GET_WORLD();

			if (world.is_valid() && world->get_space() != RID()) {
				PhysicsServer::get_singleton()->body_set_space(body, world->get_space());
			}
		}

		//PhysicsServer::get_singleton()->body_set_state(body, PhysicsServer::BODY_STATE_TRANSFORM, e.transform);

		_prop_instace->collider_add(e.transform, e.shape, e.shape->get_rid(), body, e.owns_shape);
	}

#if TOOLS_ENABLED
	if (SceneTree::get_singleton()->is_debugging_collisions_hint() && _prop_instace->collider_get_num() > 0) {
		_prop_instace->draw_debug_mdr_colliders();
	}
#endif

	set_build_phase_type(BUILD_PHASE_TYPE_NORMAL);
	next_phase();
}

void Prop2DInstanceProp2DJob::phase_setup_cache() {
	if (should_do()) {
		if (!_material_cache->get_initialized()) {
			_material_cache->mutex_lock();

			//check again, this thread might have gotten here after an another one already did the initialization!
			if (!_material_cache->get_initialized()) {
				//this will set up materials, and settings
				//Can only be called from the main thread!
				//Merger calls this
				//_material_cache->initial_setup_default();

				_material_cache->prop_add_textures(_prop_instace->get_prop_data());

				_material_cache->refresh_rects();
			}

			_material_cache->mutex_unlock();
		}

		if (should_return()) {
			return;
		}
	}

	reset_stages();
	next_phase();
}

void Prop2DInstanceProp2DJob::phase_prop() {
	if (!_prop_mesher.is_valid()) {
		set_complete(true); //So threadpool knows it's done
		return;
	}

	if (should_do()) {
		if (_prop_mesh_datas.size() == 0 && _prop_tiled_wall_datas.size() == 0) {
			//reset_meshes();
			reset_stages();
			set_complete(true); //So threadpool knows it's done
			return;
		}

#ifdef MESH_DATA_RESOURCE_PRESENT
		for (int i = 0; i < _prop_mesh_datas.size(); ++i) {
			PMDREntry &e = _prop_mesh_datas.write[i];

			Ref<Prop2D2DDataMeshData> pmd = e.mesh_data;

			Ref<MeshDataResource> mesh = pmd->get_mesh();
			Ref<Texture> tex = pmd->get_texture();
			Transform t = pmd->get_transform();

			Rect2 uvr = _material_cache->texture_get_uv_rect(tex);

			_prop_mesher->add_mesh_data_resource_transform(mesh, t, uvr);
		}
#endif

		for (int i = 0; i < _prop_tiled_wall_datas.size(); ++i) {
			PTWEntry &e = _prop_tiled_wall_datas.write[i];

			Ref<Prop2DDataTiledWall2D> pdtw = e.data;
			//Transform t = pdtw->get_transform();
			Transform t = e.base_transform;

			_prop_mesher->add_tiled_wall_simple(pdtw->get_width(), pdtw->get_heigth(), t, pdtw->get_data(), _material_cache);
		}

		if (_prop_mesher->get_vertex_count() == 0) {
			//reset_meshes();

			reset_stages();

			set_complete(true); //So threadpool knows it's done
			return;
		}

		if (should_return()) {
			return;
		}
	}

	if (should_do()) {
		if ((_prop_mesher->get_build_flags() & Prop2DMesher::BUILD_FLAG_USE_LIGHTING) != 0) {
			_prop_mesher->bake_colors();
		}

		if (should_return()) {
			return;
		}
	}

	reset_stages();
	next_phase();
}

void Prop2DInstanceProp2DJob::phase_steps() {
	ERR_FAIL_COND(!_prop_mesher.is_valid());

	if (should_return()) {
		return;
	}

	if (_prop_mesher->get_vertex_count() == 0) {
		reset_stages();
		//next_phase();
		set_complete(true); //So threadpool knows it's done
		finished();
		return;
	}

	//set up the meshes
	if (should_do()) {
		if (_prop_instace->mesh_get_num() == 0) {
			//need to allocate the meshes

			//first count how many we need
			int count = 0;
			for (int i = 0; i < _job_steps.size(); ++i) {
				Ref<Prop2DMesherJobStep> step = _job_steps[i];

				ERR_FAIL_COND(!step.is_valid());

				switch (step->get_job_type()) {
					case Prop2DMesherJobStep::TYPE_NORMAL:
						++count;
						break;
					case Prop2DMesherJobStep::TYPE_NORMAL_LOD:
						++count;
						break;
					case Prop2DMesherJobStep::TYPE_DROP_UV2:
						++count;
						break;
					case Prop2DMesherJobStep::TYPE_MERGE_VERTS:
						++count;
						break;
					case Prop2DMesherJobStep::TYPE_BAKE_TEXTURE:
						++count;
						break;
					case Prop2DMesherJobStep::TYPE_SIMPLIFY_MESH:
#ifdef MESH_UTILS_PRESENT
						count += step->get_simplification_steps();
#endif
						break;
					default:
						break;
				}
			}

			//allocate
			if (count > 0) {
				_prop_instace->meshes_create(count);
			}

		} else {
			//we have the meshes, just clear
			int count = _prop_instace->mesh_get_num();

			for (int i = 0; i < count; ++i) {
				RID mesh_rid = _prop_instace->mesh_get(i);

				if (VS::get_singleton()->mesh_get_surface_count(mesh_rid) > 0)
#if !GODOT4
					VS::get_singleton()->mesh_remove_surface(mesh_rid, 0);
#else
					VS::get_singleton()->mesh_clear(mesh_rid);
#endif
			}
		}
	}

	for (; _current_job_step < _job_steps.size();) {
		Ref<Prop2DMesherJobStep> step = _job_steps[_current_job_step];

		ERR_FAIL_COND(!step.is_valid());

		switch (step->get_job_type()) {
			case Prop2DMesherJobStep::TYPE_NORMAL:
				step_type_normal();
				break;
			case Prop2DMesherJobStep::TYPE_NORMAL_LOD:
				step_type_normal_lod();
				break;
			case Prop2DMesherJobStep::TYPE_DROP_UV2:
				step_type_drop_uv2();
				break;
			case Prop2DMesherJobStep::TYPE_MERGE_VERTS:
				step_type_merge_verts();
				break;
			case Prop2DMesherJobStep::TYPE_BAKE_TEXTURE:
				step_type_bake_texture();
				break;
			case Prop2DMesherJobStep::TYPE_SIMPLIFY_MESH:
				step_type_simplify_mesh();
				break;
			case Prop2DMesherJobStep::TYPE_OTHER:
				//do nothing
				break;
		}

		++_current_job_step;

		if (should_return()) {
			return;
		}
	}

	reset_stages();
	//next_phase();
	set_complete(true); //So threadpool knows it's done
	finished();
}

void Prop2DInstanceProp2DJob::step_type_normal() {
	//TODO add a lighting generation step

	temp_mesh_arr = _prop_mesher->build_mesh();

	RID mesh_rid = _prop_instace->mesh_get(_current_mesh);

	VS::get_singleton()->mesh_add_surface_from_arrays(mesh_rid, VisualServer::PRIMITIVE_TRIANGLES, temp_mesh_arr);

	Ref<Material> lmat = _material_cache->material_lod_get(_current_mesh);

	if (lmat.is_valid()) {
		VisualServer::get_singleton()->mesh_surface_set_material(mesh_rid, 0, lmat->get_rid());
	}

	++_current_mesh;
}

void Prop2DInstanceProp2DJob::step_type_normal_lod() {
	print_error("Error: step_type_normal_lod doesn't work for TerraProp2DJobs!");

	++_current_mesh;
}

void Prop2DInstanceProp2DJob::step_type_drop_uv2() {
	RID mesh_rid = _prop_instace->mesh_get(_current_mesh);

	temp_mesh_arr[VisualServer::ARRAY_TEX_UV2] = Variant();

	VisualServer::get_singleton()->mesh_add_surface_from_arrays(mesh_rid, VisualServer::PRIMITIVE_TRIANGLES, temp_mesh_arr);

	Ref<Material> lmat = _material_cache->material_lod_get(_current_mesh);

	if (lmat.is_valid()) {
		VisualServer::get_singleton()->mesh_surface_set_material(mesh_rid, 0, lmat->get_rid());
	}

	++_current_mesh;
}

void Prop2DInstanceProp2DJob::step_type_merge_verts() {
	Array temp_mesh_arr2 = merge_mesh_array(temp_mesh_arr);
	temp_mesh_arr = temp_mesh_arr2;

	RID mesh_rid = _prop_instace->mesh_get(_current_mesh);

	VisualServer::get_singleton()->mesh_add_surface_from_arrays(mesh_rid, VisualServer::PRIMITIVE_TRIANGLES, temp_mesh_arr);

	Ref<Material> lmat = _material_cache->material_lod_get(_current_mesh);

	if (lmat.is_valid()) {
		VisualServer::get_singleton()->mesh_surface_set_material(mesh_rid, 0, lmat->get_rid());
	}

	++_current_mesh;
}

void Prop2DInstanceProp2DJob::step_type_bake_texture() {
	Ref<ShaderMaterial> mat = _material_cache->material_lod_get(0);
	Ref<SpatialMaterial> spmat = _material_cache->material_lod_get(0);
	Ref<Texture> tex;

	if (mat.is_valid()) {
		tex = mat->get_shader_param("texture_albedo");
	} else if (spmat.is_valid()) {
		tex = spmat->get_texture(SpatialMaterial::TEXTURE_ALBEDO);
	}

	if (tex.is_valid()) {
		temp_mesh_arr = bake_mesh_array_uv(temp_mesh_arr, tex);
		temp_mesh_arr[VisualServer::ARRAY_TEX_UV] = Variant();

		RID mesh_rid = _prop_instace->mesh_get(_current_mesh);

		VisualServer::get_singleton()->mesh_add_surface_from_arrays(mesh_rid, VisualServer::PRIMITIVE_TRIANGLES, temp_mesh_arr);

		Ref<Material> lmat = _material_cache->material_lod_get(_current_mesh);

		if (lmat.is_valid()) {
			VisualServer::get_singleton()->mesh_surface_set_material(mesh_rid, 0, lmat->get_rid());
		}
	}

	++_current_mesh;
}

void Prop2DInstanceProp2DJob::step_type_simplify_mesh() {
#ifdef MESH_UTILS_PRESENT

	Ref<Prop2DMesherJobStep> step = _job_steps[_current_job_step];
	ERR_FAIL_COND(!step.is_valid());
	Ref<FastQuadraticMeshSimplifier> fqms = step->get_fqms();
	ERR_FAIL_COND(!fqms.is_valid());

	fqms->initialize(temp_mesh_arr);

	for (int i = 0; i < step->get_simplification_steps(); ++i) {
		fqms->simplify_mesh(temp_mesh_arr.size() * step->get_simplification_step_ratio(), step->get_simplification_agressiveness());
		temp_mesh_arr = fqms->get_arrays();

		RID mesh_rid = _prop_instace->mesh_get(_current_mesh);

		VisualServer::get_singleton()->mesh_add_surface_from_arrays(mesh_rid, VisualServer::PRIMITIVE_TRIANGLES, temp_mesh_arr);

		Ref<Material> lmat = _material_cache->material_lod_get(_current_mesh);

		if (lmat.is_valid()) {
			VisualServer::get_singleton()->mesh_surface_set_material(mesh_rid, 0, lmat->get_rid());
		}

		++_current_mesh;
	}

#endif
}

Array Prop2DInstanceProp2DJob::merge_mesh_array(Array arr) const {
	ERR_FAIL_COND_V(arr.size() != VisualServer::ARRAY_MAX, arr);

	PoolVector3Array verts = arr[VisualServer::ARRAY_VERTEX];
	PoolVector3Array normals = arr[VisualServer::ARRAY_NORMAL];
	PoolVector2Array uvs = arr[VisualServer::ARRAY_TEX_UV];
	PoolColorArray colors = arr[VisualServer::ARRAY_COLOR];
	PoolIntArray indices = arr[VisualServer::ARRAY_INDEX];

	bool has_normals = normals.size() > 0;
	bool has_uvs = uvs.size() > 0;
	bool has_colors = colors.size() > 0;

	int i = 0;
	while (i < verts.size()) {
		Vector3 v = verts[i];

		Array equals;
		for (int j = i + 1; j < verts.size(); ++j) {
			Vector3 vc = verts[j];

			if (Math::is_equal_approx(v.x, vc.x) && Math::is_equal_approx(v.y, vc.y) && Math::is_equal_approx(v.z, vc.z))
				equals.push_back(j);
		}

		for (int k = 0; k < equals.size(); ++k) {
			int rem = equals[k];
			int remk = rem - k;

			verts.remove(remk);

			if (has_normals)
				normals.remove(remk);
			if (has_uvs)
				uvs.remove(remk);
			if (has_colors)
				colors.remove(remk);

			for (int j = 0; j < indices.size(); ++j) {
				int indx = indices[j];

				if (indx == remk)
					indices.set(j, i);
				else if (indx > remk)
					indices.set(j, indx - 1);
			}
		}

		++i;
	}

	arr[VisualServer::ARRAY_VERTEX] = verts;

	if (has_normals)
		arr[VisualServer::ARRAY_NORMAL] = normals;
	if (has_uvs)
		arr[VisualServer::ARRAY_TEX_UV] = uvs;
	if (has_colors)
		arr[VisualServer::ARRAY_COLOR] = colors;

	arr[VisualServer::ARRAY_INDEX] = indices;

	return arr;
}
Array Prop2DInstanceProp2DJob::bake_mesh_array_uv(Array arr, Ref<Texture> tex, const float mul_color) const {
	ERR_FAIL_COND_V(arr.size() != VisualServer::ARRAY_MAX, arr);
	ERR_FAIL_COND_V(!tex.is_valid(), arr);

	Ref<Image> img = tex->get_data();

	ERR_FAIL_COND_V(!img.is_valid(), arr);

	Vector2 imgsize = img->get_size();

	PoolVector2Array uvs = arr[VisualServer::ARRAY_TEX_UV];
	PoolColorArray colors = arr[VisualServer::ARRAY_COLOR];

	if (colors.size() < uvs.size())
		colors.resize(uvs.size());

#if !GODOT4
	img->lock();
#endif

	for (int i = 0; i < uvs.size(); ++i) {
		Vector2 uv = uvs[i];
		uv *= imgsize;

		int ux = static_cast<int>(CLAMP(uv.x, 0, imgsize.x - 1));
		int uy = static_cast<int>(CLAMP(uv.y, 0, imgsize.y - 1));

		Color c = img->get_pixel(ux, uy);

		colors.set(i, colors[i] * c * mul_color);
	}

#if !GODOT4
	img->unlock();
#endif

	arr[VisualServer::ARRAY_COLOR] = colors;

	return arr;
}

void Prop2DInstanceProp2DJob::reset_meshes() {
	if (!_prop_instace) {
		return;
	}

	//we have meshes, clear
	if (_prop_instace->mesh_get_num() != 0) {
		int count = _prop_instace->mesh_get_num();

		for (int i = 0; i < count; ++i) {
			RID mesh_rid = _prop_instace->mesh_get(i);

			if (VS::get_singleton()->mesh_get_surface_count(mesh_rid) > 0)
#if !GODOT4
				VS::get_singleton()->mesh_remove_surface(mesh_rid, 0);
#else
				VS::get_singleton()->mesh_clear(mesh_rid);
#endif
		}
	}
}

Prop2DInstanceProp2DJob::Prop2DInstanceProp2DJob() {
	set_build_phase_type(BUILD_PHASE_TYPE_PHYSICS_PROCESS);

	_prop_instace = NULL;

	_current_job_step = 0;

	//todo allocate this in a virtual method
	_prop_mesher.instance();
	_prop_mesher->set_build_flags(Prop2DMesher::BUILD_FLAG_USE_LIGHTING | Prop2DMesher::BUILD_FLAG_USE_AO | Prop2DMesher::BUILD_FLAG_USE_RAO | Prop2DMesher::BUILD_FLAG_BAKE_LIGHTS);
}

Prop2DInstanceProp2DJob::~Prop2DInstanceProp2DJob() {
}

void Prop2DInstanceProp2DJob::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_material_cache"), &Prop2DInstanceProp2DJob::get_material_cache);
	ClassDB::bind_method(D_METHOD("set_material_cache", "packer"), &Prop2DInstanceProp2DJob::set_material_cache);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material_cache", PROPERTY_HINT_RESOURCE_TYPE, "Prop2DMaterialCache", 0), "set_material_cache", "get_material_cache");

	ClassDB::bind_method(D_METHOD("get_jobs_step", "index"), &Prop2DInstanceProp2DJob::get_jobs_step);
	ClassDB::bind_method(D_METHOD("set_jobs_step", "index", "mesher"), &Prop2DInstanceProp2DJob::set_jobs_step);
	ClassDB::bind_method(D_METHOD("remove_jobs_step", "index"), &Prop2DInstanceProp2DJob::remove_jobs_step);
	ClassDB::bind_method(D_METHOD("add_jobs_step", "mesher"), &Prop2DInstanceProp2DJob::add_jobs_step);
	ClassDB::bind_method(D_METHOD("get_jobs_step_count"), &Prop2DInstanceProp2DJob::get_jobs_step_count);

	ClassDB::bind_method(D_METHOD("get_prop_mesher"), &Prop2DInstanceProp2DJob::get_prop_mesher);
	ClassDB::bind_method(D_METHOD("set_prop_mesher", "mesher"), &Prop2DInstanceProp2DJob::set_prop_mesher);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "prop_mesher", PROPERTY_HINT_RESOURCE_TYPE, "Prop2DMesher", 0), "set_prop_mesher", "get_prop_mesher");

	ClassDB::bind_method(D_METHOD("add_light", "light"), &Prop2DInstanceProp2DJob::add_light);
	ClassDB::bind_method(D_METHOD("clear_lights"), &Prop2DInstanceProp2DJob::clear_lights);

	ClassDB::bind_method(D_METHOD("_physics_process", "delta"), &Prop2DInstanceProp2DJob::_physics_process);
}
