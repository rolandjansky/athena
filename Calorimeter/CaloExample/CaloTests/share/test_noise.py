###############################################################
#
# Job options file to run Digitization
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_geant4() 
DetFlags.detdescr.all_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

DetFlags.readRDOPool.LAr_setOn()
DetFlags.makeRIO.LAr_setOn()


include( "PartPropSvc/PartPropSvc.py" )


from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.007234.singlepart_mu200.simul.HITS.v12000301_tid003123._00001.pool.root" ]


# the Tile, LAr and Calo detector description package
DetDescrVersion = "ATLAS-CSC-01-02-00" 

include( "DetDescrDictionary/DetDescrDictionaryDict_joboptions.py" )
include ("AtlasGeoModel/SetGeometryVersion.py")
include( "AtlasGeoModel/GeoModelInit.py" )
include( "TileConditions/TileConditions_jobOptions.py" )
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )


#force alignment to use ideal one
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.GlobalTag="OFLCOND-CSC-00-00-00"



from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

 	

from MyAnalysis.MyAnalysisConf import MyAnalysis__Analysis
topSequence += MyAnalysis__Analysis()

Analysis=MyAnalysis__Analysis("Analysis")


Analysis.LoopRaw = FALSE
Analysis.Check = FALSE
Analysis.LoopHit = TRUE
Analysis.LoopCalHit = FALSE
Analysis.LoopCluster = FALSE
Analysis.LoopCell = FALSE
Analysis.UseTriggerTime = FALSE;
# Analysis.TriggerTimeToolName = "CosmicTriggerTimeTool"

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = VERBOSE
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

THistSvc = Service ( "THistSvc" )
THistSvc.Output  = ["file1 DATAFILE='test.root' OPT='RECREATE'"];


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1
theApp.EvtSel = "EventSelector"

# Use auditors
theApp.Dlls += [ "GaudiAud" ]
#
# write out a summary of the time spent
#
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
MemStatAuditor.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.infoLimit  = 100000000
MessageSvc.OutputLevel = VERBOSE

#StoreGateSvc=Service("StoreGateSvc");
#StoreGateSvc.dump=TRUE

