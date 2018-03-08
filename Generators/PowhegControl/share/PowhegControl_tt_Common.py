from PowhegControl.PowhegUtils import PowhegConfig_tt

# Use the Powheg_hvq configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_tt(runArgs)
else :
  PowhegConfig = PowhegConfig_tt()
