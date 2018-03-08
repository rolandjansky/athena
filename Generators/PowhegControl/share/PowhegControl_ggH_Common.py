from PowhegControl.PowhegUtils import PowhegConfig_ggH

# Use the Powheg_Z configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_ggH(runArgs)
else :
  PowhegConfig = PowhegConfig_ggH()
