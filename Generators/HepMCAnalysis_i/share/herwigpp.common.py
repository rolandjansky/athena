cmds.append("cd /Herwig/Generators")
cmds.append("set LHCGenerator:EventHandler:LuminosityFunction:Energy 14000.0")

#Intrinsic pT tune extrapolated to LHC energy
cmds.append("set /Herwig/Shower/Evolver:IntrinsicPtGaussian 2.2*GeV")
cmds.append("cd /Herwig/Partons")

#PDF set
cmds.append("create ThePEG::LHAPDF myPDFset")
cmds.append("set myPDFset:PDFName cteq6ll.LHpdf")
cmds.append("set myPDFset:RemnantHandler HadronRemnants")
cmds.append("set /Herwig/Particles/p+:PDF myPDFset")
cmds.append("set /Herwig/Particles/pbar-:PDF myPDFset")

#Enable QED YFS radiation
cmds.append("cd /Herwig/EventHandlers")
cmds.append("insert LHCHandler:PostSubProcessHandlers[0] /Herwig/QEDRadiation/QEDRadiationHandler")

#some default parameters
cmds.append("cd /Herwig/Generators")
cmds.append("set LHCGenerator:DebugLevel 1")
cmds.append("set LHCGenerator:PrintEvent 0")
cmds.append("set LHCGenerator:MaxErrors 10000")
cmds.append("set LHCGenerator:NumberOfEvents 1000000000")

