from PowhegControl import PowhegConfig_Zj

# Use the Powheg_Zj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Zj( runArgs=transform_runArgs, opts=transform_opts )
