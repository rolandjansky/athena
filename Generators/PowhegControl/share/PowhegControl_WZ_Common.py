from PowhegControl import PowhegConfig_WZ

# Use the Powheg_WZ configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_WZ(runArgs)
else :
  PowhegConfig = PowhegConfig_WZ()
