###############################################################
#
# Job options file to run Digitization
#
#==============================================================

#these lines is to have information on what is installed
theAuditorSvc = svcMgr.AuditorSvc
theApp.AuditAlgorithms=True
theApp.AuditServices=True
theApp.AuditTools=True
theAuditorSvc += CfgMgr.NameAuditor()
#=================

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

from AthenaCommon.DetFlags import DetFlags
#David :
DetFlags.detdescr.all_setOn()
DetFlags.Muon_setOff()
DetFlags.ID_setOff()
# DetFlags.Tile_setOff()
# DetFlags.readRDOBS.LAr_setOn()
# DetFlags.makeRIO.Calo_setOn()

#ADDED TO CHECK
#David :
#DetFlags.Calo_setOn()

from RecExConfig.RecFlags import rec
rec.doCBNT=False
rec.doMuon = False
rec.doInDet = False
rec.doTrigger =False
#rec.doTile = False
rec.doESD = False
rec.doWriteESD = False
rec.doAOD = False
rec.doWriteAOD = False
rec.doWriteRDO = False

#do the print
rec.doDumpProperties=True

include( "PartPropSvc/PartPropSvc.py" )

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Get a handle to the ApplicationManager
#from AthenaCommon.AppMgr import theApp

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = ["ESD.pool.root"]


from LArConditionsCommon.LArCondFlags import larCondFlags

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-00-00-00'
#jobproperties.Global.DetDescrVersion='ATLAS-CSC-00-00-00'

include("RecExCommon/RecExCommon_topOptions.py")

from IOVDbSvc import IOVDb
svcMgr.IOVDbSvc.GlobalTag = "COMCOND-ES1C-000-00"
svcMgr.IOVDbSvc.OutputLevel      = DEBUG

from LArAffectedRegion.LArAffectedRegionConf import LArAffectedRegionAlg
theLArAffectedRegionAlg=LArAffectedRegionAlg()

# if we read from ESD, AOD, etc. set the following to false
theLArAffectedRegionAlg.ReadingFromBytestream = False
# Need the following when reading from ESD/AOD/etc.
#avoid searching on oracle
svcMgr.IOVDbSvc.Folders += [ "/LAR/LArAffectedRegionInfo<metaOnly/>" ]

topSequence += theLArAffectedRegionAlg

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
