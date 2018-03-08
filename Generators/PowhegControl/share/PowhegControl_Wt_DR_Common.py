from PowhegControl import PowhegConfig_Wt_DR

# Use the PowhegConfig_Wt_DR configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_Wt_DR(runArgs)
else :
  PowhegConfig = PowhegConfig_Wt_DR()
