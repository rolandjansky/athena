###############################################################
#
# Job options file for CaloNoiseDbExample
# Simple readback of some values is performed.
#
#                                           nils.gollub@cern.ch
#==============================================================
import AthenaCommon.AtlasUnixGeneratorJob
from   AthenaCommon import CfgMgr
from   AthenaCommon.AppMgr import theApp
from   AthenaCommon.AppMgr import ServiceMgr as svcMgr
from   AthenaCommon.AppMgr import ToolSvc


# Setup Db stuff
import AthenaPoolCnvSvc.AthenaPool
import IOVDbSvc.IOVDb

#-------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
#DetFlags.LAr_setOn()
DetFlags.Tile_setOn()
#DetFlags.digitize.all_setOff()

# Get a handle to the default top-level algorithm sequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# Setup Db stuff
import AthenaPoolCnvSvc.AthenaPool

from AthenaCommon.GlobalFlags import jobproperties
# jobproperties.Global.DetDescrVersion=Geometry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#------------------------------------

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from CaloCondBlobAlgs.CaloCondBlobAlgsConf import CaloNoiseDbExample
topSequence += CaloNoiseDbExample( "CaloNoiseDbExample" )

#--------------------------------------------------------------
#---Specify folder, tag and dbConnection explicitely
#--------------------------------------------------------------
#svcMgr.IOVDbSvc.dbConnection="sqlite://DUMMY;schema=caloSqlite.db;dbname=COMP200"
#connStr = "<dbConnection>sqlite://DUMMY;schema=caloSqlite.db;dbname=COMP200</dbConnection>"
#connStr ="<dbConnection>COOLONL_CALO/COMP200</dbConnection>"
connStr ="<dbConnection>COOLOFL_CALO/COMP200</dbConnection>"
#tag     = "<tag>CaloNoiseCellnoise-UPD1-00</tag>"
tag     = "<tag>CaloOflNoiseCellnoise-UPD1-00</tag>"
folder  = "/CALO/Ofl/Noise/CellNoise"
#folder  = "/CALO/Noise/CellNoise"
svcMgr.IOVDbSvc.Folders += [ folder + tag + connStr ]

#----------- properties
CaloNoiseDbExample.FolderName           = folder

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = 919900
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 1

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = WARNING
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
svcMgr.IOVDbSvc.OutputLevel        = INFO
CaloNoiseDbExample.OutputLevel     = DEBUG

