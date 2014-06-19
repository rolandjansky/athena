#Set up for qqbar -> Z -> tau tau  process
cmds.append("cd /Herwig/MatrixElements")
cmds.append("insert SimpleQCD:MatrixElements[0] MEqq2gZ2ff")
cmds.append("set SimpleQCD:MatrixElements[0]:Process 7")
cmds.append("set SimpleQCD:MatrixElements[0]:GammaZ 2")
