from PowhegControl import PowhegConfig_WW

# Use the Powheg_WW configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_WW( runArgs=transform_runArgs, opts=transform_opts )

