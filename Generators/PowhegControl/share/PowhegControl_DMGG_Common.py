from PowhegControl import PowhegConfig_DMGG

# Use the Powheg_DMGG configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_DMGG( runArgs=transform_runArgs, opts=transform_opts )
