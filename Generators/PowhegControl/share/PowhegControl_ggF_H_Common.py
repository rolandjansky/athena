from PowhegControl import PowhegConfig_ggF_H

# Use the Powheg_ggF_H configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_ggF_H( runArgs=transform_runArgs, opts=transform_opts )
