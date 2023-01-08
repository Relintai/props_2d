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

#include "register_types.h"

#include "core/version.h"

#if VERSION_MAJOR > 3
#include "core/config/engine.h"
#else
#include "core/engine.h"
#endif

#include "tiled_wall/tiled_wall_2d.h"
#include "tiled_wall/tiled_wall_2d_data.h"

#include "props/prop_2d_data.h"
#include "props/prop_2d_data_entry.h"
#include "props/prop_2d_data_light.h"
#include "props/prop_2d_data_prop.h"
#include "props/prop_2d_data_scene.h"
#include "props/prop_2d_data_sprite.h"
#include "props/prop_2d_data_tiled_wall_2d.h"

#if VERSION_MINOR >= 4
#include "props/prop_2d_data_portal.h"
#endif

#include "clutter/ground_clutter_2d.h"
#include "clutter/ground_clutter_2d_foliage.h"

#include "prop_2d_ess_entity.h"
#include "prop_2d_instance.h"
#include "prop_2d_instance_merger.h"

#include "prop_2d_instance_job.h"
#include "prop_2d_instance_prop_job.h"

#include "jobs/prop_2d_texture_job.h"

#include "prop_2d_scene_instance.h"

#include "singleton/prop_2d_cache.h"
#include "singleton/prop_2d_utils.h"

#include "lights/prop_2d_light.h"

#include "./editor/prop_2d_editor_plugin.h"

#include "prop_2d_mesher.h"

#include "material_cache/prop_2d_material_cache.h"

#ifdef TEXTURE_PACKER_PRESENT
#include "material_cache/prop_2d_material_cache_pcm.h"
#endif

static Prop2DUtils *prop_utils = NULL;
static Prop2DCache *prop_texture_cache = NULL;

void initialize_props_2d_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(TiledWall2D);
		GDREGISTER_CLASS(TiledWall2DData);

		GDREGISTER_CLASS(Prop2DLight);

		GDREGISTER_CLASS(Prop2DData);
		GDREGISTER_CLASS(Prop2DDataEntry);
		GDREGISTER_CLASS(Prop2DDataScene);
		GDREGISTER_CLASS(Prop2DDataLight);
		GDREGISTER_CLASS(Prop2DDataProp2D);
		GDREGISTER_CLASS(Prop2DDataTiledWall2D);
		GDREGISTER_CLASS(Prop2DDataSprite);

#if VERSION_MINOR >= 4
		GDREGISTER_CLASS(Prop2DDataPortal);
#endif

		GDREGISTER_CLASS(GroundClutter2D);
		GDREGISTER_CLASS(GroundClutter2DFoliage);

		GDREGISTER_CLASS(Prop2DMesher);

		GDREGISTER_CLASS(Prop2DInstance);
		GDREGISTER_CLASS(Prop2DInstanceMerger);

		GDREGISTER_CLASS(Prop2DESSEntity);

		GDREGISTER_CLASS(Prop2DInstanceJob);
		GDREGISTER_CLASS(Prop2DInstanceProp2DJob);

		GDREGISTER_CLASS(Prop2DTextureJob);

		GDREGISTER_CLASS(Prop2DSceneInstance);

		GDREGISTER_CLASS(Prop2DMaterialCache);

#ifdef TEXTURE_PACKER_PRESENT
		GDREGISTER_CLASS(Prop2DMaterialCachePCM);
#endif

		prop_utils = memnew(Prop2DUtils);
		GDREGISTER_CLASS(Prop2DUtils);
		Engine::get_singleton()->add_singleton(Engine::Singleton("Prop2DUtils", Prop2DUtils::get_singleton()));

		prop_texture_cache = memnew(Prop2DCache);
		GDREGISTER_CLASS(Prop2DCache);
		Engine::get_singleton()->add_singleton(Engine::Singleton("Prop2DCache", Prop2DCache::get_singleton()));

		Ref<Prop2DDataLight> light_processor = Ref<Prop2DDataLight>(memnew(Prop2DDataLight));
		Prop2DUtils::add_processor(light_processor);

		Ref<Prop2DDataProp2D> prop_processor = Ref<Prop2DDataProp2D>(memnew(Prop2DDataProp2D));
		Prop2DUtils::add_processor(prop_processor);

		Ref<Prop2DDataScene> scene_processor = Ref<Prop2DDataScene>(memnew(Prop2DDataScene));
		Prop2DUtils::add_processor(scene_processor);

#if VERSION_MINOR >= 4
		Ref<Prop2DDataPortal> portal_processor = Ref<Prop2DDataPortal>(memnew(Prop2DDataPortal));
		Prop2DUtils::add_processor(portal_processor);
#endif

		Ref<Prop2DDataTiledWall2D> tiled_wall_processor = Ref<Prop2DDataTiledWall2D>(memnew(Prop2DDataTiledWall2D));
		Prop2DUtils::add_processor(tiled_wall_processor);

		Ref<Prop2DDataSprite> sprite_processor = Ref<Prop2DDataSprite>(memnew(Prop2DDataSprite));
		Prop2DUtils::add_processor(sprite_processor);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::add_by_type<Prop2DEditorPlugin>();
	}
#endif
}

void uninitialize_props_2d_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		if (prop_utils) {
			memdelete(prop_utils);
		}

		if (prop_texture_cache) {
			memdelete(prop_texture_cache);
		}
	}
}
