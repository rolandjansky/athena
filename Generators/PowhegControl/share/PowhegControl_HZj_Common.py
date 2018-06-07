from PowhegControl import PowhegConfig_HZj

# Use the PowhegConfig_HZj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_HZj( runArgs=transform_runArgs, opts=transform_opts )
