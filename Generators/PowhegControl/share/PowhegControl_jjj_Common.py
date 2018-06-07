from PowhegControl import PowhegConfig_jjj

# Use the Powheg_jjj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_jjj( runArgs=transform_runArgs, opts=transform_opts )
