###############################################################
#
# Job options file to run Digitization
#
#==============================================================

from AthenaCommon.Resilience import treatException,protectedInclude

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
globalflags.InputFormat = 'bytestream'

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOn()
DetFlags.Muon_setOff()
DetFlags.ID_setOff()
DetFlags.Tile_setOff()
DetFlags.ALFA_setOff()
DetFlags.readRDOBS.LAr_setOn()
DetFlags.makeRIO.Calo_setOn()

#ADDED TO CHECK
DetFlags.Calo_setOn()

from RecExConfig.RecFlags import rec
rec.doMuon = False
rec.doInDet = False
rec.doTile = False
rec.doESD = False
rec.doWriteESD = False
rec.doAOD = False
rec.doWriteAOD = False
rec.doWriteRDO = False

include( "PartPropSvc/PartPropSvc.py" )

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.BSRDOInput = ["/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data"]

include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

svcMgr.ByteStreamInputSvc.FullFileName=athenaCommonFlags.BSRDOInput()
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]


from LArConditionsCommon.LArCondFlags import larCondFlags

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-00-00-00'

from IOVDbSvc import IOVDb
svcMgr.IOVDbSvc.GlobalTag = "COMCOND-ES1C-000-00"
svcMgr.IOVDbSvc.OutputLevel      = DEBUG

#=================For HV
from IOVDbSvc.CondDB import conddb
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16")
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8")
#to read HV mapping from database file instead of Ascii file in LArTools
conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/HVLineToElectrodeMap")
conddb.addOverride("/LAR/IdentifierOfl/HVLineToElectrodeMap","LARIdentifierOflHVLineToElectrodeMap-UPD4-01")
#======================

# the Tile, LAr and Calo detector description package
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#    ---- add algorithm
from LArAffectedRegion.LArAffectedRegionConf import LArAffectedRegionAlg
topSequence+=LArAffectedRegionAlg()

#------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#-------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO 
#increase the number of letter reserved to the alg/tool name from 18 to 30
svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" 
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
svcMgr.MessageSvc.useColors = False
svcMgr.MessageSvc.defaultLimit=1000000

#---------------------------------------------------------------------
# Perfmon
#--------------------------------------------------------------------

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc()
jobproperties.PerfMonFlags.doDetailedMonitoring = True


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
theApp.EvtSel = "EventSelector"


#Create output StreamESD
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamESD=AthenaPoolOutputStream("StreamESD",athenaCommonFlags.PoolESDOutput(),True)
StreamESD.ForceRead = True
# Put MetaData in ESD stream via StreamESD_FH.
StreamESD.MetadataItemList += [ "LumiBlockCollection#*" ]

include ("EventAthenaPool/EventAthenaPoolItemList_joboptions.py")
StreamESD.ItemList+=fullItemList

     


#David, instruction to get all the info in the DetectorStore
ServiceMgr.DetectorStore.Dump=True 
