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
include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_ExtBrl_4.py")

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
DetDescrVersion = 'ATLAS-P2-ITK-06-00-00'

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = globalflags.DetDescrVersion()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += GeoModelSvc

svcMgr.MessageSvc.OutputLevel  = INFO
# increase the number of letter reserved to the alg/tool name from 18 to 30
svcMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages


from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')

# Tracking specifications ####################################
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

TrkDetFlags.SLHC_Geometry = True
TrkDetFlags.MaterialSource = 'None'
TrkDetFlags.MaterialVersion = 20

# material binning for ID
# - longitudinal
TrkDetFlags.BeamPipeLayerMaterialBinsZ              = 35
TrkDetFlags.PixelBarrelLayerMaterialBinsZ           = 100
TrkDetFlags.PixelEndcapLayerMaterialBinsR           = 50
TrkDetFlags.SCT_BarrelLayerMaterialBinsZ            = 100
TrkDetFlags.SCT_EndcapLayerMaterialBinsR            = 50
TrkDetFlags.InDetPassiveLayerMaterialBinsRz         = 100
# - phi
TrkDetFlags.PixelBarrelLayerMaterialBinsPhi         = 1
TrkDetFlags.PixelEndcapLayerMaterialBinsPhi         = 1
TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi          = 1
TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi          = 1
TrkDetFlags.InDetPassiveLayerMaterialBinsPhi        = 1

print TrkDetFlags

doWriteToCool    = True

#ImputPath = 'MaterialStep'

import glob
FileList = glob.glob("./MaterialStepCollection.root")

import AthenaPoolCnvSvc.ReadAthenaPool 
ServiceMgr.EventSelector.InputCollections = FileList

print "FileList = ", FileList

# Number of events to be processed
theApp.EvtMax =-1 # 50000 #5000 #00 #0
#ServiceMgr.EventSelector.SkipEvents = 2

splitGeo = DetDescrVersion.split('-')
MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2] + '-' + splitGeo[3]

############ Configure the output #####################################

if doWriteToCool:
   MaterialStoreGateKey  = TrkDetFlags.MaterialStoreGateKey.get_Value()
   MaterialTag           = TrkDetFlags.MaterialTagBase.get_Value()+str(TrkDetFlags.MaterialVersion.get_Value())+'_'+MaterialMagicTag
    
################################################################
# get the TrackingGeometry Service
TrackingGeometrySvc = None

include("InDetSLHC_Example/SLHC_Setup_ITk_TrackingGeometry.py")
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
BinnedLayerMaterialCreator.OutputLevel               = INFO
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
CompressedLayerMaterialCreator.OutputLevel = INFO
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
CompoundLayerMaterialCreator.OutputLevel = INFO
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
FullCompoundLayerMaterialCreator.OutputLevel             = INFO
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
MaterialMapping.MaterialMapper               = MaterialMapper
MaterialMapping.EtaCutOff                    = 6.
MaterialMapping.OutputLevel                  = INFO
MaterialMapping.LayerMaterialRecordAnalyser  =  LayerMaterialRecordAnalyser
MaterialMapping.LayerMaterialCreators        =  LayerMaterialCreators
MaterialMapping.LayerMaterialAnalysers       =  LayerMaterialAnalysers
# screen output level [ Default : 0 |  0, 1, 2 )
MaterialMapping.MaterialScreenOutputLevel    = 0
topSequence += MaterialMapping

#################################################################
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'

TempOutputFolder = './'
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

include("InDetSLHC_Example/postInclude.SLHC_Setup_ExtBrl_4.py")
