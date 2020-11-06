import AthenaCommon.AtlasUnixStandardJob

# Setup logger
from AthenaCommon.Logging import logging
msg = logging.getLogger("testBSError")
msg.setLevel(logging.INFO)

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

# use auditors
from AthenaCommon.AppMgr import ServiceMgr
from GaudiCommonSvc.GaudiCommonSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += ["ChronoAuditor"]
theAuditorSvc.Auditors  += ["MemStatAuditor"]
theApp.AuditAlgorithms=True


#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
msg.info(globalflags)

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
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
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()



import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

# Set up SCT cabling
from AthenaCommon.Include import include
include('InDetRecExample/InDetRecCabling.py')

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------

IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2018-03"

from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
sct_ConfigurationConditionsToolSetup.setup()
SCT_ConfigurationConditionsTool = sct_ConfigurationConditionsToolSetup.getTool()

from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
sct_ByteStreamErrorsToolSetup.setConfigTool(SCT_ConfigurationConditionsTool)
sct_ByteStreamErrorsToolSetup.setup()
SCT_ByteStreamErrorsTool = sct_ByteStreamErrorsToolSetup.getTool()

SCT_ByteStreamErrorsTool.OutputLevel=DEBUG

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ByteStreamErrorsTestAlg
job+= SCT_ByteStreamErrorsTestAlg(ByteStreamErrorsTool=SCT_ByteStreamErrorsTool)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if numThreads >= 2:
    from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
    sctCondAlgCardinality.set(numThreads)
    job.SCT_ByteStreamErrorsTestAlg.Cardinality = numThreads

import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr += PoolSvc()

athenaCommonFlags.FilesInput = [ "./ESD.pool.root" ]
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput.get_Value()

theApp.EvtMax = -1

ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
