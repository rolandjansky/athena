from PowhegControl import PowhegConfig_tt

# Use the Powheg_tt configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_tt(runArgs)
else :
  PowhegConfig = PowhegConfig_tt()
