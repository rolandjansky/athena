#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

theApp.EvtMax = 20

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

#---------------------------------------------------------------------------------#
# NEW Conditions access infrastructure
#
from IOVSvc.IOVSvcConf import CondInputLoader
topSequence += CondInputLoader( "CondInputLoader", OutputLevel=DEBUG,  )

import StoreGate.StoreGateConf as StoreGateConf
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc()
# NEW Conditions access infrastructure
#---------------------------------------------------------------------------------#

# Make sure PerfMon is off
include( "PerfMonGPerfTools/DisablePerfMon_jobOFragment.py" )

# Input file
dataFile = "/afs/cern.ch/atlas/project/rig/referencefiles/MC/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.e3099_s2578/RDO.04919495._000958.pool.root.1"

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=[dataFile,dataFile]

# AutoConfiguration
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore
from PyUtils.MetaReaderPeeker import convert_itemList
objKeyStore.addManyTypesInputFile(convert_itemList(layout = '#join'))

#---------------------------------------------------------------------------------#
# Detector Description
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
from TileGeoModel.TileGeoModelConf import TileDetectorTool

ServiceMgr.GeoModelSvc.DetectorTools += [ LArDetectorToolNV(ApplyAlignments = True, GeometryConfig = "RECO"),
                                          TileDetectorTool(GeometryConfig = "RECO")
                                          ]


include( "TileConditions/TileConditions_jobOptions.py" )

include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py")
include( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py")

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
from LArConditionsCommon import LArAlignable
ServiceMgr.DetDescrCnvSvc.DecodeIdDict = True
# Detector Description
#---------------------------------------------------------------------------------#

import AthenaPoolCnvSvc.ReadAthenaPool  #Maybe better to break up to get rid of MetaData stuff

svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from GaudiAlg.GaudiAlgConf import EventCounter
topSequence+=EventCounter(Frequency=2)

from LArROD.LArRODFlags import larRODFlags
larRODFlags.readDigits=False

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.clusterCellGetterName = 'CaloRec.CaloCellGetter.CaloCellGetter'

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArDeadOTXCorr=False

from CaloRec.CaloCellGetter import CaloCellGetter
CaloCellGetter()

from CaloRec.CaloClusterSWCmbGetter import CaloClusterSWCmbGetter
CaloClusterSWCmbGetter()

include( "CaloRec/CaloTopoCluster_jobOptions.py" )

import AthenaPoolCnvSvc.WriteAthenaPool
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )
from OutputStreamAthenaPool.CreateOutputStreams import  createOutputStream

StreamESD=createOutputStream("StreamESD","myESD.pool.root",True)
include ("CaloRec/CaloRecOutputItemList_jobOptions.py")
StreamESD.ItemList+=CaloESDList

print StreamESD.ItemList


