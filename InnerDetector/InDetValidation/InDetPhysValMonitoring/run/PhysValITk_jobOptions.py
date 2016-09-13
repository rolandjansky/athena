# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: PhysVal_jobOptions.py 714189 2015-12-11 17:33:02Z sroe $

# Set up the reading of the input xAOD:
import AthenaPoolCnvSvc.ReadAthenaPool
# read single file
#FNAME="ESD.pool.root"
# uncomment this for testing: step 1.2, mu=200 file:
#FNAME="root://eosatlas//eos/atlas/user/l/lmijovic/shared_atlas/upgrade/inputs/mc15_14TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.recon.AOD.e1133_s2900_s2904_r7914/AOD.08762005._000011.pool.root.1"
#FNAME="/afs/cern.ch/work/j/jwang/private/ATLAS-Upgrade/StripSim/TryLongShortStrips-Step15a/WorkArea/run/SLHCtest.ExtBrl4_GMX.AOD.pool.root"
FNAME="/afs/cern.ch/work/j/jwang/public/ToShaun/SLHCtest.ExtBrl4_GMX.AOD.pool.root"

svcMgr.EventSelector.InputCollections = [ FNAME ] 

#eosDir = "/eos/atlas/user/n/norap/ITK/DAOD_IDTRKVALD/singleElectron/"
#fileType = "mc15_14TeV.422024.ParticleGun_single_ele_PtFlat_0p2_2000.recon.DAOD_IDTRKVALID.e5074_s2897_t604_r8069/"
#from InDetBeamSpotExample import DiskUtils
#inFileList = DiskUtils.filelist( eosDir+fileType, "root://eosatlas.cern.ch/")
#athenaCommonFlags.FilesInput = inFileList


# read multiple files:
#import glob
#INFILES=glob.glob('indir/*pool.root*')
#svcMgr.EventSelector.InputCollections = INFILES

# Set global flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']

xmlTags = [ ["ATLAS-P2-ITK-05","ExtBrl_32",""],
            ["ATLAS-P2-SFCAL-01-05-01","ExtBrl_32","GMX"],
            ["ATLAS-P2-ITK-06","ExtBrl_4",""],
            ["ATLAS-P2-ITK-09-00-00","ExtBrl_4","GMX"],
            ["ATLAS-P2-ITK-07","IExtBrl_4",""],
            ["ATLAS-P2-SFCAL-01-07-01","IExtBrl_4","GMX"],
            ["ATLAS-P2-ITK-08","InclBrl_4",""],
            ["ATLAS-P2-SFCAL-01-08-01","InclBrl_4","GMX"],]

for geoTag, layoutDescr, gmx in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      print "preIncludes for ",layoutDescr, " layout"
      from InDetRecExample.InDetJobProperties import InDetFlags
      include('InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py')
      include('InDetSLHC_Example/preInclude.SLHC_Setup_'+layoutDescr+'.py')
      if gmx=="GMX":
         print "preIncludes for GMX strip layout"
         include('InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py')
         include('InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry.py')
      else:
         include('InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry.py')
      break


include('InDetSLHC_Example/preInclude.SLHC_Setup_ExtBrl_4.py')
include('InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py')
include('InDetSLHC_Example/preInclude.SiliconOnly.py')


globalflags.ConditionsTag = 'OFLCOND-MC15c-SDR-12'

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialMagicTag = 'ATLAS-P2-ITK-09-00-00' 
TrkDetFlags.MaterialStoreGateKey = '/GLOBAL/TrackingGeo/LayerMaterialITK'
TrkDetFlags.MaterialVersion = 17 
TrkDetFlags.SLHC_Geometry = True 



# Just turn on the detector components we need
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff() 
DetFlags.detdescr.SCT_setOn() 
DetFlags.detdescr.BField_setOn() 
DetFlags.detdescr.pixel_setOn() 

# Set up geometry and BField 
include("RecExCond/AllDet_detDescr.py")

for geoTag, layoutDescr,gmx in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      print "postInclude for ",layoutDescr, " layout"
      #include('InDetSLHC_Example/postInclude.SLHC_Setup_'+layoutDescr+'.py')
      break

import TrkDetDescrSvc.AtlasTrackingGeometrySvc

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/ITKHistDef.xml"
# ../share/InDetPhysValMonitoringPlotDefinitions.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

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
InDetTrackSelectorTool.minNSiHits       = 9            # Pixel + SCT
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
# todo: manually adapt this acc. to the fiducial acceptance of your detector
TrackTruthSelectionTool.maxEta     = 4.0
TrackTruthSelectionTool.maxPt      = -1
TrackTruthSelectionTool.minPt      = 900 # default 400 MeV
TrackTruthSelectionTool.maxBarcode = 200e3
TrackTruthSelectionTool.pdgId      = -1
TrackTruthSelectionTool.requireCharged = True
TrackTruthSelectionTool.requireStatus1 = True
TrackTruthSelectionTool.maxProdVertRadius = 260. # max prod. vertex radius for secondaries [mm]
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
theApp.EvtMax = 2


from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialMagicTag = 'ATLAS-P2-ITK-09-00-00'
TrkDetFlags.MaterialStoreGateKey = '/GLOBAL/TrackingGeo/LayerMaterialITK'
TrkDetFlags.MaterialVersion = 17
TrkDetFlags.SLHC_Geometry = True

print "TrkDetFlags: ",  TrkDetFlags 


include('InDetSLHC_Example/postInclude.SLHC_Setup_ExtBrl_4.py')


# dump configuration
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
