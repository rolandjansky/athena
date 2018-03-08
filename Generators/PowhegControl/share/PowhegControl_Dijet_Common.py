from PowhegControl.PowhegUtils import PowhegConfig_Dijet

# Use the Powheg_Dijet configuration
if 'runArgs' in dir() : 
  PowhegConfig = PowhegConfig_Dijet(runArgs)
else :
  PowhegConfig = PowhegConfig_Dijet()
