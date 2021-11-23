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

#include "register_types.h"

#include "core/version.h"

#if VERSION_MAJOR > 3
#include "core/config/engine.h"
#else
#include "core/engine.h"
#endif

#include "tiled_wall/tiled_wall_2d.h"
#include "tiled_wall/tiled_wall_data_2d.h"

#include "props/prop_data_2d.h"
#include "props/prop_data_entry_2d.h"
#include "props/prop_data_light_2d.h"
#include "props/prop_data_prop_2d.h"
#include "props/prop_data_scene_2d.h"
#include "props/prop_data_tiled_wall_2d.h"

#if VERSION_MINOR >= 4
#include "props/prop_data_portal_2d.h"
#endif

#include "clutter/ground_clutter_2d.h"
#include "clutter/ground_clutter_foliage_2d.h"

#include "prop_ess_entity_2d.h"
#include "prop_instance_2d.h"
#include "prop_instance_merger_2d.h"

#include "prop_instance_job_2d.h"
#include "prop_instance_prop_job_2d.h"

#include "jobs/prop_mesher_job_step_2d.h"
#include "jobs/prop_texture_job_2d.h"

#include "prop_scene_instance_2d.h"

#include "singleton/prop_cache_2d.h"
#include "singleton/prop_utils_2d.h"

#include "lights/prop_light_2d.h"

#include "./editor/prop_editor_plugin_2d.h"

#include "prop_mesher_2d.h"

#include "material_cache/prop_material_cache_2d.h"

#ifdef TEXTURE_PACKER_PRESENT
#include "material_cache/prop_material_cache_pcm_2d.h"
#endif

static PropUtils *prop_utils = NULL;
static PropCache *prop_texture_cache = NULL;

void register_props_2d_types() {
	ClassDB::register_class<TiledWall>();
	ClassDB::register_class<TiledWallData>();

	ClassDB::register_class<PropLight>();

	ClassDB::register_class<PropData>();
	ClassDB::register_class<PropDataEntry>();
	ClassDB::register_class<PropDataScene>();
	ClassDB::register_class<PropDataLight>();
	ClassDB::register_class<PropDataProp>();
	ClassDB::register_class<PropDataTiledWall>();

#if VERSION_MINOR >= 4
	ClassDB::register_class<PropDataPortal>();
#endif

	ClassDB::register_class<GroundClutter>();
	ClassDB::register_class<GroundClutterFoliage>();

	ClassDB::register_class<PropMesher>();
	ClassDB::register_class<PropMesherJobStep>();

	ClassDB::register_class<PropInstance>();
	ClassDB::register_class<PropInstanceMerger>();

	ClassDB::register_class<PropESSEntity>();

	ClassDB::register_class<PropInstanceJob>();
	ClassDB::register_class<PropInstancePropJob>();

	ClassDB::register_class<PropTextureJob>();

	ClassDB::register_class<PropSceneInstance>();

	ClassDB::register_class<PropMaterialCache>();

#ifdef TEXTURE_PACKER_PRESENT
	ClassDB::register_class<PropMaterialCachePCM>();
#endif

	prop_utils = memnew(PropUtils);
	ClassDB::register_class<PropUtils>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("PropUtils", PropUtils::get_singleton()));

	prop_texture_cache = memnew(PropCache);
	ClassDB::register_class<PropCache>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("PropCache", PropCache::get_singleton()));

	Ref<PropDataLight> light_processor = Ref<PropDataLight>(memnew(PropDataLight));
	PropUtils::add_processor(light_processor);

	Ref<PropDataProp> prop_processor = Ref<PropDataProp>(memnew(PropDataProp));
	PropUtils::add_processor(prop_processor);

	Ref<PropDataScene> scene_processor = Ref<PropDataScene>(memnew(PropDataScene));
	PropUtils::add_processor(scene_processor);

#if VERSION_MINOR >= 4
	Ref<PropDataPortal> portal_processor = Ref<PropDataPortal>(memnew(PropDataPortal));
	PropUtils::add_processor(portal_processor);
#endif

	Ref<PropDataTiledWall> tiled_wall_processor = Ref<PropDataTiledWall>(memnew(PropDataTiledWall));
	PropUtils::add_processor(tiled_wall_processor);

#ifdef TOOLS_ENABLED
	EditorPlugins::add_by_type<PropEditorPlugin>();
#endif
}

void unregister_props_2d_types() {
	if (prop_utils) {
		memdelete(prop_utils);
	}

	if (prop_texture_cache) {
		memdelete(prop_texture_cache);
	}
}
