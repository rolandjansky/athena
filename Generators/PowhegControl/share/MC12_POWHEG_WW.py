from PowhegControl.PowhegUtils import PowhegConfig_WW

# Use the Powheg_bb configuration
if 'runArgs' in dir() : 
  PowhegConfig = PowhegConfig_WW(runArgs)
else :
  PowhegConfig = PowhegConfig_WW()
  