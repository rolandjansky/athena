from PowhegControl import PowhegConfig_HWj

# Use the PowhegConfig_HWj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_HWj( runArgs=transform_runArgs, opts=transform_opts )
