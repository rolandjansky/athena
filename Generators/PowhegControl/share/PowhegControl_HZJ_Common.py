from PowhegControl import PowhegConfig_HZJ

# Use the PowhegConfig_HZJ configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_HZJ(runArgs)
else :
  PowhegConfig = PowhegConfig_HZJ()
