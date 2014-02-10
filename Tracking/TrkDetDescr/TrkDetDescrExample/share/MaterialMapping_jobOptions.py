# necessity to create a new PoolFileCatalog 
import os
if os.path.exists('./PoolFileCatalog.xml') :
   print '[!] PoolFileCatalog exists in the run directory (may use old PFN!)'
   print '[>] Deleting it now !'
   os.remove('./PoolFileCatalog.xml')

###############################################################
#
# Job options 
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# the global detflags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
DetDescrVersion = 'ATLAS-GEO-20-00-00'

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = globalflags.DetDescrVersion()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += GeoModelSvc

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SIM-00-00-00")

# Tracking specifications ####################################
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

TrkDetFlags.MaterialVersion = 15

# material binning for ID
TrkDetFlags.BeamPipeLayerMaterialBinsZ              = 25
TrkDetFlags.PixelBarrelLayerMaterialBinsZ           = 100
TrkDetFlags.PixelBarrelLayerMaterialBinsPhi         = 1
TrkDetFlags.PixelEndcapLayerMaterialBinsR           = 100
TrkDetFlags.PixelEndcapLayerMaterialBinsPhi         = 1
TrkDetFlags.SCT_BarrelLayerMaterialBinsZ            = 100
TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi          = 1
TrkDetFlags.SCT_EndcapLayerMaterialBinsR            = 100
TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi          = 1
TrkDetFlags.TRT_BarrelLayerMaterialBinsZ            = 50
TrkDetFlags.TRT_BarrelLayerMaterialBinsPhi          = 1
TrkDetFlags.TRT_EndcapLayerMaterialBinsR            = 50
TrkDetFlags.TRT_EndcapLayerMaterialBinsPhi          = 1
TrkDetFlags.InDetPassiveLayerMaterialBinsRz         = 100
TrkDetFlags.InDetPassiveLayerMaterialBinsPhi        = 1
TrkDetFlags.InDetEntryLayerMaterialBinsRz           = 100
TrkDetFlags.InDetEntryLayerMaterialBinsPhi          = 1

# material binning for Calo
TrkDetFlags.LArBarrelLayerMaterialBinsRz            = 100
TrkDetFlags.LArBarrelLayerMaterialBinsPhi           = 50
TrkDetFlags.LArEndcapLayerMaterialBinsRz            = 100
TrkDetFlags.LArEndcapLayerMaterialBinsPhi           = 1
TrkDetFlags.LArEndcapHecLayerMaterialBinsRz         = 100
TrkDetFlags.LArEndcapHecLayerMaterialBinsPhi        = 1
TrkDetFlags.LArEndcapFcalLayerMaterialBinsRz        = 100
TrkDetFlags.LArEndcapFcalLayerMaterialBinsPhi       = 1
TrkDetFlags.LArEndcapGapLayerMaterialBinsRz         = 50
TrkDetFlags.LArEndcapGapLayerMaterialBinsPhi        = 50
TrkDetFlags.TileBarrelLayerMaterialBinsRz           = 50
TrkDetFlags.TileBarrelLayerMaterialBinsPhi          = 50
TrkDetFlags.TileGirderLayerMaterialBinsRz           = 50
TrkDetFlags.TileGirderLayerMaterialBinsPhi          = 1
TrkDetFlags.TileFingerLayerMaterialBinsRz           = 50
TrkDetFlags.TileFingerLayerMaterialBinsPhi          = 1 
TrkDetFlags.TileExtendedBarrelLayerMaterialBinsRz   = 50
TrkDetFlags.TileExtendedBarrelLayerMaterialBinsPhi  = 50
TrkDetFlags.TileGapLayerMaterialBinsRz              = 50
TrkDetFlags.TileGapLayerMaterialBinsPhi             = 50
TrkDetFlags.CaloLayerMaterialBinsRz                 = 50
TrkDetFlags.CaloLayerMaterialBinsPhi                = 1

print TrkDetFlags


doWriteToCool    = True

