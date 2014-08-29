## Stripped options for memory benchmarking:

include("G4AtlasTests/jobOptions.Tile.py")

## Remove G4 algorithms from the job list
def rm_g4_algs():
    from AthenaCommon.AppMgr import theApp
    ta = theApp.TopAlg
    ta.remove('PyAthena::Alg/PyG4AtlasAlg')
    ta.remove('G4AtlasAlg/G4AtlasAlg')
    theApp.TopAlg = ta
simFlags.InitFunctions.add_function("preInit", rm_g4_algs)

## Disable the SD initialisation
AtlasG4Eng.G4Eng._InitList.append('init_SenDetector')
