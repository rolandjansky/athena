from PowhegControl import PowhegConfig_Wt_DR

# Use the PowhegConfig_Wt_DR configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_Wt_DR( runArgs=transform_runArgs, opts=transform_opts )

# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_Wt_DR(runArgs)
# else :
#   PowhegConfig = PowhegConfig_Wt_DR()
