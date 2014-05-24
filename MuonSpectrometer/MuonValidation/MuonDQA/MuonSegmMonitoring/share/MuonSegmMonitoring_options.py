#======================================================================
# TOP JobOptions to run on MuonSegmMonitoring
#====================================================================== 
from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags
from MuonSegmMonitoring.MuonSegmMonitoringConf import MuonSegmValidation
if MuonDQAFlags.doMuonSegmMon:
   #############MuonSegmValidation#############
   MuonSegmValidation = MuonSegmValidation (
   name = "MuonSegmValidation" )
   ### Which Segm Collections to read (set array element to 0 or 1):
   # 1-> Moore, 2->MuonBoy
   MuonSegmValidation.WhichSegmCollections = [1,1]
   MuonSegmValidation.MuonSegmCollections = ["MooreSegments" ,"ConvertedMBoySegments" ]
   ToolSvc += MuonSegmValidation
   monMan.AthenaMonTools += [ MuonSegmValidation ]

   #====================================================================== 
   # CSC Segment Monitoring JobOptions
   #====================================================================== 
   from MuonSegmMonitoring.MuonSegmMonitoringConf import CSCSegmValAlg
   segmCollections = { "MooreSegments":1, "ConvertedMBoySegments":1 }
   CSCSegmValAlg = CSCSegmValAlg ( name = "CSCSegmValAlg", MuonSegmentKeys = segmCollections )
   ToolSvc += CSCSegmValAlg
   monMan.AthenaMonTools += [ CSCSegmValAlg ]

