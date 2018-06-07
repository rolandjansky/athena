from PowhegControl import PowhegConfig_Hjj

# Use the PowhegConfig_Hjj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Hjj( runArgs=transform_runArgs, opts=transform_opts )
