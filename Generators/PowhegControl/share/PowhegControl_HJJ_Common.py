from PowhegControl import PowhegConfig_HJJ

# Use the PowhegConfig_HJJ configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_HJJ( runArgs=transform_runArgs, opts=transform_opts )
# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_HJJ(runArgs)
# else :
#   PowhegConfig = PowhegConfig_HJJ()
