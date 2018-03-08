from PowhegControl import PowhegConfig_t_tch_4f

# Use the PowhegConfig_t_sch configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_t_tch_4f( runArgs=transform_runArgs, opts=transform_opts )
