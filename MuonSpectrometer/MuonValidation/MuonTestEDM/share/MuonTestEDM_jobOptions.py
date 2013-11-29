# Begin Test_jobOptions.py
# ------------------------

# ------------------------------------------------------------
# TEST EDM
# ------------------------------------------------------------
SegCollNames   = ["MuonSegments"]
TrackCollNames = ["MuonSpectrometerTracks" ]
TrackPartCollNames = ["MuonSpectrometerTrackParticles"]

from MuonTestEDM.MuonTestEDMConf import MuonTestEDM
MyEDMTester = MuonTestEDM(SegmentCollectionNames=SegCollNames, TrackCollectionNames=TrackCollNames, DoDetailedChecks=False)
MyEDMTester.DoDumpPRDs=True
MyEDMTester.DoDumpTracks=True
MyEDMTester.DoDumpRDOs=True
MyEDMTester.DoDumpSegments=True

if not 'DumpFileName' in dir(): DumpFileName="MuonTestEDM"

# override output names if necessary
if 'DumpFileName' in dir():
    MyEDMTester.RdoDumpFileName    = DumpFileName+".rdo.log"
    MyEDMTester.PrdDumpFileName    = DumpFileName+".prd.log"
    MyEDMTester.TrackDumpFileName  = DumpFileName+".track.log"
    MyEDMTester.SegmentDumpFileName= DumpFileName+".segment.log"
    MyEDMTester.SummaryDumpFileName= DumpFileName+".summary.log"


topSequence += MyEDMTester

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# create a THistSvc configurable *only* if not there already
if not hasattr(svcMgr, 'THistSvc'):
   svcMgr += CfgMgr.THistSvc()
# short-hand
hsvc = svcMgr.THistSvc

RootTupleOutputFileName = DumpFileName+'.root'

#--------------------------------------------------------------  
# ntuples: AANT  (Athena Aware NTuples)
#-------------------------------------------------------------- 
# if os.path.exists(RootTupleOutputFileName):
#     os.remove(RootTupleOutputFileName)

hsvc.Output += ["MuonHits DATAFILE='%s' OPT='RECREATE'" % RootTupleOutputFileName]



#print MyEDMTester

#ServiceMgr.PoolSvc.AttemptCatalogPatch=True


# End Test_jobOptions.py
# ------------------------

