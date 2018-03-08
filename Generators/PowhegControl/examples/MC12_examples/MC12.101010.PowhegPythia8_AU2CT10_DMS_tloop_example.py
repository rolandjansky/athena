# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg DMV setup starting from ATLAS defaults
#--------------------------------------------------------------
include("PowhegControl/PowhegControl_DMS_tloop_Common.py")
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with AU2 CT10
#--------------------------------------------------------------
include("MC12JobOptions/PowhegPythia8_AU2_CT10_Common.py")
include("MC12JobOptions/Pythia8_Powheg.py")

# id:all = name antiName spinType chargeType colType m0 mWidth mMin mMax tau0
genSeq.Pythia8.Commands += [ "1000022:all = X Xbar 2 0 0 100 0.0 0.0 0.0 0.0",
                             "1000022:isVisible = false" ]

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+Pythia8 DMS t-loop production with AU2 CT10 tune"
evgenConfig.keywords    = [ "BSM", "WIMP", "invisible" ]
evgenConfig.contact     = [ "james.robinson@cern.ch", "caterina.doglioni@cern.ch" ]
evgenConfig.generators += [ "Powheg", "Pythia8" ]
