import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
if jobproperties.ConcurrencyFlags.NumThreads() > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

# use auditors
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
print globalflags

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()
DetFlags.TRT_setOff()

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

# Set up SCT cabling
from AthenaCommon.Include import include
include('InDetRecExample/InDetRecCabling.py')

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------

IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-MC16-SDR-18"

conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/ROD", "/SCT/DAQ/Config/ROD")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/Geog", "/SCT/DAQ/Config/Geog")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/RODMUR", "/SCT/DAQ/Config/RODMUR")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/MUR", "/SCT/DAQ/Config/MUR")

from SCT_ConditionsTools.SCT_RODVetoToolSetup import SCT_RODVetoToolSetup
sct_RODVetoToolSetup = SCT_RODVetoToolSetup()
sct_RODVetoToolSetup.setup()
sct_RODVetoToolSetup.getAlg().BadRODIds = [0x240100, 0x240030]

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_RODVetoTestAlg
job+= SCT_RODVetoTestAlg(SCT_RODVetoTool=sct_RODVetoToolSetup.getTool())


import AthenaCommon.AtlasUnixGeneratorJob
sct_RODVetoToolSetup.getTool().OutputLevel=VERBOSE
ServiceMgr.EventSelector.InitialTimeStamp = 1500000000
ServiceMgr.EventSelector.RunNumber = 300000 # MC16c 2017 run number
theApp.EvtMax = 20

ServiceMgr.MessageSvc.infoLimit = 50000
