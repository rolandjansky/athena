from PowhegControl import PowhegConfig_ttH

# Use the Powheg_ttH configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_ttH( runArgs=transform_runArgs, opts=transform_opts )
