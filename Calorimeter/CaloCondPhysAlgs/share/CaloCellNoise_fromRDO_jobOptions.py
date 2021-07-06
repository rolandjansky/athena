# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Job options file to run simple algorithm computing noise per cell
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')

DetFlags.detdescr.all_setOn()
DetFlags.Muon_setOff()
DetFlags.ALFA_setOff()

DetFlags.readRDOPool.LAr_setOn()
DetFlags.makeRIO.LAr_setOn()
DetFlags.digitize.LAr_setOff()

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "PartPropSvc/PartPropSvc.py" )
#
# Pool Converters
#
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )
include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp
#
# Pool input
#
svcMgr.EventSelector.InputCollections = [
"calib0.007061.singlepart_e_E100.digit.RDO.v12003101_tid003422._00351.pool.root.2"
]
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections
from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Input.Files = athenaCommonFlags.FilesInput()

# the Tile, LAr and Calo detector description package
#from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.DetDescrVersion='ATLAS-CSC-01-00-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" ) 

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.readInputFile('RecExCond/OKS_streamRDO.py')

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doCaloCluster = False
jobproperties.CaloRecFlags.doEmCluster = False
jobproperties.CaloRecFlags.doCaloTopoCluster = False

include( "CaloRec/CaloRec_jobOptions.py" )

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()

from CaloCondPhysAlgs.CaloCondPhysAlgsConf import CaloCellNoiseAlg
myNoise = CaloCellNoiseAlg("CaloCellNoiseAlg")
myNoise.doMC = True
myNoise.readNtuple = False
myNoise.doFit = True
myNoise.TotalNoiseKey = ''
myNoise.ElecNoiseKey = ''

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += myNoise


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
myNoise.OutputLevel        = DEBUG

if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='cellnoise.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 100
theApp.EvtSel = "EventSelector"
