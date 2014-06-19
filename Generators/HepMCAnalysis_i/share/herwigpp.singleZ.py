#Set up for Drell-Yan Z  process
cmds.append("cd /Herwig/MatrixElements")
cmds.append("insert SimpleQCD:MatrixElements[0] MEqq2gZ2ff")

#Only include the Z (no Gamma) in the process
cmds.append("set SimpleQCD:MatrixElements[0]:GammaZ 2")
