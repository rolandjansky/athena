from PowhegControl import PowhegConfig_t_tch_4FS

# Use the PowhegConfig_t_tch_4FS configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_t_tch_4FS( runArgs=transform_runArgs, opts=transform_opts )
