from PowhegControl import PowhegConfig_W_EW

# Use the Powheg_W_EW configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_W_EW( runArgs=transform_runArgs, opts=transform_opts )
