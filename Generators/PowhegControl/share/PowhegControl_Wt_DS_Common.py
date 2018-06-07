from PowhegControl import PowhegConfig_Wt_DS

# Use the PowhegConfig_Wt_DS configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Wt_DS( runArgs=transform_runArgs, opts=transform_opts )
