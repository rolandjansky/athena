###############################################################
#
# Job options file to run Digitization for CTB Calo only
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.DataSource.set_geant4() 
DetFlags.detdescr.all_setOn()
DetFlags.readRDOPool.LAr_setOn()
DetFlags.makeRIO.LAr_setOn()
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.pileup.all_setOff()

# global tag (not set by simulation) (for LAr not relevant for CBT)
GlobalTag = "OFLCOND-CSC-00-00-00"


include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

include( "PartPropSvc/PartPropSvc.py" )

#
# Pool Converters
#
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )

#
# Pool input
#
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AthenaCommonFlags import jobproperties
svcMgr.EventSelector.InputCollections =jobproperties.AthenaCommonFlags.FilesInput.get_Value() 
#["ctb_MyOutputFile.root"]

# the Tile, LAr and Calo detector description package
from AtlasGeoModel import SetGeometryVersion

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.AtlasVersion = "ATLAS-CTB-01"

CTBforTTCellMap=True
from AtlasGeoModel import GeoModelInit

#configure LAr to read from OFLP200
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LArDBConnection="<dbConnection>COOLONL_LAR/OFLP200</dbConnection>"

include("LArDetDescr/LArDetDescr_H8_joboptions.py")
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='CTB'
DetDescrVersion = "CTB"

# difference is in the IDET tag which is not defined from the jo above and which is found in the input file
ServiceMgr.GeoModelSvc.IgnoreTagDifference = True


# LAr Digitization
from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.doCaloNoise = True
jobproperties.Digitization.physicsList = 'QGSP_BERT'

include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
from LArROD.LArRawChannelGetter import LArRawChannelGetter
LArRawChannelGetter()

# Tile digitization
include( "TileConditions/TileTBConditions_jobOptions.py" ) 
include( "TileSimAlgs/TileTBDigitization_jobOptions.py" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      =  INFO

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
#theApp.EvtMax = 20
theApp.EvtSel = "EventSelector"

# Use auditors
theApp.Dlls += [ "GaudiAud" ]
#
# write out a summary of the time spent
#
from GaudiSvc.GaudiSvcConf import AuditorSvc
svcMgr += AuditorSvc()
from GaudiSvc.GaudiSvcConf import ChronoStatSvc
svcMgr += ChronoStatSvc()

from GaudiSvc.GaudiSvcConf import MessageSvc
svcMgr += MessageSvc( defaultLimit = 100000000 )

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream("Stream1")
Stream1.OutputFile = "ctb_CaloDigit.pool.root"
Stream1.ForceRead=TRUE
Stream1.ItemList+=["EventInfo#*"]
Stream1.ItemList+=["LArRawChannelContainer#*"]
Stream1.ItemList+=["TileRawChannelContainer#*"]
Stream1.ItemList+=["McEventCollection#*"]    
Stream1.ItemList += ["CaloCalibrationHitContainer#*"]

svcMgr.IOVDbSvc.GlobalTag = GlobalTag
