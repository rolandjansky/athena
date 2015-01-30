from PowhegControl import PowhegConfig_bb

# Use the Powheg_bb configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_bb(runArgs)
else :
  PowhegConfig = PowhegConfig_bb()
