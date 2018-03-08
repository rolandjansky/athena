from PowhegControl import PowhegConfig_HZJ

# Use the PowhegConfig_HZJ configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_HZJ( runArgs=transform_runArgs, opts=transform_opts )
# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_HZJ(runArgs)
# else :
#   PowhegConfig = PowhegConfig_HZJ()
