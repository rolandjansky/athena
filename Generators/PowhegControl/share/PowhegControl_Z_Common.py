from PowhegControl import PowhegConfig_Z

# Use the Powheg_Z configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Z( runArgs=transform_runArgs, opts=transform_opts )

# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_Z(runArgs)
# else :
#   PowhegConfig = PowhegConfig_Z()
