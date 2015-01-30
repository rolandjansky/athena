from PowhegControl import PowhegConfig_VBF_H

# Use the Powheg_VBF_H configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_VBF_H(runArgs)
else :
  PowhegConfig = PowhegConfig_VBF_H()
