#############################################################
# Run jobOptions for the Algorithm: Material Mapping
#
# Mapping of the Geantino files onto the TrackingGeometry
#
#############################################################
#
# author: Andreas.Salzburger@cern.ch
# Modified for GeoModelXml: Nigel Hessey 
#
#############################################################
#
# date: 13/04/2015
#
#############################################################

import os
if os.path.exists('./PoolFileCatalog.xml') :
   print '[!] PoolFileCatalog exists in the run directory (may use old PFN!)'
   print '[>] Deleting it now !'
   os.remove('./PoolFileCatalog.xml')

execfile ("ID_only.py")
execfile("geometry.py")

# set to monte carlo ---------------------------
import AthenaCommon.AtlasUnixStandardJob    
from AthenaCommon import AthenaCommonFlags
from AthenaCommon.AppMgr import theApp    
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence() 

DetFlags.Calo_setOn()

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry                   = True
# TrkDetFlags.MaterialFromCool                = False
TrkDetFlags.PixelBuildingOutputLevel        = VERBOSE
TrkDetFlags.SCT_BuildingOutputLevel         = VERBOSE
TrkDetFlags.TRT_BuildingOutputLevel         = VERBOSE
TrkDetFlags.MagneticFieldCallbackEnforced   = False
TrkDetFlags.TRT_BuildStrawLayers            = False
TrkDetFlags.MaterialVersion                 = 6
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat'

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

DetVerName =  SLHC_Flags.SLHC_Version()

# no field for this test to avoid problems with the magnetic field callback
TrkDetFlags.MagneticFieldMode            = 0
TrkDetFlags.MagneticFieldSynchronisation = True 

TrkDetFlags.PixelBarrelLayerMaterialBinsZ    = 500
TrkDetFlags.PixelBarrelLayerMaterialBinsPhi  = 1
TrkDetFlags.PixelEndcapLayerMaterialBinsR    = 500
TrkDetFlags.PixelEndcapLayerMaterialBinsPhi  = 1
TrkDetFlags.SCT_BarrelLayerMaterialBinsZ     = 500
TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi   = 1
TrkDetFlags.SCT_EndcapLayerMaterialBinsR     = 500
TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi   = 1
TrkDetFlags.InDetPassiveLayerMaterialBinsRz  = 100
TrkDetFlags.InDetPassiveLayerMaterialBinsPhi = 1
# TrkDetFlags.InDetEntryLayerMaterialBinsRz    = 100
# TrkDetFlags.InDetEntryLayerMaterialBinsPhi   = 1

#TrkDetFlags.LArBarrelLayerMaterialBinsRz      = 100
#TrkDetFlags.LArBarrelLayerMaterialBinsPhi     = 50
#TrkDetFlags.LArEndcapLayerMaterialBinsRz      = 100
#TrkDetFlags.LArEndcapLayerMaterialBinsPhi     = 1
#TrkDetFlags.LArEndcapHecLayerMaterialBinsRz   = 100
#TrkDetFlags.LArEndcapHecLayerMaterialBinsPhi  = 1
#TrkDetFlags.LArEndcapFcalLayerMaterialBinsRz  = 100
#TrkDetFlags.LArEndcapFcalLayerMaterialBinsPhi = 1
#TrkDetFlags.LArEndcapGapLayerMaterialBinsRz   = 50
#TrkDetFlags.LArEndcapGapLayerMaterialBinsPhi  = 50
#TrkDetFlags.TileBarrelLayerMaterialBinsRz          = 50
#TrkDetFlags.TileBarrelLayerMaterialBinsPhi         = 50
#TrkDetFlags.TileGirderLayerMaterialBinsRz          = 50
#TrkDetFlags.TileGirderLayerMaterialBinsPhi         = 1
#TrkDetFlags.TileFingerLayerMaterialBinsRz          = 50
#TrkDetFlags.TileFingerLayerMaterialBinsPhi         = 1 
#TrkDetFlags.TileExtendedBarrelLayerMaterialBinsRz  = 50
#TrkDetFlags.TileExtendedBarrelLayerMaterialBinsPhi = 50
#TrkDetFlags.TileGapLayerMaterialBinsRz             = 50
#TrkDetFlags.TileGapLayerMaterialBinsPhi            = 50
#TrkDetFlags.CaloLayerMaterialBinsRz                = 50
#TrkDetFlags.CaloLayerMaterialBinsPhi               = 1

