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
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
DetDescrVersion = 'ATLAS-R1-2012-02-01-00' #ATLAS-GEO-21-02-02' #R1-2012-01-02'

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
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')

# Tracking specifications ####################################
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

TrkDetFlags.MaterialVersion = 17
TrkDetFlags.MaterialSource = 'None'

# material binning for ID
# - longitudinal
TrkDetFlags.BeamPipeLayerMaterialBinsZ              = 35
TrkDetFlags.PixelBarrelLayerMaterialBinsZ           = 100
TrkDetFlags.PixelEndcapLayerMaterialBinsR           = 50
TrkDetFlags.SCT_BarrelLayerMaterialBinsZ            = 100
TrkDetFlags.SCT_EndcapLayerMaterialBinsR            = 50
TrkDetFlags.TRT_BarrelLayerMaterialBinsZ            = 100
TrkDetFlags.TRT_EndcapLayerMaterialBinsR            = 50
TrkDetFlags.InDetPassiveLayerMaterialBinsRz         = 100
# - phi
TrkDetFlags.PixelBarrelLayerMaterialBinsPhi         = 1
TrkDetFlags.PixelEndcapLayerMaterialBinsPhi         = 1
TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi          = 1
TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi          = 1
TrkDetFlags.TRT_BarrelLayerMaterialBinsPhi          = 1
TrkDetFlags.TRT_EndcapLayerMaterialBinsPhi          = 1
TrkDetFlags.InDetPassiveLayerMaterialBinsPhi        = 1

print TrkDetFlags

doWriteToCool    = True

#ImputPath = 'MaterialStep'

import glob
FileList = glob.glob("/afs/cern.ch/user/s/salzburg/atlas/workspace2/atlas_id_material/user.salzburg.GeantinoMapping.p1.ATLAS-R1-2012-10M.140520174400_MaterialStream/*root*") #/tmp/salzburg/*/*.root*")

import AthenaPoolCnvSvc.ReadAthenaPool 
ServiceMgr.EventSelector.InputCollections = FileList

# Number of events to be processed
theApp.EvtMax = 200000  #-1 # 50000 #5000 #00 #0
#ServiceMgr.EventSelector.SkipEvents = 2

splitGeo = DetDescrVersion.split('-')
MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2]

############ Configure the output #####################################

if doWriteToCool:
   MaterialStoreGateKey  = TrkDetFlags.MaterialStoreGateKey.get_Value()
   MaterialTag           = TrkDetFlags.MaterialTagBase.get_Value()+str(TrkDetFlags.MaterialVersion.get_Value())+'_'+MaterialMagicTag
    
################################################################
# get the TrackingGeometry Service
TrackingGeometrySvc = None

from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc.AssignMaterialFromCOOL = False

###############################################################

from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__MaterialMapper
MaterialMapper = Trk__MaterialMapper(name='MaterialMapper')
MaterialMapper.OutputLevel = INFO
ToolSvc += MaterialMapper

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# the magnetic field
from MagFieldServices import SetupField
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil3')

from TrkExEngine.AtlasExtrapolationEngine import AtlasExtrapolationEngine
ExtrapolationEngine = AtlasExtrapolationEngine(name='Extrapolation', nameprefix='Atlas', ToolOutputLevel=INFO)
ToolSvc += ExtrapolationEngine


from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__LayerMaterialAnalyser
LayerMaterialRecordAnalyser =Trk__LayerMaterialAnalyser('LayerMaterialRecordAnalyser')
ToolSvc += LayerMaterialRecordAnalyser


