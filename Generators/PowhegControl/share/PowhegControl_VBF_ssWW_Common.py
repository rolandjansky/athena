from PowhegControl import PowhegConfig_VBF_ssWW

# Use the Powheg_VBF_ssWW configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_VBF_ssWW( runArgs=transform_runArgs, opts=transform_opts )
