from PowhegControl import PowhegConfig_ggHZ

# Use the PowhegConfig_ggHZ configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_ggHZ(runArgs)
else :
  PowhegConfig = PowhegConfig_ggHZ()
