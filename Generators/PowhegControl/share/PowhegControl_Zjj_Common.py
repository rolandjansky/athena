from PowhegControl import PowhegConfig_Zjj

# Use the Powheg_Zjj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Zjj( runArgs=transform_runArgs, opts=transform_opts )
