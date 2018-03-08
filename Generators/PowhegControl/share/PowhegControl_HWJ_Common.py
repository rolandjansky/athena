from PowhegControl.PowhegUtils import PowhegConfig_HWJ

# Use the PowhegConfig_HWJ configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_HWJ(runArgs)
else :
  PowhegConfig = PowhegConfig_HWJ()
