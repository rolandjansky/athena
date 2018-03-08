# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg HJJ setup starting from ATLAS defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_HJJ_Common.py')
PowhegConfig.ncall1   = 100000
PowhegConfig.ncall2   = 100000
PowhegConfig.ncall1rm = 100000
PowhegConfig.ncall2rm = 100000
PowhegConfig.foldx    = 1
PowhegConfig.foldy    = 1
PowhegConfig.foldphi  = 1
PowhegConfig.nubound  = 1000
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with main31 and CTEQ6L1
#--------------------------------------------------------------
# include('MC15JobOptions/Pythia8_AU2_CTEQ6L1_Common.py')
include('MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include('MC15JobOptions/Pythia8_Powheg.py')
#topAlg.Pythia8.UserModes += [ 'Main31:NFinal = 3' ]

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 H+dijet production with AU2 CT10 tune'
evgenConfig.keywords    = [ 'Higgs', 'dijet' ]
evgenConfig.contact     = [ 'james.robinson@cern.ch' ]
evgenConfig.generators += [ 'Powheg', 'Pythia8' ]
