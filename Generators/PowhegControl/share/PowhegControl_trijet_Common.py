from PowhegControl import PowhegConfig_trijet

# Use the Powheg_trijet configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_trijet( runArgs=transform_runArgs, opts=transform_opts )

# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_trijet(runArgs)
# else :
#   PowhegConfig = PowhegConfig_trijet()
