from PowhegControl.PowhegUtils import PowhegConfig_hvq

# Use the Powheg_bb configuration
if 'runArgs' in dir() : 
  PowhegConfig = PowhegConfig_hvq(runArgs)
else :
  PowhegConfig = PowhegConfig_hvq()