# 0 - Material plain out of the box
# 0 - a) the creator
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__BinnedLayerMaterialCreator
BinnedLayerMaterialCreator = Trk__BinnedLayerMaterialCreator('BinnedLayerMaterialCreator')
BinnedLayerMaterialCreator.LayerMaterialName         = 'BinnedLayerMaterial'
BinnedLayerMaterialCreator.LayerMaterialDirectory    = '/GLOBAL/TrackingGeo/'
BinnedLayerMaterialCreator.OutputLevel               = VERBOSE
ToolSvc += BinnedLayerMaterialCreator
# 0 - b) the analyser
BinnedLayerMaterialAnalyser = Trk__LayerMaterialAnalyser('BinnedLayerMaterialAnalyser')
BinnedLayerMaterialAnalyser.LayerMaterialName         =  BinnedLayerMaterialCreator.LayerMaterialName
BinnedLayerMaterialAnalyser.ValidationTreeName        = 'BinnedLayerMaterialAnalyser'
BinnedLayerMaterialAnalyser.ValidationTreeDescription = 'Output of the BinnedLayerMaterialAnalyser'
BinnedLayerMaterialAnalyser.ValidationTreeFolder      = '/val/BinnedLayerMaterialAnalyser'
ToolSvc += BinnedLayerMaterialAnalyser

# 1 - Compressed Layer material
# 1 - a) the creator
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CompressedLayerMaterialCreator
CompressedLayerMaterialCreator = Trk__CompressedLayerMaterialCreator('CompressedLayerMaterialCreator')
CompressedLayerMaterialCreator.LayerMaterialName         = 'CompressedLayerMaterial'
CompressedLayerMaterialCreator.LayerMaterialDirectory    = '/GLOBAL/TrackingGeo/'
CompressedLayerMaterialCreator.OutputLevel = VERBOSE
ToolSvc += CompressedLayerMaterialCreator

# 1 - b) the analyser
CompressedLayerMaterialAnalyser = Trk__LayerMaterialAnalyser('CompressedLayerMaterialAnalyser')
CompressedLayerMaterialAnalyser.LayerMaterialName         = CompressedLayerMaterialCreator.LayerMaterialName
CompressedLayerMaterialAnalyser.ValidationTreeName        = 'CompressedLayerMaterialAnalyser'
CompressedLayerMaterialAnalyser.ValidationTreeDescription = 'Output of the CompressedLayerMaterialAnalyser'
CompressedLayerMaterialAnalyser.ValidationTreeFolder      = '/val/CompressedLayerMaterialAnalyser'
ToolSvc += CompressedLayerMaterialAnalyser

# 2 - Compound Layer material (mixed)
# 2 - a) the creator
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CompoundLayerMaterialCreator
CompoundLayerMaterialCreator = Trk__CompoundLayerMaterialCreator('CompoundLayerMaterialCreator')
CompoundLayerMaterialCreator.LayerMaterialName           = 'CompoundLayerMaterial'
CompoundLayerMaterialCreator.LayerMaterialDirectory      = '/GLOBAL/TrackingGeo/'
CompoundLayerMaterialCreator.OutputLevel = VERBOSE
ToolSvc += CompoundLayerMaterialCreator
# 2 - b) the analyser
CompoundLayerMaterialAnalyser = Trk__LayerMaterialAnalyser('CompundLayerMaterialAnalyser')
CompoundLayerMaterialAnalyser.LayerMaterialName         = CompoundLayerMaterialCreator.LayerMaterialName
CompoundLayerMaterialAnalyser.ValidationTreeName        = 'CompoundLayerMaterialAnalyser'
CompoundLayerMaterialAnalyser.ValidationTreeDescription = 'Output of the CompoundLayerMaterialAnalyser'
CompoundLayerMaterialAnalyser.ValidationTreeFolder      = '/val/CompoundLayerMaterialAnalyser'
ToolSvc += CompoundLayerMaterialAnalyser

