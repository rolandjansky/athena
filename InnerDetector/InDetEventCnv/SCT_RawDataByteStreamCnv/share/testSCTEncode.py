###############################################################
#
# Job options file to test SCT encoding
#
#==============================================================

if not "doPrint" in dir():
    doPrint = False
if not "EvtMax" in dir():
    EvtMax = 10

msg = None
if doPrint:
    from AthenaCommon.Logging import logging
    msg = logging.getLogger("testSCTEncode")
    msg.setLevel(logging.INFO)

#--------------------------------------------------------------
# Standard includes
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
numThreads = jobproperties.ConcurrencyFlags.NumThreads()
if numThreads > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R2-2015-03-01-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
if doPrint:
    msg.info(globalflags)

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.SCT_setOn()
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()

# ---- switch parts of ID off/on as follows
#switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.writeBS.SCT_setOn()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.readRDOPool.all_setOff()
DetFlags.readRDOPool.SCT_setOn()
DetFlags.writeRIOPool.all_setOff()

if doPrint:
    DetFlags.Print()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit
# import MagFieldServices.SetupField

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
conddb.dbdata="OFLP200"
IOVDbSvc.GlobalTag="OFLCOND-RUN12-SDR-31"
IOVDbSvc.OutputLevel = WARNING

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condAlgName = "SCT_CablingCondAlgFromCoraCool"
if not hasattr(condSeq, condAlgName):
    from AthenaCommon.CfgGetter import getAlgorithm
    SCT_CablingCondAlgFromCoraCool = getAlgorithm(condAlgName)
    condSeq += SCT_CablingCondAlgFromCoraCool

# Set input byte stream file (from RecExCommon/myTopOptions.py)
inputRDOFiles = ["/afs/cern.ch/atlas/project/rig/referencefiles/RTTinputFiles/MC15_13TeV/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r6699_10evt.pool.root"]

# Set up POOL file reading
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = inputRDOFiles

# Set up event info cnv alg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()

# Set up byte stream writing
include("ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py")

from ByteStreamCnvSvc import WriteByteStream
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")
StreamBSFileOutput.ItemList += [ "SCT_RDO_Container#SCT_RDOs" ]
svcMgr.ByteStreamEventStorageOutputSvc.WriteEventlessFiles = False

# Print algorithms
if doPrint:
    msg.info(topSequence)

# Set the number of events to be processed
theApp.EvtMax = EvtMax

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = DEBUG
ServiceMgr.MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

if numThreads >= 2:
    from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
    sctCondAlgCardinality.set(numThreads)
