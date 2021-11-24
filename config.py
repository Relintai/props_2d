

def can_build(env, platform):
  return True


def configure(env):
	pass


def get_doc_classes():
    return [
        "PropDataEntry2D",
        "PropDataLight2D",
        "PropDataProp2D",
        "PropDataScene2D",
        "PropDataPortal2D",
        "PropDataTiledWall2D",
        "PropData2D",
        
        "TiledWall2D",
        "TiledWallData2D",

        "PropDataProcessor2D",

        "GroundClutterFoliage2D",
        "GroundClutter2D",

        "PropESSEntity2D",
        "PropInstance2D",
        "PropMeshUtils2D",

        "PropSceneInstance2D",

        "PropUtils2D",
    ]

def get_doc_path():
    return "doc_classes"

