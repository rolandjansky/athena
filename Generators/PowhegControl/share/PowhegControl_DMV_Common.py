from PowhegControl import PowhegConfig_DMV

# Use the Powheg_bb configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_DMV( runArgs=transform_runArgs, opts=transform_opts )
