from PowhegControl.PowhegUtils import PowhegConfig_ZZ

# Use the Powheg_ZZ configuration
if 'runArgs' in dir() : 
  PowhegConfig = PowhegConfig_ZZ(runArgs)
else :
  PowhegConfig = PowhegConfig_ZZ()
