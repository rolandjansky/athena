from PowhegControl import PowhegConfig_Zj

# Use the Powheg_Zj configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_Zj(runArgs)
else :
  PowhegConfig = PowhegConfig_Zj()
