#**************************************************************
# jopOptions file for Combined Monitoring in Athena
#**************************************************************

#disable stuff calling topoclustering until further notice - PUEO 4/1/14
CALOCLUSTER=False

#Make m_trigDecTool available:
TrigDecisionTool= monTrigDecTool if DQMonFlags.useTrigger() else "",

#Doesn't work anyway
#DQMonFlags.doMuonCombinedMon.set_Value_and_Lock(False)
#DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)


#Global monitoring checks to make sure all triggers are firing. The following triggers are monitored. Triggers are listed here:https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ExpressStream#Physics_pp_v2_menu_collisions

listOfTriggers = ['EF_g20_loose', 'EF_tauNoCut', 'EF_mu15', 'EF_2mu4_Upsimumu', 'EF_2mu10_loose', 'EF_tauNoCut_L1TAU50', 'EF_2e12_medium', 'EF_mu4_L1J10_matched', 'EF_tau16_IDTrkNoCut', 'EF_2mu4_Jpsimumu_IDTrkNoCut', 'EF_e20_medium_IDTrkNoCut', 'EF_L1J10_firstempty_NoAlg', 'EF_L1J30_firstempty_NoAlg', 'EF_rd0_filled_NoAlg', 'EF_rd0_empty_NoAlg', 'EF_tauNoCut_cosmic', 'EF_j240_a4tc_EFFS', 'EF_fj30_a4tc_EFFS', 'EF_tau50_IDTrkNoCut', 'EF_xe20_noMu', 'EF_mbMbts_1_eff', 'EF_2e5_tight', 'EF_2mu10', 'EF_b10_IDTrkNoCut', 'EF_mu15_mu10_EFFS', 'EF_j30_a4tc_EFFS', 'EF_mu20_IDTrkNoCut', 'EF_InDetMon_FS', 'EF_2mu13_Zmumu_IDTrkNoCut', 'EF_mu20_muCombTag_NoEF']


# Import AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if rec.doCalo and CALOCLUSTER:
    include ('DataQualityTools/HadTopoClusterMaker.py')

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
isOnline=False
isOffline=True
if athenaCommonFlags.isOnline==True:
    isOnline=True
    isOffline=False


from RecExConfig.RecFlags import rec
if rec.doHeavyIon():
    JetCollectionKey='AntiKt4HIJets'
else:
    JetCollectionKey='AntiKt4EMTopoJets'
                           
# Import Algorithm
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "GlobalMonManager" )
ManagedAthenaGlobalMon = topSequence.GlobalMonManager
ManagedAthenaGlobalMon.FileKey             = DQMonFlags.monManFileKey()
ManagedAthenaGlobalMon.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
ManagedAthenaGlobalMon.DataType            = DQMonFlags.monManDataType()
ManagedAthenaGlobalMon.Environment         = DQMonFlags.monManEnvironment()

from AthenaCommon.JobProperties import jobproperties
if not 'InDetKeys' in dir():
    from InDetRecExample.InDetKeys import InDetKeys
    
IDTrkContNames = [ InDetKeys.Tracks() ]

isCosmics=False
isBeam=True

if jobproperties.Beam.beamType()=='cosmics':
    isCosmics=True
    isBeam=False

if DQMonFlags.monManEnvironment != 'tier0ESD':
     # Import Det Synch tool
    if DQMonFlags.monManEnvironment in ('tier0Raw', 'tier0') and globalflags.DataSource.get_Value() != 'geant4':
        from DataQualityTools.DataQualityToolsConf import  DQTDetSynchMonTool
        DQTDetSynchMon = DQTDetSynchMonTool(name            = 'DQTDetSynchMon',
                                            histoPathBase   = "/GLOBAL/DQTSynch",
                                            doRunCosmics            = isCosmics,
                                            doRunBeam               = isBeam,
                                            doOfflineHists          = isOffline,
                                            doOnlineHists           = isOnline
                                            );

        ManagedAthenaGlobalMon.AthenaMonTools += [ DQTDetSynchMon ];

    if rec.doCalo and CALOCLUSTER:
        ## Import CaloCluster Tool
        from DataQualityTools.DataQualityToolsConf import  DQTCaloClusterTool
        DQTCaloClusterTool = DQTCaloClusterTool(name            = 'DQTCaloClusterTool',
                                          histoPathBase   = "/GLOBAL/DQTCaloCluster",
                                          doRunCosmics            = isCosmics,
                                          doRunBeam               = isBeam,
                                          doOfflineHists          = isOffline,
                                          doOnlineHists           = isOnline
                                          );
        
        ManagedAthenaGlobalMon.AthenaMonTools += [ DQTCaloClusterTool ];


    # Import BackgroundMon tool

    if DQMonFlags.useTrigger():

        from DataQualityTools.DataQualityToolsConf import DQTBackgroundMon
        DQTBackgroundMon = DQTBackgroundMon(name = 'DQTBackgroundMon',
                                            histoPathBase = "/GLOBAL/DQTBackgroundMon",
                                            doOfflineHists = isOffline,
                                            doOnlineHists = isOnline,
                                            doRunCosmics = isCosmics,
                                            doRunBeam = isBeam,
                                            doMuons = rec.doMuon(),
                                            doTrigger = rec.doTrigger()
                                            );
        
        ManagedAthenaGlobalMon.AthenaMonTools += [ DQTBackgroundMon ];
    

    # Default values
    MinSCTHits=5
    MinPtCut=4000

    #For now, to increase statistics in cosmics data taking
    if athenaCommonFlags.isOnline==True:
        MinSCTHits=0
        MinPtCut=500

    if not rec.doMuon:
        try:
            svcMgr.ByteStreamAddressProviderSvc.TypeNames.remove("RpcPadContainer/RPCPAD")
        except:
            print 'RPCPAD cannot be removed'