# 2 - Full Compound Layer material (full)
# 2 - a) the creator
FullCompoundLayerMaterialCreator = Trk__CompoundLayerMaterialCreator('FullCompoundLayerMaterialCreator')
FullCompoundLayerMaterialCreator.OutputLevel             = VERBOSE
FullCompoundLayerMaterialCreator.LayerMaterialName       = 'FullCompoundLayerMaterial'
FullCompoundLayerMaterialCreator.LayerMaterialDirectory  = '/GLOBAL/TrackingGeo/'
FullCompoundLayerMaterialCreator.FullCompoundCalculation = True
ToolSvc += FullCompoundLayerMaterialCreator
# 2 - b) the analyser
FullCompoundLayerMaterialAnalyser = Trk__LayerMaterialAnalyser('FullCompundLayerMaterialAnalyser')
FullCompoundLayerMaterialAnalyser.ValidationTreeName        = 'FullCompoundLayerMaterialAnalyser'
FullCompoundLayerMaterialAnalyser.LayerMaterialName         = FullCompoundLayerMaterialCreator.LayerMaterialName
FullCompoundLayerMaterialAnalyser.ValidationTreeDescription = 'Output of the FullCompoundLayerMaterialAnalyser'
FullCompoundLayerMaterialAnalyser.ValidationTreeFolder      = '/val/FullCompoundLayerMaterialAnalyser'
ToolSvc += FullCompoundLayerMaterialAnalyser

# create the lists of MaterialCreators & Analysers
LayerMaterialCreators  = [ BinnedLayerMaterialCreator ] #, CompressedLayerMaterialCreator, CompoundLayerMaterialCreator, FullCompoundLayerMaterialCreator ]
LayerMaterialAnalysers = [ BinnedLayerMaterialAnalyser ] #, CompressedLayerMaterialAnalyser, CompoundLayerMaterialAnalyser, FullCompoundLayerMaterialAnalyser ]


# set up the Material Mapping
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__MaterialMapping
MaterialMapping = Trk__MaterialMapping(name ='MaterialMapping')
MaterialMapping.TrackingGeometrySvc          = AtlasTrackingGeometrySvc
MaterialMapping.MappingVolumeName            = 'InDet::Containers::InnerDetector'
MaterialMapping.ExtrapolationEngine          = ExtrapolationEngine
#MaterialMapping.MappingVolumeName            = 'InDet::BeamPipe'
MaterialMapping.MaterialMapper               = MaterialMapper
MaterialMapping.EtaCutOff                    = 6.
MaterialMapping.OutputLevel                  = DEBUG
MaterialMapping.LayerMaterialRecordAnalyser  =  LayerMaterialRecordAnalyser
MaterialMapping.LayerMaterialCreators        =  LayerMaterialCreators
MaterialMapping.LayerMaterialAnalysers       =  LayerMaterialAnalysers
# screen output level [ Default : 0 |  0, 1, 2 )
MaterialMapping.MaterialScreenOutputLevel    = 2
topSequence += MaterialMapping

#################################################################
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'

TempOutputFolder = '/tmp/salzburg/'
ValidationOutputNameString = TempOutputFolder+'AtlasGeant4Geometry-'

if not hasattr(ServiceMgr, 'THistSvc'):
       from GaudiSvc.GaudiSvcConf import THistSvc
       ServiceMgr += THistSvc()
# add the G4 validation output stream
ServiceMgr.THistSvc.Output += [ "val DATAFILE='"+ValidationOutputNameString+DetDescrVersion+".root' TYPE='ROOT' OPT='RECREATE'" ]

#--------------------------------------------------------------
# Write Stream 
#--------------------------------------------------------------

LocalDataBaseName = "AtlasLayerMaterial-"+DetDescrVersion+'.db'

if doWriteToCool:
    
  # the output conditions alg
  from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   
  ################################################################
  # create the files with the appropriate name
  MaterialFileNameBase = "AtlasLayerMaterial-"+DetDescrVersion  #+'.pool.root'
  MaterialFileName  = MaterialFileNameBase+'.pool.root'

  OutputObjects = []
  IOVTagEntries = []
  for lmCreator in LayerMaterialCreators :
      OutputObjects += [ 'Trk::LayerMaterialMap#'+lmCreator.LayerMaterialDirectory+lmCreator.LayerMaterialName ]
      IOVTagEntries += [ MaterialTag ]
  # create the conditions alg
  myOCA=OutputConditionsAlg("CondAlg_Material",outputFile=MaterialFileName, ObjectList=OutputObjects, WriteIOV=True,IOVTagList=IOVTagEntries)      
  # and create the database connection
  ServiceMgr.IOVDbSvc.dbConnection = 'sqlite://schema='+LocalDataBaseName+';dbname=OFLP200'
