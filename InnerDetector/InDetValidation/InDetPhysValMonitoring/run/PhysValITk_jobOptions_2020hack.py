# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ITk joboptions, hacked to run runs over r20.20 ITk AOD/DAOD inputs
# as 20.20 and 21.9 geometry is incompatible, this requires that we avoid building geometry.
# This is acchieved by a hacked to assume run2 geometry. 
# Only works in case decorators do not run ie AOD or DAOD . Will not run on ESD

#Set to False if running on AOD and True for IDTRKVALDAOD
runDAOD = True

#-----------------------------------------------------------------------
# set up the input file:
# set up for reading pool.root format files (AOD,ESD,IDTRKVALDAOD)
import AthenaPoolCnvSvc.ReadAthenaPool

# read single file: change acc. to your file name here
#FNAME=[ "AOD.pool.root" ]

# uncomment this for testing: step 3, mu=0 file. For a quick run also set theApp.EvtMax to 10 or so
# todo: replace this with cergroup file, once we get some step3 files there 
FNAME=[ "/eos/user/l/lmijovic/atlas/nosyn/upgrade/inputs/step3_shared/step3prod_test_18082018/ttbar/step3/DAOD_IDTRKVALID_digi.pool.root" ]

# uncomment to read multiple files:
#import glob
#FNAME=glob.glob('indir/*pool.root*') 

#
# make AthenaCommonFlags aware of which file we are using
# AthenaCommonFlags are used run-time configuration (InputFilePeeker)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
svcMgr.EventSelector.InputCollections = FNAME
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

# Hack to run over 20.20 files: force GeoModel to ignore tag differences
svcMgr.GeoModelSvc = Service( "GeoModelSvc" )
svcMgr.GeoModelSvc.IgnoreTagDifference = True

#-----------------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']
## Hack to run over 20.20 files: set fake geo to hard-coded run2 value
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"

# Just turn on the detector components we need
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff() 
DetFlags.detdescr.SCT_setOn() 
DetFlags.detdescr.BField_setOn() 
DetFlags.detdescr.pixel_setOn() 

# Set up geometry and BField 
include("RecExCond/AllDet_detDescr.py")

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = ''

import TrkDetDescrSvc.AtlasTrackingGeometrySvc

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="InDetPVMPlotDefITK.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

if not runDAOD : 
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
InDetTrackSelectorTool.minPt            = 400           # Mev
InDetTrackSelectorTool.maxD0            = 1              # mm
InDetTrackSelectorTool.maxZ0            = 150          # mm
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
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AthTruthSelectionTool
TrackTruthSelectionTool = AthTruthSelectionTool()
# todo: manually adapt this acc. to the fiducial acceptance of your detector
TrackTruthSelectionTool.maxEta     = 4.0
TrackTruthSelectionTool.maxPt      = -1
TrackTruthSelectionTool.minPt      = 900 # default 400 MeV
TrackTruthSelectionTool.maxBarcode = int(200e3)
TrackTruthSelectionTool.pdgId      = -1
TrackTruthSelectionTool.requireCharged = True
TrackTruthSelectionTool.requireStatus1 = True
TrackTruthSelectionTool.maxProdVertRadius = 260. #max prod. vertex radius of secondaries [mm]
TrackTruthSelectionTool.OutputLevel = INFO
ToolSvc += TrackTruthSelectionTool

#-------------------------------------------------------------
# Set up InDetPhysValMonitoring tool
#-------------------------------------------------------------
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
InDetPhysValMonitoringTool = InDetPhysValMonitoringTool("InDetPhysValMonitoringTool")
InDetPhysValMonitoringTool.useTrackSelection = True
InDetPhysValMonitoringTool.TrackSelectionTool = InDetTrackSelectorTool
#InDetPhysValMonitoringTool.TruthSelectionTool = TrackTruthSelectionTool
InDetPhysValMonitoringTool.TruthParticleContainerName = "TruthParticles"
#InDetPhysValMonitoringTool.PileupSwitch = "HardScatter"
InDetPhysValMonitoringTool.OutputLevel = INFO
ToolSvc += InDetPhysValMonitoringTool

monMan.AthenaMonTools += [InDetPhysValMonitoringTool]

# set up output file 
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["MyPhysVal DATAFILE='MyPhysVal.root' OPT='RECREATE'"]

# set out message verbosity 
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = WARNING
ServiceMgr.MessageSvc.defaultLimit = 10000

# max. number of events to process
# theApp.EvtMax = -1
theApp.EvtMax = 10


# dump configuration
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
