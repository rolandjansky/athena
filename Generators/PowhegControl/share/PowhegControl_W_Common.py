from PowhegControl import PowhegConfig_W

# Use the Powheg_W configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_W( runArgs=transform_runArgs, opts=transform_opts )

# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_W(runArgs)
# else :
#   PowhegConfig = PowhegConfig_W()
