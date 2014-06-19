#set a pt cut for the jet production
cmds.append("set /Herwig/Cuts/JetKtCut:MinKT 50.0*GeV")

#Set up for QCD 2 to 2 scattering processe
cmds.append("cd /Herwig/MatrixElements")
cmds.append("insert SimpleQCD:MatrixElements[0] MEQCD2to2")
