from PowhegControl import PowhegConfig_W

# Use the Powheg_W configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_W(runArgs)
else :
  PowhegConfig = PowhegConfig_W()
