from PowhegControl import PowhegConfig_ssWWjj

# Use the Powheg_ssWWjj configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_ssWWjj( runArgs=transform_runArgs, opts=transform_opts )
