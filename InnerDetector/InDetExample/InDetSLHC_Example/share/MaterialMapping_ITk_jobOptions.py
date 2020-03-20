# This job options creates database files of Geantino/MaterialMapping.
# Mofifled version of Tracking/TrkDetDescr/TrkDetDescrExample/share/MaterialMapping_jobOptions.py. (TrkDetDescrExample-01-00-04)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/UpgradeSimulationInnerTrackerMigrationRel20p3p3#Creation_of_database_files

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
DetFlags.BCM_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
if 'myGeo' not in dir():
  DetDescrVersion = 'ATLAS-P2-ITK-20-03-00'
  print "=== 'myGeo' not specified, defaulting to",DetDescrVersion," geometry! ==="
else: DetDescrVersion = myGeo

if 'myInDir' not in dir():
  myInDir = "." 
  print "=== 'myInDir' not specified, defaulting to",myInDir," directory for input files! ==="


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion

## Need to set the layout option up front
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

if globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-20'):
   SLHC_Flags.LayoutOption="InclinedDuals"

elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-19'):
   SLHC_Flags.LayoutOption="InclinedQuads"

elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-17'):
   SLHC_Flags.LayoutOption="InclinedAlternative"

elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22-00') or \
     globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22-01'):
   SLHC_Flags.LayoutOption="InclinedAlternative"

SLHC_Flags.doGMX.set_Value_and_Lock(True)
include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")

if globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-20') or \
   globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-19') or \
   globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-17') or \
   globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22-00') or \
   globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22-01'):
   include("InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py")
else:
   include("InDetSLHC_Example/preInclude.SLHC_Setup.py")

include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")


from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = globalflags.DetDescrVersion()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += GeoModelSvc

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-MC15c-SDR-14-03")

# Tracking specifications ####################################
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags


TrkDetFlags.SLHC_Geometry = True
TrkDetFlags.MaterialSource = 'None'
# This will end up in the database tag that will be created! 20 is looked for by default in tracking 
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

import glob
FileList = glob.glob(myInDir+"/MaterialStepCollection*.root*")

import AthenaPoolCnvSvc.ReadAthenaPool 
ServiceMgr.EventSelector.InputCollections = FileList
if not hasattr(svcMgr, theApp.EventLoop): 
     svcMgr += getattr(CfgMgr, theApp.EventLoop)() 
evtloop = getattr(svcMgr, theApp.EventLoop) 
try: 
  evtloop.EventPrintoutInterval = 50000 
except Exception, err: 
  msg.info('failed suppressing event loop heartbeat. performances might be sub-par... sorry.') 
  pass 

print "FileList = ", FileList

# Number of events to be processed
theApp.EvtMax = -1 

MaterialMagicTag = DetDescrVersion

############ Configure the output #####################################
# set to False if you just want to have the ntuple, but no local DB
doWriteToCool    = True
# this determines what will be written to the DB
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


from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__BinnedLayerMaterialCreator
BinnedLayerMaterialCreator = Trk__BinnedLayerMaterialCreator('BinnedLayerMaterialCreator')
# NOTE that the following determines the DB folder we will be writing to. 
# the reco will look for maps in /GLOBAL/TrackingGeo/LayerMaterialITK by default  
BinnedLayerMaterialCreator.LayerMaterialName         = 'LayerMaterialITK'    
BinnedLayerMaterialCreator.LayerMaterialDirectory    = '/GLOBAL/TrackingGeo/'
BinnedLayerMaterialCreator.OutputLevel = INFO
ToolSvc += BinnedLayerMaterialCreator
# 0 - b) the analyser
BinnedLayerMaterialAnalyser = Trk__LayerMaterialAnalyser('BinnedLayerMaterialAnalyser')
BinnedLayerMaterialAnalyser.LayerMaterialName         =  BinnedLayerMaterialCreator.LayerMaterialName
BinnedLayerMaterialAnalyser.ValidationTreeName        = 'BinnedLayerMaterialAnalyser'
BinnedLayerMaterialAnalyser.ValidationTreeDescription = 'Output of the BinnedLayerMaterialAnalyser'
BinnedLayerMaterialAnalyser.ValidationTreeFolder      = '/val/BinnedLayerMaterialAnalyser'
ToolSvc += BinnedLayerMaterialAnalyser


# create the lists of MaterialCreators & Analysers
LayerMaterialCreators  = [ BinnedLayerMaterialCreator ] 
LayerMaterialAnalysers = [ BinnedLayerMaterialAnalyser ] 

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
job += MaterialMapping

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

if globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-20') or \
   globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-19') or \
   globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-17') or \
   globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22-00') or \
   globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22-01'):
   include("InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py")
else:
   include("InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py")