ImputPath = '/afs/cern.ch/user/s/salzburg/atlas/workspace1/work/material/user.salzburg.GeantinoMapping.TPC.ATLAS-GEO-20-00-00.1M.130911175333_MaterialStream/*'

import glob
FileList = glob.glob(ImputPath+"*root*")

import AthenaPoolCnvSvc.ReadAthenaPool 
ServiceMgr.EventSelector.InputCollections = FileList

# Number of events to be processed
theApp.EvtMax = 500000 #0
#ServiceMgr.EventSelector.SkipEvents = 2

splitGeo = DetDescrVersion.split('-')
MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2]

############ Configure the output #####################################

if doWriteToCool:
  MaterialStoreGateKey  = TrkDetFlags.MaterialStoreGateKey.get_Value()
  MaterialTag           = TrkDetFlags.MaterialTagBase.get_Value()+str(TrkDetFlags.MaterialVersion.get_Value())+'_'+MaterialMagicTag
    
################################################################
# create the files with the appropriate name
MaterialFileName = "AtlasLayerMaterial-"+DetDescrVersion+'.pool.root'
LocalDataBaseName = "AtlasLayerMaterial-"+DetDescrVersion+'.db'

################################################################
# get the TrackingGeometry Service
TrackingGeometrySvc = None

from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc.AssignMaterialFromCOOL = False

###############################################################

from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__MaterialMapper
MaterialMapper = Trk__MaterialMapper(name='MaterialMapper')
MaterialMapper.ValidationMode = True
MaterialMapper.OutputLevel = INFO
ToolSvc += MaterialMapper

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# set up the Material Mapping
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__MaterialMapping
MaterialMapping = Trk__MaterialMapping(name ='MaterialMapping')
MaterialMapping.TrackingGeometrySvc       = AtlasTrackingGeometrySvc
MaterialMapping.MappingVolumeName         = 'InDet::Containers::InnerDetector'
#MaterialMapping.MappingVolumeName         = 'InDet::BeamPipe'
MaterialMapping.MaterialMapper            = MaterialMapper
MaterialMapping.CompressMaterial          = False        
MaterialMapping.CompressMaterialBinsX0    = 50  
MaterialMapping.CompressMaterialBinsZARho = 25
MaterialMapping.MaximumMappingEvents      = -1
MaterialMapping.EtaCutOff                 = 6.
MaterialMapping.OutputLevel               = INFO
MaterialMapping.MaterialAssociationType   = 1
# for validation only [ Default : -1 ]
MaterialMapping.MaximumMappedEvents       = -1
# screen output level [ Default : 0 |  0, 1, 2 )
MaterialMapping.MaterialScreenOutputLevel = 0
# assign the material
MaterialMapping.OutputLayerMaterialSetName = TrkDetFlags.MaterialStoreGateKey.get_Value()
topSequence += MaterialMapping

#################################################################
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'

TempOutputFolder = '' #'/tmp/wlukas/'
ValidationOutputNameString = TempOutputFolder+'AtlasMaterialG4-'

if not hasattr(ServiceMgr, 'THistSvc'):
       from GaudiSvc.GaudiSvcConf import THistSvc
       ServiceMgr += THistSvc()
# add the G4 validation output stream
ServiceMgr.THistSvc.Output += [ "val DATAFILE='"+ValidationOutputNameString+DetDescrVersion+".root' TYPE='ROOT' OPT='RECREATE'" ]

#--------------------------------------------------------------
# Write Stream 
#--------------------------------------------------------------

if doWriteToCool :
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   myOCA=OutputConditionsAlg("MaterialStepOutputCondAlg",outputFile=MaterialFileName,
                             ObjectList=[ 'Trk::LayerMaterialMap#'+MaterialStoreGateKey ],
                             WriteIOV=True,IOVTagList=[MaterialTag])
                             
   
   ServiceMgr.IOVDbSvc.dbConnection = 'sqlite://schema='+TempOutputFolder+LocalDataBaseName+';dbname=OFLP200'

