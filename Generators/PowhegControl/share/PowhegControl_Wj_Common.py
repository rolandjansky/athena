from PowhegControl import PowhegConfig_Wj

# Use the Powheg_Wj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Wj( runArgs=transform_runArgs, opts=transform_opts )
