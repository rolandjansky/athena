# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg DMS_tloop setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_DMS_tloop_Common.py")
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include("MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("MC15JobOptions/Pythia8_Powheg.py")

# id:all = name antiName spinType chargeType colType m0 mWidth mMin mMax tau0
genSeq.Pythia8.Commands += [ "1000022:all = X Xbar 2 0 0 {0} 0.0 0.0 0.0 0.0".format(PowhegConfig.DM_mass),
                             "1000022:isVisible = false" ]

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 DMS t-loop production with A14 NNPDF2.3 tune"
evgenConfig.keywords    = [ "BSM", "WIMP", "invisible" ]
evgenConfig.contact     = [ "james.robinson@cern.ch", "caterina.doglioni@cern.ch" ]
