# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import os
include("MuonEfficiencyCorrections/CommonToolSetup.py")


#------------------------------------------------------------------------------
def getMuonTriggerSFTester(input_name, triggers=[], MuonWP="Medium", Binning = "fine"):
    from AthenaCommon.AlgSequence import AlgSequence
    algSeq = AlgSequence()
    algName ="MuTriggerSFTester_%s_%s"%(MuonWP,Binning)
    if not hasattr(algSeq, algName):
        alg = CfgMgr.Trig__TestTrigSF(algName)
        alg.inputContainerName = input_name
        alg.prwTool = GetPRWTool() 
        alg.TriggerEfficiencyTool = GetTriggerSFTool(MuonWP,Binning)
        alg.triggers = triggers
        alg.outputStream   = "%s_%s"%(MuonWP, Binning)
        algSeq +=  alg
    return getattr(algSeq,algName)


#######################################################################
#		Setup the Tools
#######################################################################
#
#athena -c "inputFile='/ptmp/mpp/zenon/DirectStau/mc16_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.deriv.DAOD_SUSY3.e3601_s3126_r9364_r9315_p3179/DAOD_SUSY3.11544886._000178.pool.root.1'" MuonEfficiencyCorrections/TriggerSF_Tester_jobOptions.py
#

#trigs = [
        #"HLT_mu20_iloose_L1MU15_OR_HLT_mu40",
        #"HLT_2mu10",
        #"HLT_mu18_mu8noL1",
        #"HLT_mu24_imedium_OR_HLT_mu40",
        #"HLT_mu24_imedium_OR_HLT_mu50",
        #"HLT_mu24_ivarmedium_OR_HLT_mu40", 
        #"HLT_mu24_ivarmedium_OR_HLT_mu50",
        #"HLT_mu26_ivarmedium_OR_HLT_mu50",
        #"HLT_2mu14",
        #"HLT_mu20_mu8noL1",
        #"HLT_mu22_mu8noL1",
        #]

trigs = [
            #'HLT_mu10',
            #'HLT_mu10_msonly',
            #'HLT_mu14',
            #'HLT_mu18',
            #'HLT_mu20',
            #'HLT_mu20_iloose_L1MU15',
            #'HLT_mu20_L1MU15',
            #'HLT_mu20_msonly',
            #'HLT_mu22',
            #'HLT_mu24',
            #'HLT_mu24_iloose_L1MU15',
            #'HLT_mu24_imedium',
            #'HLT_mu24_L1MU15',
            #'HLT_mu26',
            #'HLT_mu26_imedium',
            #'HLT_mu26_ivarmedium',
            #'HLT_mu24_ivarmedium',
            #'HLT_mu40',
            #'HLT_mu4',
            #'HLT_mu4_msonly',
            'HLT_mu50',
            #'HLT_mu6',
            #'HLT_mu6_msonly',
            ]


AssembleIO()

getMuonTriggerSFTester("Muons" , trigs)


