#======================================================================
# TOP JobOptions to run on MuonTrackMonitoring
#======================================================================
from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags
#############TriggerAware Trk Monitoring#######################
if MuonDQAFlags.MuonTrkMonDoTrigger or MuonDQAFlags.doMDTTGC:
   from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMaker
   trigDecMaker = TrigDecisionMaker()
   topSequence += trigDecMaker
   #from TrigDecision.TrigDecisionConf import TrigDec__TrigDecisionTool
   #tdt = TrigDec__TrigDecisionTool()
   #ToolSvc += tdt  
   tdt = monTrigDecTool
###############################################################
#----------------------#
# Trk Level Monitoring #
#----------------------#
### Which Track Collections to read (set array element to 0 or 1):
# 1-> Moore, 2->MuonBoy
###############################################################    
#############MuonSelectedTracksMon#############
from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonSelectedTracksMon
#############MuonGenericTracksMon#############
from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonGenericTracksMon
#############MuonGenericTracksMon#############
from MuonTrackMonitoring.MuonTrackMonitoringConf import TGCStandaloneTracksMon
############# NO TriggerAware Trk Monitoring #######################
if MuonDQAFlags.doMuonTrackMon:

   #############MuonSelectedTracksMon#############
   MuonSelectedTracksMon = MuonSelectedTracksMon(name = "MuonSelectedTracksMon",
                                                 WhichTrackCollections = [0,1,1],
                                                 MuonTrackCollections = ["ExtrapolatedMuonSpectrometerTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                 NPhiBins = 360,
                                                 UseTriggerVector = False,
                                                 MuonTriggerChainName = "NoMuonTriggerSelection" )
   #############MuonGenericTracksMon#############
   MuonGenericTracksMon = MuonGenericTracksMon(name = "MuonGenericTracksMon",
                                               WhichTrackCollections = [0,1,1],
                                               MuonTrackCollections = ["ExtrapolatedMuonSpectrometerTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                               NPhiBins = 360,
                                               UseTriggerVector = False,
                                               MuonTriggerChainName = "NoMuonTriggerSelection" )
   #############TGCStandaloneTracksMon_Trig############
   TGCStandaloneTracksMon_NoTrig = TGCStandaloneTracksMon(name = "TGCStandaloneTracksMon",
                                                          TriggerAware = False,
                                                          MuonTriggerChainName = "TriggersInChain")
   ToolSvc += MuonSelectedTracksMon
   ToolSvc += MuonGenericTracksMon
   ToolSvc += TGCStandaloneTracksMon
   monMan.AthenaMonTools += [ MuonSelectedTracksMon ]
   monMan.AthenaMonTools += [ MuonGenericTracksMon ]
   monMan.AthenaMonTools += [ TGCStandaloneTracksMon ]
 
#############TriggerAware Trk Monitoring#######################
## set to true in case you would like to use trigger-aware monitoring
## only do trigger-aware monitoring if DQMonFlags.useTrigger is true
if not DQMonFlags.useTrigger():
   print "IDPerfMon_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring"
else:
   if MuonDQAFlags.MuonTrkMonDoTrigger and MuonDQAFlags.doMuonTrackMon:
      #############MuonSelectedTracksMon_Trig#############
      MuonSelectedTracksMon_Trig = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_Trig",
                                                         WhichTrackCollections = [0,1,1],
                                                         MuonTrackCollections = ["MooreTracks","ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                         NPhiBins = 360,
                                                         UseTriggerVector = True,
                                                         MuonTriggerDecisionTool = "TrigDec::TrigDecisionTool",
                                                         Muon_Trigger_Items = ["EF_2mu10", "EF_2mu4", "EF_2mu6", "EF_mu10", "EF_mu20", "EF_mu40"],
                                                         MuonTriggerChainName = "TriggersInChain") 
      #############MuonGenericTracksMon_Trig############
      MuonGenericTracksMon_Trig = MuonGenericTracksMon(name = "MuonGenericTracksMon_Trig",
                                                       WhichTrackCollections = [0,1,1],
                                                       MuonTrackCollections = ["MooreTracks","ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                       NPhiBins = 360,
                                                       UseTriggerVector = True,
                                                       MuonTriggerDecisionTool = "TrigDec::TrigDecisionTool",
                                                       Muon_Trigger_Items = ["EF_2mu10", "EF_2mu4", "EF_2mu6", "EF_mu10", "EF_mu20", "EF_mu40"],						    
                                                       MuonTriggerChainName = "TriggersInChain")
      #############TGCStandaloneTracksMon_Trig############
      TGCStandaloneTracksMon_Trig = TGCStandaloneTracksMon(name = "TGCStandaloneTracksMon_Trig",
                                                           TriggerAware = False,
                                                           MuonTriggerDecisionTool = "TrigDec::TrigDecisionTool",
                                                           Muon_Trigger_Items = ["MU0", "MU4", "MU6", "MU10", "MU11", "MU15", "MU20", "MU40", "MU0_TGC", "MU6_TGC", "MU0_TGC_HALO" ],
                                                           MuonTriggerChainName = "TriggersInChain")
      monMan.AthenaMonTools += [ MuonSelectedTracksMon_Trig ]
      monMan.AthenaMonTools += [ MuonGenericTracksMon_Trig]
      monMan.AthenaMonTools += [ TGCStandaloneTracksMon_Trig]

      #############MuonSelectedTracksMon_Trig#############
      MuonSelectedTracksMon_trig2mu4 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trig2mu4",
                                                             WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                             MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                             UseTriggerVector = False )   
      MuonSelectedTracksMon_trig2mu6 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trig2mu6",
                                                             WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                             MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                             UseTriggerVector = False )   
      MuonSelectedTracksMon_trigmu10 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trigmu10",
                                                             WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                             MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                             UseTriggerVector = False )
      MuonSelectedTracksMon_trigmu20 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trigmu20",
                                                             WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                             MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                             UseTriggerVector = False )
      MuonSelectedTracksMon_trigmu40 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trigmu40",
                                                             WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                             MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                             UseTriggerVector = False )
      MuonSelectedTracksMon_trig2mu10 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trig2mu10",
                                                              WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                              MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                              UseTriggerVector = False )
      #############MuonGenericTracksMon_Trig#############
      MuonGenericTracksMon_trig2mu4 = MuonSelectedTracksMon(name = "MuonGenericTracksMon_trig2mu4",
                                                            WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                            MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                            UseTriggerVector = False )   
      MuonGenericTracksMon_trig2mu6 = MuonSelectedTracksMon(name = "MuonGenericTracksMon_trig2mu6",
                                                            WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                            MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                            UseTriggerVector = False )   
      MuonGenericTracksMon_trigmu10 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trigmu10",
                                                           WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                           MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                           UseTriggerVector = False)
      MuonGenericTracksMon_trigmu20 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trigmu20",
                                                           WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                           MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                           UseTriggerVector = False)
      MuonGenericTracksMon_trigmu40 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trigmu40",
                                                           WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                           MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                           UseTriggerVector = False)
      MuonGenericTracksMon_trig2mu10 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trig2mu10",
                                                            WhichTrackCollections = [0,1,1], EnableLumi = True,
                                                            MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks", "MuidExtrapolatedTracks" ],
                                                            UseTriggerVector = False)
      MuonSelectedTracksMon_trig2mu4.TrigDecisionTool = tdt
      MuonSelectedTracksMon_trig2mu6.TrigDecisionTool = tdt
      MuonSelectedTracksMon_trigmu10.TrigDecisionTool = tdt
      MuonSelectedTracksMon_trigmu20.TrigDecisionTool = tdt
      MuonSelectedTracksMon_trigmu40.TrigDecisionTool = tdt
      MuonSelectedTracksMon_trig2mu10.TrigDecisionTool = tdt
      MuonGenericTracksMon_trig2mu4.TrigDecisionTool = tdt
      MuonGenericTracksMon_trig2mu6.TrigDecisionTool = tdt
      MuonGenericTracksMon_trigmu10.TrigDecisionTool = tdt
      MuonGenericTracksMon_trigmu20.TrigDecisionTool = tdt
      MuonGenericTracksMon_trigmu40.TrigDecisionTool = tdt
      MuonGenericTracksMon_trig2mu10.TrigDecisionTool = tdt
   ##############Trigger Items for Muons###############
   ##############MuonSelectedTracksMon#################
      MuonSelectedTracksMon_trig2mu4.TriggerChain = "EF_2mu4"
      MuonSelectedTracksMon_trig2mu6.TriggerChain = "EF_2mu6"
      MuonSelectedTracksMon_trigmu10.TriggerChain = "EF_mu10"
      MuonSelectedTracksMon_trigmu20.TriggerChain = "EF_mu20"
      MuonSelectedTracksMon_trigmu40.TriggerChain = "EF_mu40"
      MuonSelectedTracksMon_trig2mu10.TriggerChain = "EF_2mu10"
      MuonSelectedTracksMon_trig2mu4.MuonTriggerChainName = "EF_2mu4"
      MuonSelectedTracksMon_trig2mu6.MuonTriggerChainName = "EF_2mu6"
      MuonSelectedTracksMon_trigmu10.MuonTriggerChainName = "EF_mu10"   
      MuonSelectedTracksMon_trigmu20.MuonTriggerChainName = "EF_mu20"
      MuonSelectedTracksMon_trigmu40.MuonTriggerChainName = "EF_mu40"
      MuonSelectedTracksMon_trig2mu10.MuonTriggerChainName = "EF_2mu10"
   ##############MuonGenericTracksMon#################
      MuonGenericTracksMon_trig2mu4.TriggerChain = "EF_2mu4"
      MuonGenericTracksMon_trig2mu6.TriggerChain = "EF_2mu6"
      MuonGenericTracksMon_trigmu10.TriggerChain = "EF_mu10"
      MuonGenericTracksMon_trigmu20.TriggerChain = "EF_mu20"
      MuonGenericTracksMon_trigmu40.TriggerChain = "EF_mu40"
      MuonGenericTracksMon_trig2mu10.TriggerChain = "EF_2mu10"
      MuonGenericTracksMon_trig2mu4.MuonTriggerChainName = "EF_2mu4"
      MuonGenericTracksMon_trig2mu6.MuonTriggerChainName = "EF_2mu6"
      MuonGenericTracksMon_trigmu10.MuonTriggerChainName   = "EF_mu10"
      MuonGenericTracksMon_trigmu20.MuonTriggerChainName   = "EF_mu20"
      MuonGenericTracksMon_trigmu40.MuonTriggerChainName   = "EF_mu40"
      MuonGenericTracksMon_trig2mu10.MuonTriggerChainName  = "EF_2mu10"
   ###################################################
      monMan.AthenaMonTools += [ MuonSelectedTracksMon_trig2mu4 ]
      monMan.AthenaMonTools += [ MuonSelectedTracksMon_trig2mu6 ]
      monMan.AthenaMonTools += [ MuonSelectedTracksMon_trigmu10 ]
      monMan.AthenaMonTools += [ MuonSelectedTracksMon_trigmu20 ]
      monMan.AthenaMonTools += [ MuonSelectedTracksMon_trigmu40 ]
      monMan.AthenaMonTools += [ MuonSelectedTracksMon_trig2mu10 ]
      monMan.AthenaMonTools += [ MuonGenericTracksMon_trig2mu4 ]
      monMan.AthenaMonTools += [ MuonGenericTracksMon_trig2mu6 ]
      monMan.AthenaMonTools += [ MuonGenericTracksMon_trigmu10 ]
      monMan.AthenaMonTools += [ MuonGenericTracksMon_trigmu20 ]
      monMan.AthenaMonTools += [ MuonGenericTracksMon_trigmu40 ]
      monMan.AthenaMonTools += [ MuonGenericTracksMon_trig2mu10 ] 
