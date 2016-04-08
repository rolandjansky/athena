#======================================================================
# TOP JobOptions to run on MuonSegmMonitoring
#====================================================================== 
#print '\n****************************************************************************************\n'
print "   ******** Welcome to the Offline MuonSegmentMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  
#print '\n****************************************************************************************\n'

print  'MuonCSCSegmDQA_options.py: package is out-of-date; fix monitoring job options!'

# if not 'MuonDQAFlags' in dir():
#     print "MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now"
#     from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

# if MuonDQADetFlags.doCSCSegmMon:
#     cscSegmMonMan = AthenaMonManager(name="CSCSegmMonManager",
#                                      FileKey             = DQMonFlags.monManFileKey(),
#                                      Environment         = DQMonFlags.monManEnvironment(),
#                                      OutputLevel         = muonOutputLevel)
#     #====================================================================== 
#     # CSC Segment Monitoring JobOptions
#     #====================================================================== 
#     from MuonSegmMonitoring.MuonSegmMonitoringConf import CSCSegmValAlg
#     myTrigDecisionTool = ""
#     ## TDT instance (this should be done already?)
#     if(DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool())):
#         myTrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
        
#         segmCollections = { "MooreSegments":1, "ConvertedMBoySegments":1 }
#         segmPrefixes = { "MooreSegments":"Moore", "ConvertedMBoySegments":"MuBoy" }
#         segmSlopeCuts = { "MooreSegments":0.07, "ConvertedMBoySegments":0.07 }
        
#         clusStatWords = [ "Unspoiled", "Simple", "Edge", "MultiPeak", "Narrow",
#                           "Wide", "Skewed", "QRatInc", "StripFitFailed",
#                           "SplitUnspoiled", "SplitSimple", "SplitEdge", "SplitMultiPeak",
#                           "SplitNarrow", "SplitWide", "SplitSkewed", "SplitQRatInc",
#                           "SplitStripFitFailed", "Undefined" ]
        
#      ## trigger-aware monitoring: sample seletion triggers (express stream menu physics_pp_v2)
#         evtSelectionTriggers = [ "L1_MU10", "L1_MU15", "EF_mu20_muCombTag_NoEF", "EF_mu15", "EF_mu15_mu10_EFFS", "EF_2mu10", "EF_2mu10_loose" ]
        
#         CSCSegmValAlg = CSCSegmValAlg ( 
#             name = "CSCSegmValAlg", 
#             SegmentKeys = segmCollections, 
#             TrigDecisionTool = myTrigDecisionTool, 
#             DoEventSelection = False, 
#             EventSelTriggers = evtSelectionTriggers,
#             SegmentPrefixes = segmPrefixes, SegmentSlopeCuts = segmSlopeCuts, ClusterStatus = clusStatWords)

#         ToolSvc += CSCSegmValAlg
#         cscSegmMonMan.AthenaMonTools += [ CSCSegmValAlg ]
#         topSequence += cscSegmMonMan
#         print cscSegmMonMan
