from PowhegControl import PowhegConfig_HJ

# Use the PowhegConfig_HJ configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_HJ( runArgs=transform_runArgs, opts=transform_opts )
# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_HJ(runArgs)
# else :
#   PowhegConfig = PowhegConfig_HJ()
