#======================================================================
# TOP JobOptions to run on MuonSegmMonitoring
#====================================================================== 
#print '\n****************************************************************************************\n'
print "   ******** Welcome to the Offline MuonSegmentMonitoring package. ******** "  
#print "   ******** Documentation may be found at:******** "  
#print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA ***** "  
#print '\n****************************************************************************************\n'

print " MuonSegmDQA_options.py: package is out-of-date; fix monitoring job options! "

# if not 'MuonDQAFlags' in dir():
#     print "MuonDQAFlags.py: MuonDQAFlags not yet imported - I import them now"
#     from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

# if MuonDQADetFlags.doMuonSegmMon:
#     muSegmMonMan = AthenaMonManager(name="MuonSegmMonManager",
#                                     FileKey             = DQMonFlags.monManFileKey(),
#                                     Environment         = DQMonFlags.monManEnvironment(),
#                                     OutputLevel         = muonOutputLevel)
#     ###MuonSegmMonitoring##################
#     from MuonSegmMonitoring.MuonSegmMonitoringConf import MuonSegmValidation
#     # Which Segm Collections to read (set array element to 0 or 1):
#     # 1-> Moore, 2->MuonBoy
#     MuonSegmValidation = MuonSegmValidation(name = "MuonSegmValidation",
#                                             WhichSegmCollections = [1,1],
#                                             MuonSegmCollections = ["MooreSegments" ,"ConvertedMBoySegments" ],
#                                             doMDT = True,
#                                             doRPC = True,
#                                             doTGC = True,
#                                             doCSC = False )
#     ToolSvc += MuonSegmValidation
#     muSegmMonMan.AthenaMonTools += [ MuonSegmValidation ]
#     topSequence += muSegmMonMan
#     print muSegmMonMan
