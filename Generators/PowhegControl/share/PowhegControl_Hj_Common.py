from PowhegControl import PowhegConfig_Hj

# Use the PowhegConfig_Hj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Hj( runArgs=transform_runArgs, opts=transform_opts )
