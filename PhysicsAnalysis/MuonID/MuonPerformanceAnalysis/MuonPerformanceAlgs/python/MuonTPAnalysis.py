# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Massimiliano Bellomo
# June 2015
#
# Example job option fragment for running tag-and-probe tools in Athena from xAOD in Reco_tf
#

def MuonTPConfig(outputFilename, doPlots = False, doEff = False):
    # outputFilename: name of the output file
    # doPlots: steers creation of probe/match histograms
    # doEff: steers additional creation of efficiency histograms if doPlots is true

    from AthenaCommon.AppMgr import ServiceMgr
    from AthenaCommon.AlgSequence import AlgSequence
    from MuonPerformanceAlgs import ZmumuTPRecoAnalysis
    from MuonPerformanceAlgs import ZmumuTPMuonAnalysis
    from MuonPerformanceAlgs import JPsiTPRecoAnalysis
    from MuonPerformanceAlgs import JPsiTPMuonAnalysis
    from MuonPerformanceAlgs import CommonMuonTPConfig

    # Full job is a list of algorithms
    job = AlgSequence()

    ############## Zmumu T&P Configuration ########################
    do_Zmumu_RecoEff_TagProbe      = True # this is for Z->mumu, reco eff. You can add your own analysis in a similar way.
    do_Zmumu_IsolationEff_TagProbe = True # this is for Z->mumu, isolation eff. You can add your own analysis in a similar way.
    do_Zmumu_TriggerEff_TagProbe   = True # this is for Z->mumu, Trigger eff.

    ############## JPsi T&P Configuration ########################
    do_JPsi_RecoEff_TagProbe       = True
    do_JPsi_IsolationEff_TagProbe  = True
    do_JPsi_TriggerEff_TagProbe    = True

 
    GRL_to_use  = []
    #GRL_to_use  = ["/afs/cern.ch/user/a/atlasdqm/grlgen/All_Good/data15_13TeV.periodA_DetStatus-v62-pro18_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml"]

    CommonMuonTPConfig.AddTrigDecisionTool()
    CommonMuonTPConfig.AddTrigMatchingTool()
    CommonMuonTPConfig.AddMuonSelectionTool()
    CommonMuonTPConfig.AddGoodRunsListSelectionTool(GRL_to_use)
    
    ##### Reco eff analysis options
    if do_Zmumu_RecoEff_TagProbe:
        ZmumuTPRecoAnalysis.AddZmumuTPRecoAnalysis(doEtaSlices=False,
                                                   writeNtuple=True,
                                                   doClosure=False,
                                                   doDRSys=True,
                                                   doValid=False,
                                                   DoProbeMatchPlots = doPlots,
                                                   ProduceEfficiencies=doPlots and doEff)    
    if do_JPsi_RecoEff_TagProbe:
        JPsiTPRecoAnalysis.AddJPsiTPRecoAnalysis(writeNtuple=True,
                                                 doValid=False,
                                                 doDRSys=True,
                                                 DoProbeMatchPlots = doPlots,
                                                 ProduceEfficiencies=doPlots and doEff) 
        
    if do_Zmumu_TriggerEff_TagProbe or do_Zmumu_IsolationEff_TagProbe:
        ZmumuTPMuonAnalysis.AddZmumuTPMuonAnalysis( doIso = do_Zmumu_IsolationEff_TagProbe,
                                doTrig = do_Zmumu_TriggerEff_TagProbe,
                                doTrigEtaSlices    = True, 
                                doIsolEtaSlices    = False, 
                                doIsolPlots      = False,
                                doTriggerPlots      = True,
                                writeNtuple=True, doClosure=False, 
                                doDRSys=True, doDPhiTPSys=True, 
                                doProbeCharSys=True, 
                                doLooseProbes=True, doMediumProbes=True, 
                                doTightProbes=True, doTruthProbes=True,
                                doHighPtProbes=True,
                                doL1=True, doL2=False, doEF=False, 
                                doHLT=True, DoProbeMatchPlots=False, 
                                ProduceEfficiencies=doPlots and doEff)
        
    if do_JPsi_TriggerEff_TagProbe or do_JPsi_IsolationEff_TagProbe:
        JPsiTPMuonAnalysis.AddJPsiTPMuonAnalysis( doIso = do_JPsi_IsolationEff_TagProbe,
                                doTrig = do_JPsi_TriggerEff_TagProbe,
                                doTrigEtaSlices    = True, 
                                doIsolEtaSlices    = False, 
                                doIsolPlots      = False,
                                doTriggerPlots      = True,
                                writeNtuple=True, doClosure=False, 
                                doDRSys=True, doDPhiTPSys=True, 
                                doProbeCharSys=True, 
                                doLooseProbes=True, doMediumProbes=True, 
                                doTightProbes=True, doTruthProbes=True, 
                                doHighPtProbes=True,
                                doL1=True, doL2=False, doEF=False, 
                                doHLT=True, DoProbeMatchPlots=False, 
                                ProduceEfficiencies=doPlots and doEff)
                
    # Add HistSvc
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
    ServiceMgr.THistSvc.Output += ["MUONTP DATAFILE='{}' OPT='RECREATE'".format(outputFilename)]

