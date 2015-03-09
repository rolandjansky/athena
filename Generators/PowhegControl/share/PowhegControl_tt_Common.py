from PowhegControl import PowhegConfig_tt

# Use the Powheg_tt configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_tt( runArgs=transform_runArgs, opts=transform_opts )
# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_tt(runArgs)
# else :
#   PowhegConfig = PowhegConfig_tt()