from DataQualityTools.DataQualityToolsConf import DQTDataFlowMonTool
import os
if 'AtlasProject' in os.environ and 'AtlasVersion' in os.environ:
    releaseString = '%s-%s' % (os.environ['AtlasProject'],
                               os.environ['AtlasVersion'])
else:
    releaseString = 'Unknown'
DQTDataFlowMon = DQTDataFlowMonTool(name = 'DQTDataFlowMon',
                                    histoPathBase = '/GLOBAL/DQTDataFlow',
                                    releaseString = releaseString
                                    );
ManagedAthenaGlobalMon.AthenaMonTools += [ DQTDataFlowMon ]

if isBeam==True and (DQMonFlags.monManEnvironment != 'tier0Raw') and rec.doInDet() and DQMonFlags.useTrigger():

    topSequence += AthenaMonManager( "GlobalMonPhysicsManager" )
    ManagedAthenaGlobalPhysMon = topSequence.GlobalMonPhysicsManager
    ManagedAthenaGlobalPhysMon.FileKey             = DQMonFlags.monManFileKey()
    ManagedAthenaGlobalPhysMon.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    ManagedAthenaGlobalPhysMon.DataType            = DQMonFlags.monManDataType()
    ManagedAthenaGlobalPhysMon.Environment         = DQMonFlags.monManEnvironment()

    from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
    ToolSvc += CP__MuonSelectionTool("DQTMuonSelectionTool",
                                     MaxEta=2.4,
                                     MuQuality=1)
    ToolSvc += CfgMgr.CP__IsolationSelectionTool("DQTIsoGradientTool",
                                                 MuonWP="LooseTrackOnly",
                                                 ElectronWP="LooseTrackOnly"
                                                 );

    from DataQualityTools.DataQualityToolsConf import DQTGlobalWZFinderTool
    MyDQTGlobalWZFinderTool = DQTGlobalWZFinderTool(
        name  = 'DQTGlobalWZFinderTool',
        doTrigger = rec.doTrigger(),
        MuonSelectionTool = ToolSvc.DQTMuonSelectionTool,
        IsolationSelectionTool = ToolSvc.DQTIsoGradientTool
    )
    ManagedAthenaGlobalPhysMon.AthenaMonTools += [ MyDQTGlobalWZFinderTool ];

    from DataQualityTools.DataQualityToolsConf import DQTLumiMonTool
    DQTLumiMonToolAnyTrigger = DQTLumiMonTool(
        name = 'DQTLumiMonToolAnyTrigger',
        histoPath  = '/GLOBAL/Luminosity/AnyTrigger',
    )
    DQTLumiMonToolMu = DQTLumiMonTool(
        name = 'DQTLumiMonToolMu',
        histoPath = '/GLOBAL/Luminosity/EF_muX',
        TriggerChain = 'CATEGORY_monitoring_muonIso',
        TrigDecisionTool = monTrigDecTool if DQMonFlags.useTrigger() else "",
    )
    DQTLumiMonToolEl = DQTLumiMonTool(
        name = 'DQTLumiMonToolEl',
        histoPath = '/GLOBAL/Luminosity/EF_eX',
        TriggerChain = 'CATEGORY_primary_single_ele',
        TrigDecisionTool = monTrigDecTool if DQMonFlags.useTrigger() else "",
    )
    ManagedAthenaGlobalPhysMon.AthenaMonTools += [ DQTLumiMonToolAnyTrigger, DQTLumiMonToolMu, DQTLumiMonToolEl]

