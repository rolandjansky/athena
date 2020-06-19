# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Job options file to read data overlay  pool file with LArRawChannel and run LArReadCells
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOn()
DetFlags.Muon_setOff()
DetFlags.Forward_setOff()


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

from RecExConfig.RecFlags import rec
rec.readESD.set_Value_and_Lock(False)
rec.readRDO.set_Value_and_Lock(True)

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0)
jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)

from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits.set_Value_and_Lock(False)

from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LoadElecCalib.set_Value_and_Lock(True)

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArDeadOTXCorr=False
jobproperties.CaloCellFlags.doLArThinnedDigits=False
jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr=False

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "PartPropSvc/PartPropSvc.py" )
#
# Pool Converters
#
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#
# Pool input
#
PoolRDOInput = [
"test1/dataOverlayRDO.pool.root"
]

svcMgr.EventSelector.InputCollections = PoolRDOInput
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = PoolRDOInput

# the Tile, LAr and Calo detector description package
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-R2-2015-03-01-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from IOVDbSvc.CondDB import conddb
conddb.blockFolder("/LAR/LArCellPositionShift")
conddb.addFolderWithTag("LAR_OFL","/LAR/LArCellPositionShift","LArCellPositionShift-ideal",force=True,forceMC=True) 

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
include("TileConditions/TileConditions_jobOptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" ) 

from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.readInputFile('RecExPers/OKS_streamRDO.py')

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from CaloRec.CaloCellGetter import CaloCellGetter
CaloCellGetter()

topSequence += CfgMgr.LArReadCells()
topSequence.LArReadCells.etCut = -1e6

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#-------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO

if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["SPLASH DATAFILE='ntuple1.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000000
theApp.EvtSel = "EventSelector"

