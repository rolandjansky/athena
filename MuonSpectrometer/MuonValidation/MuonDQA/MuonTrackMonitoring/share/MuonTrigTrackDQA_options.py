## #======================================================================
# # top JobOptions to run on TriggerAware MuonTrackMonitoring
# #======================================================================
# ## #--------------------------#
# ## #   MuonTrack  Monitoring  # 
# ## #--------------------------#
# print '\n****************************************************************************************\n'
# print "   ******** Welcome to the Offline MuonTrackMonitoring package. ******** "  
# print "   ******** Documentation may be found at:******** "  
# print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  
# print '\n****************************************************************************************\n'

# #muTrackMonMan = AthenaMonManager(name="MuonTrackMonManager",
# #                                 FileKey             = DQMonFlags.monManFileKey(),
# #                                 Environment         = DQMonFlags.monManEnvironment(),
# #                                 OutputLevel         = muonOutputLevel)
					

# from MuonRecExample import MuonRecTools
# from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonSelectedTracksMon
# from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonGenericTracksMon
# from MuonTrackMonitoring.MuonTrackMonitoringConf import TGCStandaloneTracksMon
# from MuonTrackMonitoring.MuonTrackMonitoringConf import RPCStandaloneTracksMon

# ############TriggerAware Trk Monitoring#######################
# # set to true in case you would like to use trigger-aware monitoring
# # only do trigger-aware monitoring if monTrigDecTool known by ToolSvc
# if not hasattr(ToolSvc, 'monTrigDecTool'):
#    print "MuonTrigTrackDQA_options.py: trigger decision tool not found: don't run trigger-aware monitoring"
# else:
#    #############MuonSelectedTracksMon_Trig#############
#    MuonSelectedTracksMon_Trig = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_Trig",
#                                                       WhichTrackCollections = [1,1],
#                                                       MuonTrackCollections = ["MooreTracks","ConvertedMBoyTracks" ],
#                                                       NPhiBins = 360,
#                                                       UseTriggerVector = True,
#                                                       Muon_Trigger_Items = ["EF_2mu10", "EF_2mu4", "EF_2mu6", "EF_mu10", "EF_mu20", "EF_mu40"],
#                                                       MuonTriggerChainName = "TriggersInChain",
#                                                       OutputLevel = INFO)					  
#    ###########MuonGenericTracksMon_Trig############
#    MuonGenericTracksMon_Trig = MuonGenericTracksMon(name = "MuonGenericTracksMon_Trig",
#                                                     WhichTrackCollections = [1,1],
#                                                     MuonTrackCollections = ["MooreTracks","ConvertedMBoyTracks" ],
#                                                     NPhiBins = 360,
#                                                     UseTriggerVector = True,
#                                                     Muon_Trigger_Items = ["EF_2mu10", "EF_2mu4", "EF_2mu6", "EF_mu10", "EF_mu20", "EF_mu40"],
#                                                     MuonTriggerChainName = "TriggersInChain",
#                                                     OutputLevel = INFO)
#    ###########TGCStandaloneTracksMon_Trig############
# #   TGCStandaloneTracksMon_Trig = TGCStandaloneTracksMon(name = "TGCStandaloneTracksMon_Trig",
# #                                                        TriggerAware = True,
# #							TriggerDecisionTool = monTrigDecTool,
# #                                                        Muon_Trigger_Items = ["MU0", "MU4", "MU6", "MU10", "MU11", "MU15", "MU20", "MU40", "MU0_TGC", "MU6_TGC", "MU0_TGC_HALO" ],
# #                                                        OutputLevel = INFO)
#    ############################################################
#    ToolSvc += MuonSelectedTracksMon_Trig
#    ToolSvc += MuonGenericTracksMon_Trig
# #   ToolSvc += TGCStandaloneTracksMon_Trig
#    muTrackMonMan.AthenaMonTools += [ MuonSelectedTracksMon_Trig ]
#    muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_Trig]
# #   muTgcTrackMonMan.AthenaMonTools += [ TGCStandaloneTracksMon_Trig]
#    print muTrackMonMan
# #   print muTgcTrackMonMan    
#    ###########MuonSelectedTracksMon_Trig - Specific#############
#    MuonSelectedTracksMon_trigmu10 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trigmu10",
#                                                           WhichTrackCollections = [1,1],
#                                                           MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                           UseTriggerVector = False,
#                                                           OutputLevel = INFO)
#    MuonSelectedTracksMon_trigmu20 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trigmu20",
#                                                           WhichTrackCollections = [1,1],
#                                                           MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                           UseTriggerVector = False,
#                                                           OutputLevel = INFO)
#    MuonSelectedTracksMon_trigmu40 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trigmu40",
#                                                           WhichTrackCollections = [1,1],
#                                                           MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                           UseTriggerVector = False,
#                                                           OutputLevel = INFO)
#    MuonSelectedTracksMon_trig2mu10 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trig2mu10",
#                                                            WhichTrackCollections = [1,1],
#                                                            MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                            UseTriggerVector = False,
#                                                            OutputLevel = INFO)
#    MuonSelectedTracksMon_trig2mu4 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trig2mu4",
#                                                           WhichTrackCollections = [1,1],
#                                                           MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                           UseTriggerVector = False,
#                                                           OutputLevel = INFO)
#    MuonSelectedTracksMon_trig2mu6 = MuonSelectedTracksMon(name = "MuonSelectedTracksMon_trig2mu6",
#                                                           WhichTrackCollections = [1,1],
#                                                           MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                           UseTriggerVector = False,
#                                                           OutputLevel = INFO)
#    MuonSelectedTracksMon_trigmu10.TrigDecisionTool = monTrigDecTool
#    MuonSelectedTracksMon_trigmu20.TrigDecisionTool = monTrigDecTool
#    MuonSelectedTracksMon_trigmu40.TrigDecisionTool = monTrigDecTool
#    MuonSelectedTracksMon_trig2mu10.TrigDecisionTool = monTrigDecTool
#    MuonSelectedTracksMon_trig2mu4.TrigDecisionTool = monTrigDecTool
#    MuonSelectedTracksMon_trig2mu6.TrigDecisionTool = monTrigDecTool
#    MuonSelectedTracksMon_trigmu10.TriggerChain = "EF_mu10"
#    MuonSelectedTracksMon_trigmu20.TriggerChain = "EF_mu20"
#    MuonSelectedTracksMon_trigmu40.TriggerChain = "EF_mu40"
#    MuonSelectedTracksMon_trig2mu10.TriggerChain = "EF_2mu10"
#    MuonSelectedTracksMon_trig2mu4.TriggerChain = "EF_2mu4"
#    MuonSelectedTracksMon_trig2mu6.TriggerChain = "EF_2mu6"
#    MuonSelectedTracksMon_trigmu10.MuonTriggerChainName = "EF_mu10"
#    MuonSelectedTracksMon_trigmu20.MuonTriggerChainName = "EF_mu20"
#    MuonSelectedTracksMon_trigmu40.MuonTriggerChainName = "EF_mu40"
#    MuonSelectedTracksMon_trig2mu10.MuonTriggerChainName = "EF_2mu10"
#    MuonSelectedTracksMon_trig2mu4.MuonTriggerChainName = "EF_2mu4"
#    MuonSelectedTracksMon_trig2mu6.MuonTriggerChainName = "EF_2mu6"
#    ToolSvc += MuonSelectedTracksMon_trigmu10
#    ToolSvc += MuonSelectedTracksMon_trigmu20
#    ToolSvc += MuonSelectedTracksMon_trigmu40
#    ToolSvc += MuonSelectedTracksMon_trig2mu10
#    ToolSvc += MuonSelectedTracksMon_trig2mu4
#    ToolSvc += MuonSelectedTracksMon_trig2mu6
#    muTrackMonMan.AthenaMonTools += [ MuonSelectedTracksMon_trigmu10]
#    muTrackMonMan.AthenaMonTools += [ MuonSelectedTracksMon_trigmu20]
#    muTrackMonMan.AthenaMonTools += [ MuonSelectedTracksMon_trigmu40]
#    muTrackMonMan.AthenaMonTools += [ MuonSelectedTracksMon_trig2mu10]
#    muTrackMonMan.AthenaMonTools += [ MuonSelectedTracksMon_trig2mu4]
#    muTrackMonMan.AthenaMonTools += [ MuonSelectedTracksMon_trig2mu6]      
#    print muTrackMonMan
#    ###########MuonGenericTracksMon_Trig - Specific#############
#    MuonGenericTracksMon_trigmu10 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trigmu10",
#                                                         WhichTrackCollections = [1,1],
#                                                         MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                         UseTriggerVector = False,
#                                                         OutputLevel = INFO)
#    MuonGenericTracksMon_trigmu20 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trigmu20",
#                                                         WhichTrackCollections = [1,1],
#                                                         MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                         UseTriggerVector = False,
#                                                         OutputLevel = INFO)
#    MuonGenericTracksMon_trigmu40 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trigmu40",
#                                                         WhichTrackCollections = [1,1],
#                                                         MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                         UseTriggerVector = False,
#                                                         OutputLevel = INFO)
#    MuonGenericTracksMon_trig2mu10 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trig2mu10",
#                                                          WhichTrackCollections = [1,1],
#                                                          MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                          UseTriggerVector = False,
#                                                          OutputLevel = INFO)
#    MuonGenericTracksMon_trig2mu4 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trig2mu4",
#                                                         WhichTrackCollections = [1,1],
#                                                         MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                         UseTriggerVector = False,
#                                                         OutputLevel = INFO)
#    MuonGenericTracksMon_trig2mu6 = MuonGenericTracksMon(name = "MuonGenericTracksMon_trig2mu6",
#                                                         WhichTrackCollections = [1,1],
#                                                         MuonTrackCollections = ["MooreTracks" ,"ConvertedMBoyTracks" ],
#                                                         UseTriggerVector = False,
#                                                         OutputLevel = INFO)
#    MuonGenericTracksMon_trigmu10.TrigDecisionTool = monTrigDecTool
#    MuonGenericTracksMon_trigmu20.TrigDecisionTool = monTrigDecTool
#    MuonGenericTracksMon_trigmu40.TrigDecisionTool = monTrigDecTool
#    MuonGenericTracksMon_trig2mu10.TrigDecisionTool = monTrigDecTool
#    MuonGenericTracksMon_trig2mu4.TrigDecisionTool = monTrigDecTool
#    MuonGenericTracksMon_trig2mu6.TrigDecisionTool = monTrigDecTool
#    MuonGenericTracksMon_trigmu10.TriggerChain = "EF_mu10"
#    MuonGenericTracksMon_trigmu20.TriggerChain = "EF_mu20"
#    MuonGenericTracksMon_trigmu40.TriggerChain = "EF_mu40"
#    MuonGenericTracksMon_trig2mu10.TriggerChain = "EF_2mu10"
#    MuonGenericTracksMon_trig2mu4.TriggerChain = "EF_2mu4"
#    MuonGenericTracksMon_trig2mu6.TriggerChain = "EF_2mu6"
#    MuonGenericTracksMon_trigmu10.MuonTriggerChainName = "EF_mu10"
#    MuonGenericTracksMon_trigmu20.MuonTriggerChainName = "EF_mu20"
#    MuonGenericTracksMon_trigmu40.MuonTriggerChainName = "EF_mu40"
#    MuonGenericTracksMon_trig2mu10.MuonTriggerChainName = "EF_2mu10"
#    MuonGenericTracksMon_trig2mu4.MuonTriggerChainName = "EF_2mu4"
#    MuonGenericTracksMon_trig2mu6.MuonTriggerChainName = "EF_2mu6"
#    ToolSvc += MuonGenericTracksMon_trigmu10
#    ToolSvc += MuonGenericTracksMon_trigmu20
#    ToolSvc += MuonGenericTracksMon_trigmu40
#    ToolSvc += MuonGenericTracksMon_trig2mu10
#    ToolSvc += MuonGenericTracksMon_trig2mu4
#    ToolSvc += MuonGenericTracksMon_trig2mu6
#    muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_trigmu10]
#    muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_trigmu20]
#    muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_trigmu40]
#    muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_trig2mu10]
#    muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_trig2mu4]
#    muTrackMonMan.AthenaMonTools += [ MuonGenericTracksMon_trig2mu6]
#    print muTrackMonMan
