# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: PhysVal_jobOptions.py 714189 2015-12-11 17:33:02Z sroe $

# Set up the reading of the input xAOD:

#"AOD.05522648._000044.pool.root.1" K-short dataset
#"ESD.05108991._000060.pool.root.1" original ttbar dataset 
#"ESD.05297574._000081.pool.root.1" new ttbar dataset (this one should enable residuals)
#import getpass
#FNAME="root://eosatlas//eos/atlas/atlascerngroupdisk/det-slhc/users/oda/xAODexample/mc12_14TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.e1323_ATLAS-P2-ITK-01-00-00_19.2.3.1_20.1.3.2.pool.root"

FNAME="AOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [ FNAME ] 
#include( "AthenaPython/iread_file.py" )

# Set global flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']

xmlTags = [ ["ATLAS-P2-ITK-05","ExtBrl_32"],
            ["ATLAS-P2-ITK-06","ExtBrl_4"],
            ["ATLAS-P2-ITK-07","IExtBrl_4"],
            ["ATLAS-P2-ITK-08","InclBrl_4"] ]

for geoTag, layoutDescr in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      print "preIncludes for ",layoutDescr, " layout"
      from InDetRecExample.InDetJobProperties import InDetFlags
      include('InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py')
      include('InDetSLHC_Example/preInclude.SLHC_Setup_'+layoutDescr+'.py')
      include('InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py')
      break

# Just turn on the detector components we need
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff() 
DetFlags.detdescr.SCT_setOn() 
DetFlags.detdescr.BField_setOn() 
DetFlags.detdescr.pixel_setOn() 

# Set up geometry and BField 
include("RecExCond/AllDet_detDescr.py")

# Set up tracking geometry
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = ''

         
from AthenaCommon.GlobalFlags import jobproperties
DetDescrVersion = jobproperties.Global.DetDescrVersion()

# Set up tracking geometry
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialStoreGateKey="/GLOBAL/TrackingGeo/LayerMaterialITK"
TrkDetFlags.MaterialMagicTag = DetDescrVersion
TrkDetFlags.MaterialVersion=17
TrkDetFlags.MaterialSubVersion=""
TrkDetFlags.SLHC_Geometry=True

# Set to True if you use a local geometry file
TrkDetFlags.MaterialDatabaseLocal=False

if TrkDetFlags.MaterialDatabaseLocal():
   TrkDetFlags.MaterialDatabaseLocalPath="./"
   TrkDetFlags.MaterialDatabaseLocalName="AtlasLayerMaterial-"+DetDescrVersion+".db"

for geoTag, layoutDescr in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      print "postInclude for ",layoutDescr, " layout"
      include('InDetSLHC_Example/postInclude.SLHC_Setup_'+layoutDescr+'.py')
      break

import TrkDetDescrSvc.AtlasTrackingGeometrySvc

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
#ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/inDetPhysValMonitoringPlotDefinitions.hdef"
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/ITKHistDef.hdef"


from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg
decorators = InDetPhysValDecoratorAlg()
topSequence += decorators


from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "MyPhysVal"
topSequence += monMan

#-------------------------------------------------------------
# Set up track selection tool
#-------------------------------------------------------------
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool = InDet__InDetTrackSelectionTool("InDetTrackSelectorTool")
InDetTrackSelectorTool.minPt            = 900           # Mev
InDetTrackSelectorTool.maxD0            = 1             # mm
InDetTrackSelectorTool.maxZ0            = 150           # mm
InDetTrackSelectorTool.minNSiHits       = 11            # Pixel + SCT
InDetTrackSelectorTool.maxNPixelHoles   = 2             # Pixel only
#eta dependant hit cut below
#InDetTrackSelectorTool.vecEtaCutoffsForSiHitsCut = [0,1.0,1.2,1.8,2.2]
#InDetTrackSelectorTool.vecMinNSiHitsAboveEta = [11,11,11,13,10]
ToolSvc += InDetTrackSelectorTool
print "Set Up InDetTrackSelectorTool"

#-------------------------------------------------------------
# Set up truth selection tool
#-------------------------------------------------------------
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import TrackTruthSelectionTool
TrackTruthSelectionTool = TrackTruthSelectionTool("TrackTruthSelectionTool")
TrackTruthSelectionTool.maxEta     = 5.0
TrackTruthSelectionTool.maxPt      = -1
TrackTruthSelectionTool.minPt      = 900 # default 400 MeV
TrackTruthSelectionTool.maxBarcode = 200e3
TrackTruthSelectionTool.pdgId      = -1
TrackTruthSelectionTool.requireCharged = True
TrackTruthSelectionTool.requireStatus1 = True
TrackTruthSelectionTool.requireDecayBeforePixel = True
TrackTruthSelectionTool.OutputLevel = DEBUG
ToolSvc += TrackTruthSelectionTool

#-------------------------------------------------------------
# Set up InDetPhysValMonitoring tool
#-------------------------------------------------------------
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
InDetPhysValMonitoringTool = InDetPhysValMonitoringTool("InDetPhysValMonitoringTool")
##rdh InDetPhysValMonitoringTool.DirName = "InDetPhysValMon_inclusive/"
InDetPhysValMonitoringTool.useTrackSelection = True
InDetPhysValMonitoringTool.TrackSelectionTool = InDetTrackSelectorTool
InDetPhysValMonitoringTool.TruthSelectionTool = TrackTruthSelectionTool
InDetPhysValMonitoringTool.TruthParticleContainerName = "TruthParticles"
#InDetPhysValMonitoringTool.PileupSwitch = "HardScatter"
InDetPhysValMonitoringTool.OutputLevel = DEBUG
ToolSvc += InDetPhysValMonitoringTool

monMan.AthenaMonTools += [InDetPhysValMonitoringTool]


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["MyPhysVal DATAFILE='MyPhysVal.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = 50

# dump configuration
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
