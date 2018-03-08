from PowhegControl import PowhegConfig_HWJ

# Use the PowhegConfig_HWJ configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_HWJ( runArgs=transform_runArgs, opts=transform_opts )

# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_HWJ(runArgs)
# else :
#   PowhegConfig = PowhegConfig_HWJ()
