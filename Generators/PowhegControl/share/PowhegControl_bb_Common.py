from PowhegControl import PowhegConfig_bb

# Use the Powheg_bb configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_bb( runArgs=transform_runArgs, opts=transform_opts )

# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_bb(runArgs)
# else :
#   PowhegConfig = PowhegConfig_bb()
