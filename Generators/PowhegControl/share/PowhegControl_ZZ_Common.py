from PowhegControl import PowhegConfig_ZZ

# Use the Powheg_ZZ configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_ZZ( runArgs=transform_runArgs, opts=transform_opts )

# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_ZZ(runArgs)
# else :
#   PowhegConfig = PowhegConfig_ZZ()
