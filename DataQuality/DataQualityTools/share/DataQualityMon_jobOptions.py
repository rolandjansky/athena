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
    JetCollectionKey='antikt4HIItrEM_TowerJets'
else:
    JetCollectionKey='AntiKt4EMTopoJets'
                        
# Import external configuration
#if not 'DQMonFlags' in dir():
#   print "DataQualityMon_jobOptions.py: DQMonFlags not yet imported - I import them now"
#   from AthenaMonitoring.DQMonFlags import DQMonFlags
   
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
    
#if (jobproperties.Beam.beamType()=="collisions"):
#    IDTrkContNames = [ InDetKeys.UnslimmedTracks() ]
#
#else:
#    IDTrkContNames = [ InDetKeys.TRTTracks() ]

IDTrkContNames = [ InDetKeys.Tracks() ]

isCosmics=False
isBeam=True

if jobproperties.Beam.beamType()=='cosmics':
    isCosmics=True
    isBeam=False

if DQMonFlags.monManEnvironment != 'tier0ESD':
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    DefaultExtrapolator = AtlasExtrapolator()
    ToolSvc += DefaultExtrapolator

    # Import Muon Trk Eff tool ---> Obsolete
    #from DataQualityTools.DataQualityToolsConf import DQTMuTrkEff
    #DQTMuTrkMon = DQTMuTrkEff(name                    = 'DQTMuTrkMon',
    #                    doInDet                 = rec.doInDet(),
    #                    doMuons                 = rec.doMuon(),
    #                    doTile                  = rec.doTile(),
    #                    doCalo                  = muonCombinedRecFlags.doCaloTrkMuId(),
    #                    applyCaloCuts           = True,
    #                    caloCutOpen             = 0.0,
    #                    caloCutLow              = 25.0,
    #                    caloCutNormal           = 50.0,
    #                    trackName               = IDTrkContNames,
    #                    trackTag                = ["indet"            ],
    #                    trackPostfix            = ["" ], #eventually it can be "Up" "Low"
    #                    mooreMuContainerNameVec = ["MooreTracks", "ConvertedMBoyTracks"],
    #                    caloMuContainerNameVec  = ["CaloESDMuonCollection"],
    #                    tileMuContainerName     = "TileCosmicMuonHT",
    #                    setVertexLimit          = True,
    #                    maxD0                   = 100.0,
    #                    maxZ0                   = 200.0,
    #                    histoPathBase           = "/GLOBAL/DQTEff",
    #                    maxConeSize             = 0.01,
    #                    doRunCosmics            = isCosmics,
    #                    doRunBeam               = isBeam,
    #                    doOfflineHists          = isOffline,
    #                    doOnlineHists           = isOnline                    
    #                    );
    #
    #
    #DQTMuTrkMon.extrapolator = DefaultExtrapolator
    #
    #ToolSvc += DQTMuTrkMon;
    #ManagedAthenaGlobalMon.AthenaMonTools += [ DQTMuTrkMon ];

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

        ToolSvc += DQTDetSynchMon;
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
        
        ToolSvc += DQTCaloClusterTool;
        ManagedAthenaGlobalMon.AthenaMonTools += [ DQTCaloClusterTool ];


    # Import GlobalWFinder tool ---> Obsolete
    #from DataQualityTools.DataQualityToolsConf import DQTGlobalWFinderTool
    #DQTGlobalWFinderTool = DQTGlobalWFinderTool(name            = 'DQTGlobalWFinderTool',
    #                                 histoPathBase   = "/GLOBAL/DQTGlobalWFinder",
    #                                 doOfflineHists          = isOffline,
    #                                 doOnlineHists           = isOnline,
    #                                 doRunCosmics            = isCosmics,
    #                                 doRunBeam               = isBeam 
    #                                );
    #
    #ToolSvc += DQTGlobalWFinderTool;
    #ManagedAthenaGlobalMon.AthenaMonTools += [ DQTGlobalWFinderTool ];

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
        
        ToolSvc += DQTBackgroundMon;
        ManagedAthenaGlobalMon.AthenaMonTools += [ DQTBackgroundMon ];
    

    # Default values
    MinSCTHits=5
    MinPtCut=4000

    #For now, to increase statistics in cosmics data taking
    if athenaCommonFlags.isOnline==True:
        MinSCTHits=0
        MinPtCut=500

    # Import MuonID tool
    #from DataQualityTools.DataQualityToolsConf import DQTMuonIDTrackTool
    #DQTMuonIDTrackMon = DQTMuonIDTrackTool(name            = 'DQTMuonIDTrackMon',
    #                                 histoPathBase   = "/GLOBAL/DQTMuonVsInDet",
    #                                 doOfflineHists          = isOffline,
    #                                 doOnlineHists           = isOnline,
    #                                 doRunCosmics            = isCosmics,
    #                                 doRunBeam               = isBeam,
    #                                 MinSCTHits              = MinSCTHits,
    #                                 MinPtCut                = MinPtCut
    #                                );

    #DQTMuonIDTrackMon.ExtrapolationTool = DefaultExtrapolator
    #ToolSvc += DQTMuonIDTrackMon;
    #ManagedAthenaGlobalMon.AthenaMonTools += [ DQTMuonIDTrackMon ];

    # Import ElectronQuality Tool ---> Obsolete
    #from DataQualityTools.DataQualityToolsConf import  DQTElectronQualityTool
    #DQTElectronQualityTool = DQTElectronQualityTool(name            = 'DQTElectronQualityTool',
    #                                          histoPathBase   = "/GLOBAL/DQTElectronQuality",
    #                                          doRunCosmics            = isCosmics,
    #                                          doRunBeam               = isBeam,
    #                                          doOfflineHists          = isOffline,
    #                                          doOnlineHists           = isOnline
    #                                          );
    #
    #ToolSvc += DQTElectronQualityTool;
    #ManagedAthenaGlobalMon.AthenaMonTools += [ DQTElectronQualityTool ];

    # Import Rate Mon Tool ---> Obsolete
    #from DataQualityTools.DataQualityToolsConf import DQTRateMonTool
    #DQTRateMonTool = DQTRateMonTool(name            = 'DQTRateMonTool',
    #                                 histoPathBase   = "/GLOBAL/DQTRateMon",
    #                                 doOfflineHists          = isOffline,
    #                                 doOnlineHists           = isOnline,
    #                                 doRunCosmics            = isCosmics,
    #                                 doRunBeam               = isBeam,
    #                                 doTrigger               = rec.doTrigger(),
    #                                 JetCollectionName       = JetCollectionKey,
    #                                 triggerList             = listOfTriggers
    #                                );
    #ToolSvc += DQTRateMonTool;
    #ManagedAthenaGlobalMon.AthenaMonTools += [ DQTRateMonTool ];


    #This does not seem needed anymore
    #include( "MuonCommRecExample/ReadMuCTPI_jobOptions.py" )

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
ToolSvc += DQTDataFlowMon
ManagedAthenaGlobalMon.AthenaMonTools += [ DQTDataFlowMon ]
#print ManagedAthenaGlobalMon;

