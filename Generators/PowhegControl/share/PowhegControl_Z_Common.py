from PowhegControl import PowhegConfig_Z

# Use the Powheg_Z configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_Z(runArgs)
else :
  PowhegConfig = PowhegConfig_Z()
