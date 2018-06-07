from PowhegControl import PowhegConfig_ggF_HZ

# Use the PowhegConfig_ggF_HZ configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_ggF_HZ( runArgs=transform_runArgs, opts=transform_opts )
