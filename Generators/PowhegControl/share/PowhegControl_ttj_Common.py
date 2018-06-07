from PowhegControl import PowhegConfig_ttj

# Use the Powheg_ttj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_ttj( runArgs=transform_runArgs, opts=transform_opts )
