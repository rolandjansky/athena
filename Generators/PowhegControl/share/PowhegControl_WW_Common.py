from PowhegControl import PowhegConfig_WW

# Use the Powheg_WW configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_WW(runArgs)
else :
  PowhegConfig = PowhegConfig_WW()

