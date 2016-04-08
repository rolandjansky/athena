from VBFNLOControl.VbfnloUtils import VbfnloConfig

# Use the Vbfnlo_SMHiggs configuration                                                                                                             
if 'runArgs' in dir() :
  VbfnloConfig = VbfnloConfig(runArgs)
else :
  VbfnloConfig = VbfnloConfig()
