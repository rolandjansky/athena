from PowhegControl import PowhegConfig_ggHZ

# Use the PowhegConfig_ggHZ configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_ggHZ( runArgs=transform_runArgs, opts=transform_opts )
# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_ggHZ(runArgs)
# else :
#   PowhegConfig = PowhegConfig_ggHZ()
