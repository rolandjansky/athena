from PowhegControl import PowhegConfig_jj

# Use the Powheg_jj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_jj( runArgs=transform_runArgs, opts=transform_opts )
