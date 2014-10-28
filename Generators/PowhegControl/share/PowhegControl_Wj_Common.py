from PowhegControl.PowhegUtils import PowhegConfig_Wj

# Use the Powheg_Wj configuration
if 'runArgs' in dir() : 
  PowhegConfig = PowhegConfig_Wj(runArgs)
else :
  PowhegConfig = PowhegConfig_Wj()
