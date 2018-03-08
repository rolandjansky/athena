from PowhegControl import PowhegConfig_Wjj

# Use the Powheg_Wjj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Wjj( runArgs=transform_runArgs, opts=transform_opts )
