#UE parameters equivalent to fHerwig
cmds.append("set /Herwig/Particles/K_S0:Stable Stable")
cmds.append("set /Herwig/Particles/Lambda0:Stable Stable")
cmds.append("cd /Herwig/Hadronization")
#Clustering parameters for b-quarks equivalent to fHerwig
cmds.append("set ClusterFissioner:ClMaxBottom  3.35*GeV")
cmds.append("set ClusterFissioner:ClPowBottom  1.2")

#MPI setup
cmds.append("cd /Herwig/UnderlyingEvent")
cmds.append("set MPIHandler:Cuts 4.91*GeV")
cmds.append("set MPIHandler:InvRadius 1.8*GeV2")

#ptmin = 10 GeV
cmds.append("set /Herwig/Cuts/JetKtCut:MinKT 10.0*GeV")

#Set up for pp -> Z + jets  process
cmds.append("cd /Herwig/MatrixElements")
cmds.append("insert SimpleQCD:MatrixElements[0] MEZJet")
#cmds.append("insert SimpleQCD:MatrixElements[0] MEqq2gZ2ff")
cmds.append("set SimpleQCD:MatrixElements[0]:GammaZ 2")
#cmds.append("set SimpleQCD:MatrixElements[0]:Process 5")
#cmds.append("set SimpleQCD:MatrixElements[0]:ZDecay 5")
#cmds.append("cd /Herwig/MatrixElements")
#cmds.append("insert SimpleQCD:MatrixElements[0] MEQCD2to2")
