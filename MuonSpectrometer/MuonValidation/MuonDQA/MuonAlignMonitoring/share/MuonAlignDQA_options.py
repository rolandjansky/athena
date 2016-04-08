#======================================================================
# TOP JobOptions to run on MuonAlignMonitoring
#====================================================================== 
# print '\n********************************************************************************************\n'
print "   ******** Welcome to the Offline MuonAlignMonitoring package. ******** "  
# print "   ******** Documentation may be found at:******** "  
# print "   **** https://twiki.cern.ch/twiki/bin/view/Atlas/MuonOfflineDQA#OfflineTrackAlignMonitoring "  
# print '\n********************************************************************************************\n'

print  'MuonAlignDQA_options.py: package is out-of-date; fix monitoring job options!'


# from AthenaCommon.AppMgr import ServiceMgr
# from AthenaCommon import CfgMgr
# from AthenaCommon.BeamFlags import jobproperties
# from RecExConfig.RecFlags import rec



# muPointAlignMonMan = AthenaMonManager(name="MuonPointAlignMonManager",
#                                       FileKey             = DQMonFlags.monManFileKey(),
#                                       Environment         = DQMonFlags.monManEnvironment(),
#                                       OutputLevel         = muonOutputLevel)
# muSegmAlignMonMan = AthenaMonManager(name="MuonSegAlignMonManager",
#                                      FileKey             = DQMonFlags.monManFileKey(),
#                                      Environment         = DQMonFlags.monManEnvironment(),
#                                      OutputLevel         = muonOutputLevel)


# ###MuonAlignMonitoring##################

# # This is the pointing geometry:  ideally what we want to use this with colliding beams:
# #
# # FIXME:  instead of commenting this out, what to use a flag which picks up if cosmic vs if beam
# #
# #if ( rec.doMuon and rec.doMuonCombined ):
# #     from MuonAlignMonitoring.MuonAlignMonitoringConf import MuonAlignValidation
# #     MuonPointAlignMon = MuonAlignValidation(name = "MuonPointAlignMon",
# #                                        WhichSegmCollections = [1,1],
# #                                        SegmentCollectionName = ["MooreSegments","ConvertedMBoySegments"],
# #                                        MuonTrackCollections = ["MooreTracks","ConvertedMBoyMuonSpectroOnlyTracks"],
# #                                        bMdtSegmentChiSqMax  = 10.,
# #                                        # bMdtSegmentNHolesMax = 2,
# #                                        bMdtSegmentNHotsMin  = 5,
# #                                        eMdtSegmentChiSqMax  = 10.,
# #                                        eMdtSegmentNHolesMax = 2,
# #                                        eMdtSegmentNHotsMin  = 5,
# #                                        mdtSegmentDThetaMax  = 0.2,
# #                                        mdtSegmentDPhiMax    = 0.25,
# #                                        mdtSegmentDZMax      = 100.,
# #                                        mdtSegmentDRMax      = 100.,
# #                                        eCscSegmentChiSqMax  = 100.,
# #                                        eCscSegmentNHotsMin  = 4)
# #     ToolSvc += MuonPointAlignMon
# #     muPointAlignMonMan.AthenaMonTools += [ MuonPointAlignMon ]
# #     topSequence += muPointAlignMonMan
# #     print muPointAlignMonMan
 
# # This is alignment test with segment:   what will work for cosmics without B-field 
# if ( rec.doMuon ):
#      from MuonAlignMonitoring.MuonAlignMonitoringConf import MuonAlignSegment
#      MuonSegAlignMon = MuonAlignSegment(name = "MuonSegAlignMon",
#                                    WhichSegmCollections = [1,1],
#                                    SegmentCollectionName = ["MooreSegments","ConvertedMBoySegments"],
#                                    bMdtSegmentChiSqMax  = 10.,
#                                    bMdtSegmentNHolesMax = 2,
#                                    bMdtSegmentNHotsMin  = 5,
#                                    eMdtSegmentChiSqMax  = 10.,
#                                    eMdtSegmentNHolesMax = 2,
#                                    eMdtSegmentNHotsMin  = 5,
#                                    mdtSegmentDThetaMax  = 0.05,
#                                    mdtSegmentDPhiMax    = 0.05,
#                                    mdtSegmentDZMax      = 100.,
#                                    mdtSegmentDRMax      = 100.,
#                                    eCscSegmentChiSqMax  = 100.,
#                                    eCscSegmentNHotsMin  = 4)
# #     ToolSvc += MuonSegAlignMon
# #     muSegmAlignMonMan.AthenaMonTools += [ MuonSegAlignMon ]
# #     topSequence += muSegmAlignMonMan
# #     print muSegmAlignMonMan
