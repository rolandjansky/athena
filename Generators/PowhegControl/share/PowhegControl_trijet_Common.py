from PowhegControl import PowhegConfig_trijet

# Use the Powheg_trijet configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_trijet(runArgs)
else :
  PowhegConfig = PowhegConfig_trijet()
