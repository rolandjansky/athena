from PowhegControl.PowhegUtils import PowhegConfig_hvq

# Use the Powheg_hvq configuration
if 'runArgs' in dir() : 
  PowhegConfig = PowhegConfig_hvq(runArgs)
else :
  PowhegConfig = PowhegConfig_hvq()