doWriteToCool    = True

from AthenaCommon.AppMgr import ServiceMgr

import AthenaPoolCnvSvc.ReadAthenaPool 

ServiceMgr.EventSelector.InputCollections += ['MaterStepFile.root' ]
#ServiceMgr.EventSelector.InputCollections += ['GeantinoMapping-'+jobproperties.Global.DetDescrVersion()+'-v3.root' ]
#ServiceMgr.EventSelector.InputCollections += [ CastorRootPath+'GeantinoMapping-'+SLHC_Flags.SLHC_Version()+'-v3.root' ]

# Number of events to be processed
theApp.EvtMax = 1000

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = INFO
MessageSvc.defaultLimit     = 500
MessageSvc.Format           = "% F%37W%S%7W%R%T %0W%M"

################################################################
# create the files with the appropriate name
MaterialFileName = "SLHC_LayerMaterial-"+DetVerName+'.pool.root'
LocalDataBaseName = "SLHC_LayerMaterial-"+DetVerName+'.db'

################################################################
# the bfield 
try:
   import MagFieldServices.SetupField
except:
   # fall back
   include( 'BFieldAth/BFieldAth_jobOptions.py')

TrackingGeometrySvc = None
include ( 'TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
AtlasTrackingGeometrySvc.AssignMaterialFromCOOL = False
TrackingGeometrySvc = AtlasTrackingGeometrySvc
  
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__MaterialMapper
TheMaterialMapper = Trk__MaterialMapper(name='MaterialMapper')
TheMaterialMapper.OutputLevel = INFO
ToolSvc += TheMaterialMapper

# set up the Geometry Builder test
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__MaterialMapping
MaterialMapping = Trk__MaterialMapping(name ='MaterialMapping')
MaterialMapping.TrackingGeometrySvc      = TrackingGeometrySvc
MaterialMapping.MaterialMapper           = TheMaterialMapper
#MaterialMapping.MaterialInspector        = TheLayerMaterialInspector
MaterialMapping.MaximumMappingEvents     = 50000
MaterialMapping.OutputLevel              = INFO

############ Configure the output #####################################

if doWriteToCool:
  MaterialStoreGateKey  = TrkDetFlags.MaterialStoreGateKey.get_Value()
  MaterialTag           = TrkDetFlags.MaterialTagBase.get_Value()+'_v'+str(TrkDetFlags.MaterialVersion.get_Value())+'_'+DetVerName

# assign the material
MaterialMapping.OutputLayerMaterialSetName = TrkDetFlags.MaterialStoreGateKey.get_Value()

topSequence += MaterialMapping
  
  
#################################################################
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'

ValidationOutputNamePath = ''
ValidationOutputNameString = 'SLHC_MaterialG4-'

from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
       from GaudiSvc.GaudiSvcConf import THistSvc
       ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "val DATAFILE='"+ValidationOutputNamePath+ValidationOutputNameString+DetVerName+".root' TYPE='ROOT' OPT='RECREATE'" ]
      
#--------------------------------------------------------------
# Write Stream 
#--------------------------------------------------------------

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("MaterialStepOutputCondAlg",outputFile=MaterialFileName,
                          ObjectList=[ 'LayerMaterialPropertiesSet#'+MaterialStoreGateKey ],
                          WriteIOV=True,IOVTagList=[MaterialTag])
                          
ServiceMgr.IOVDbSvc.dbConnection = 'sqlite://schema='+LocalDataBaseName+';dbname=OFLP200'
