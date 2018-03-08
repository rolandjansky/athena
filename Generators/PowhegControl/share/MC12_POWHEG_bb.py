from PowhegControl.PowhegUtils import PowhegConfig_bb

# Use the Powheg_bb configuration
PowhegConfig = PowhegConfig_bb()
if 'runArgs' in dir() : 
  PowhegConfig = PowhegConfig_bb(runArgs)
else :
  PowhegConfig = PowhegConfig_bb()
