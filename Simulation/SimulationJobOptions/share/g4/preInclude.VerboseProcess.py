# Use verbose output for all G4 process.
# Only works in dbg builds.

def use_verbose_process():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.gbl.G4Commands().process.verbose(10)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitPhysics", use_verbose_process)
