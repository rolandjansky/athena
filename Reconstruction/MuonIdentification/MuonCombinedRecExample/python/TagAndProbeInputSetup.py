# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include


def setupTagAndProbeInput():
    print "Setting up setupTagAndProbeInput"

    #from MuonPerformanceAlgs.ZmumuTPAnalysis import *
    #from MuonPerformanceAlgs.ZmumuTPIsolationAnalysis import *
    #from MuonPerformanceAlgs.ZmumuTPTrigAnalysis import *
    include('MuonPerformanceAlgs/ZmumuTPAnalysis.py')
    include('MuonPerformanceAlgs/ZmumuTPIsolationAnalysis.py')
    include('MuonPerformanceAlgs/ZmumuTPTrigAnalysis.py')
 
     # now we can add the TP analysis/es itself/themselves :)
 
    ############## Zmumu T&P Configuration ########################
    do_Zmumu_RecoEff_TagProbe      = True # this is for Z->mumu, reco eff. You can add your own analysis in a similar way.
    do_Zmumu_IsolationEff_TagProbe = True # this is for Z->mumu, isolation eff. You can add your own analysis in a similar way.
    do_Zmumu_Trigger_TagProbe      = True  # this is for Z->mumu, Trigger eff.
    
    ##### General analysis options
    writeNtuple = True                     # Write an ntuple on top of the histos - for detailed studies, but increases output file size
    doEtaSlices = False
    doClosure   = False
    doDRSys     = False
    doValid     = False
    
    
    # Add utilities (tool, upstream algorithms) we need
    AddIsolationTools()
    AddTrigDecisionTool()
    AddTrigMatchingTool()
    AddMuonSelectionTool()
    
    
    ##### Reco eff analysis options
    if do_Zmumu_RecoEff_TagProbe:
        writeNtuple = True                   
        doEtaSlices = False
        doClosure   = False
        doDRSys     = True
        doValid     = True
        AddZmumuTPAnalysis(doEtaSlices,writeNtuple,doClosure,doDRSys,doValid)     
    ###############################################################
    
    
    ##### Isolation eff analysis options
    if do_Zmumu_IsolationEff_TagProbe:
        doEtaSlices = False
        doClosure   = False
        doDRSys     = True
        doValid     = True
        AddZmumuTPIsolationAnalysis(doEtaSlices,writeNtuple,doClosure,doDRSys)
        pass
    ###############################################################
    
    
    ##### Trigger analysis options
    if do_Zmumu_Trigger_TagProbe:
        doTriggerL1  = True
        doTriggerL2  = False
        doTriggerEF  = False
        doTriggerHLT = True
        doEtaSlices  = True
        doDRSys      = False
        AddTriggerTPAnalysis(doEtaSlices,writeNtuple,doClosure,doDRSys,doL1=doTriggerL1,doL2=doTriggerL2,doEF=doTriggerEF,doHLT=doTriggerHLT)
