from PowhegControl import PowhegConfig_WZ

# Use the Powheg_WZ configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_WZ( runArgs=transform_runArgs, opts=transform_opts )