#if isBeam==True and (DQMonFlags.monManEnvironment == 'tier0ESD' or DQMonFlags.monManEnvironment == 'online' or DQMonFlags.monManEnvironment == 'tier0' or DQMonFlags.monManEnvironment == 'tier0Raw' ) and rec.doInDet():
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
    from TrigMuonMatching.TrigMuonMatchingConf import Trig__TrigMuonMatching
    ToolSvc += Trig__TrigMuonMatching("MatchingTool");

    from DataQualityTools.DataQualityToolsConf import DQTGlobalWZFinderTool
    MyDQTGlobalWZFinderTool = DQTGlobalWZFinderTool(
        name  = 'DQTGlobalWZFinderTool',
        doTrigger = rec.doTrigger(),
        JetCollectionName = JetCollectionKey,
        MuonPtCut = 27,
        MuonMaxEta = ToolSvc.DQTMuonSelectionTool.MaxEta,
        MuonSelectionTool = ToolSvc.DQTMuonSelectionTool,
        IsolationSelectionTool = ToolSvc.DQTIsoGradientTool,
        MuonTrigMatchingTool = ToolSvc.MatchingTool,
    )
    ToolSvc += MyDQTGlobalWZFinderTool;
    ManagedAthenaGlobalPhysMon.AthenaMonTools += [ MyDQTGlobalWZFinderTool ];

    #from TrigBunchCrossingTool.TrigBunchCrossingToolConf import Trig__TrigConfBunchCrossingTool
    #ToolSvc += Trig__TrigConfBunchCrossingTool("DQTBunchCrossingTool")
    from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
    #ToolSvc += BunchCrossingTool("DQTBunchCrossingTool")
    DontUseBunchCrossingTool = False
    if (rec.triggerStream() == 'CosmicCalo'
        or globalflags.DataSource.get_Value() == 'geant4'
        or 'collisions' not in DQMonFlags.monManDataType.get_Value()):
        DontUseBunchCrossingTool = True


    from DataQualityTools.DataQualityToolsConf import DQTLumiMonTool
    DQTLumiMonToolAnyTrigger = DQTLumiMonTool(
        name = 'DQTLumiMonToolAnyTrigger',
        histoPath  = '/GLOBAL/Luminosity/AnyTrigger',
        bunchCrossingTool=BunchCrossingTool(),
        TurnOffBunchTool = DontUseBunchCrossingTool
    )
    DQTLumiMonToolMu = DQTLumiMonTool(
        name = 'DQTLumiMonToolMu',
        histoPath = '/GLOBAL/Luminosity/EF_muX',
        TriggerChain = 'CATEGORY_monitoring_muonIso',
        bunchCrossingTool=BunchCrossingTool(),
        TrigDecisionTool = monTrigDecTool if DQMonFlags.useTrigger() else "",
        TurnOffBunchTool= DontUseBunchCrossingTool
    )
    DQTLumiMonToolEl = DQTLumiMonTool(
        name = 'DQTLumiMonToolEl',
        histoPath = '/GLOBAL/Luminosity/EF_eX',
        TriggerChain = 'CATEGORY_primary_single_ele',
        bunchCrossingTool=BunchCrossingTool(),
        TrigDecisionTool = monTrigDecTool if DQMonFlags.useTrigger() else "",
        TurnOffBunchTool= DontUseBunchCrossingTool
    )
    ToolSvc += [DQTLumiMonToolAnyTrigger, DQTLumiMonToolMu, DQTLumiMonToolEl]
    ManagedAthenaGlobalPhysMon.AthenaMonTools += [ DQTLumiMonToolAnyTrigger, DQTLumiMonToolMu, DQTLumiMonToolEl]

