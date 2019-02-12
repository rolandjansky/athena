# $Id: PhysVal_jobOptions.py 714189 2015-12-11 17:33:02Z sroe $

#-----------------------------------------------------------------------
# set up the input file:
# set up for reading pool.root format files (AOD,ESD,IDTRKVALDAOD)
import AthenaPoolCnvSvc.ReadAthenaPool

# read single file: warious options
#.......................................................................
# FNAME=[ "AOD.pool.root" ]
# runDAOD = False # set to False for AOD/ESD and True for IDTRKVALDAOD
#.......................................................................
# FNAME=[ "ESD.pool.root" ]
# runDAOD = False # set to False for AOD/ESD and True for IDTRKVALDAOD
#.......................................................................
# FNAME=[ "IDTRKVALDAOD.pool.root" ]
# runDAOD = True # set to False for AOD/ESD and True for IDTRKVALDAOD
#.......................................................................
# uncomment this for testing: step 1.9, mu=0 single muon with pt=100GeV samples:
#FNAME=[ "root://eosatlas.cern.ch//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_14TeV/bb/50/DAOD_IDTRKVALID.11169418._000001.pool.root.1",
#        "root://eosatlas.cern.ch//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_14TeV/1e/72/DAOD_IDTRKVALID.11169418._000002.pool.root.1" ]
#.......................................................................
# uncomment for step 2.2, mu=0 single muon pt=100GeV:
#FNAME=[ "root://eosatlas.cern.ch///eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_14TeV/b3/4c/DAOD_IDTRKVALID.11973958._000201.pool.root.1" ]

runDAOD = False # these are AOD or ESD
try:
   FNAME = os.environ["inputESDFile"]
except KeyError:
   try:
      FNAME = os.environ["inputDAOD_IDTRKVALIDFile"]
      runDAOD = True # these are DAOD-s
   except KeyError:
      try:
         FNAME = os.environ["inputAODFile"]
      except KeyError:
         sys.exit("No input file specified in environment - exit")

FNAME = FNAME.split(",")

#.......................................................................
# uncomment to read multiple files:
#import glob
#FNAME=glob.glob('indir/*pool.root*')
#runDAOD = True # set to False for AOD/ESD and True for IDTRKVALDAODs
#.......................................................................
# uncomment this to process samples as in ascii (1 sample / line):
# with open('samples.txt','r') as inSamp:
#   FNAME = [line.rstrip('\n') for line in inSamp]
# runDAOD = True # set to False for AOD/ESD and True for IDTRKVALDAODs
#.......................................................................
#-----------------------------------------------------------------------

# make AthenaCommonFlags aware of which file we are using
# AthenaCommonFlags are used run-time configuration (InputFilePeeker)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
svcMgr.EventSelector.InputCollections = FNAME
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

#-----------------------------------------------------------------------
# pre- and post- includes files needed to read input files
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']

# if you are running with a geometry not yet included in xmlTags:
# add your Geometry and Layout below.
# than x-check with:
# GetTfCommand.py --AMI your-recodigi-tag 
# that pre- and post- includes we schedule are the right ones
xmlTags = [ ["ATLAS-P2-SFCAL-01-05-01","ExtBrl_32",""],
            ["ATLAS-P2-SFCAL-01-06-01","ExtBrl_4",""],
            ["ATLAS-P2-SFCAL-01-07-01","IExtBrl_4",""],
            ["ATLAS-P2-SFCAL-01-08-01","InclBrl_4",""],
            ["ATLAS-P2-ITK-10-00-00","InclBrl_4",""],
            ["ATLAS-P2-ITK-09-00-00","ExtBrl_4",""],
            ["ATLAS-P2-ITK-10-00-01","InclBrl_4",""],
            ["ATLAS-P2-ITK-09-00-01","ExtBrl_4",""],
            ["ATLAS-P2-ITK-11-00-01","ExtBrl_4_33mm",""],
            ["ATLAS-P2-ITK-12-00-01","IExtBrl_4",""],
            # step 1.6 with BCL
            ["ATLAS-P2-ITK-10-04-01","InclBrl_4",""],
            # step 1.6 with all long strips 
            ["ATLAS-P2-ITK-10-03-01","InclBrl_4",""],
            # step 1.7
            ["ATLAS-P2-ITK-09-00-03","ExtBrl_4",""],
            ["ATLAS-P2-ITK-10-00-03","InclBrl_4",""],
            # step 1.8 and step 1.9, nominal (run4) beam-pipe
            ["ATLAS-P2-ITK-13-00-00","InclBrl_4_OptRing",""],
            # step 1.9, run2 beam-pipe
            ["ATLAS-P2-ITK-13-00-02","InclBrl_4_OptRing",""],
            # step 2
            ["ATLAS-P2-ITK-13-00-02","InclBrl_4_OptRing",""],
            ["ATLAS-P2-ITK-19-00-00","InclBrl_4","InclinedQuads"],
            ["ATLAS-P2-ITK-20-00-00","InclBrl_4","InclinedDuals"],
            # step 3
            ["ATLAS-P2-ITK-17-00-00","InclBrl_4","InclinedAlternative"]
            ]

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

for geoTag, layoutDescr, layoutOption in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      print "preIncludes for ",layoutDescr, " layout"
      if (layoutOption!=""): 
         SLHC_Flags.LayoutOption=layoutOption
      from InDetRecExample.InDetJobProperties import InDetFlags
      include('InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py')
      include('InDetSLHC_Example/preInclude.SLHC_Setup_'+layoutDescr+'.py')
      include('InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py')
      if geoTag=="ATLAS-P2-ITK-10-00-00" or geoTag=="ATLAS-P2-ITK-09-00-00" :
         include('InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry.py')
      else:
         include('InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py')      
      break
   
# Just turn on the detector components we need
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff() 
DetFlags.detdescr.SCT_setOn() 
DetFlags.detdescr.BField_setOn() 
DetFlags.detdescr.pixel_setOn() 

# Set up geometry and BField 
include("RecExCond/AllDet_detDescr.py")

SLHC_Flags.SLHC_Version = ''

         
from AthenaCommon.GlobalFlags import jobproperties
DetDescrVersion = jobproperties.Global.DetDescrVersion()

for geoTag, layoutDescr, layoutOption in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      print "postInclude for ",layoutDescr, " layout"
      include('InDetSLHC_Example/postInclude.SLHC_Setup_'+layoutDescr+'.py')
      break

#-----------------------------------------------------------------------

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# include xml file with histogram definitions 
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/InDetPVMPlotDefITK.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

# this fills some extra histograms when not running over DAOD
# when running over DAOD, decorators should be off to prevent crashes
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
monMan.FileKey = "physval"
topSequence += monMan

#-------------------------------------------------------------
# Set up track selection tool
#-------------------------------------------------------------
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool = InDet__InDetTrackSelectionTool("InDetTrackSelectorTool")
##### DO NOT USE THESE !!!!! !!!! !!! ################
##### DO NOT COMMENT THEM OUT !!!!! ##################
InDetTrackSelectorTool.minPt            = 400           # Mev
InDetTrackSelectorTool.maxD0            = 2              # mm
InDetTrackSelectorTool.maxZ0            = 250          # mm
InDetTrackSelectorTool.minNSiHits       = 6            # Pixel + SCT
InDetTrackSelectorTool.maxNPixelHoles   = 2             # Pixel only
InDetTrackSelectorTool.OutputLevel = INFO
#eta dependant hit cut below
#InDetTrackSelectorTool.vecEtaCutoffsForSiHitsCut = [0,1.0,1.2,1.8,2.2]
#InDetTrackSelectorTool.vecMinNSiHitsAboveEta = [11,11,11,13,10]

##### Temporary cuts - Will be fixed in 20.20.8.X ######
############################################ USE THESE ONES!!!! #################################
######### IF THE CUT YOU NEED IS NOT DEFINED BELOW, ADD IT TO InDetTrackSelectorTool. ABOVE #####
#from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrackCutSvc
#InDetTrackCutSvcIDPVM = InDet__InDetTrackCutSvc("InDetTrackCutSvcIDPVM")
#InDetTrackCutSvcIDPVM.MaxD0 = 2. #mm
#InDetTrackCutSvcIDPVM.MaxZ0 = 250. #mm
#InDetTrackCutSvcIDPVM.MaxEta = 4.0
#InDetTrackCutSvcIDPVM.MinSiHits = 6 # Pixel + SCT
#InDetTrackCutSvcIDPVM.MinPixelHits = 0 
#InDetTrackCutSvcIDPVM.MinSCTHits = 0


#svcMgr += InDetTrackCutSvcIDPVM
#InDetTrackSelectorTool.TrackSelectionSvc = InDetTrackCutSvcIDPVM

ToolSvc += InDetTrackSelectorTool
##### Temporary cuts - Will be fixed in 20.20.8.X ######

print "Set Up InDetTrackSelectorTool"

#-------------------------------------------------------------
# Set up truth selection tool
#-------------------------------------------------------------
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AthTruthSelectionTool
TrackTruthSelectionTool = AthTruthSelectionTool()
# todo: manually adapt this acc. to the fiducial acceptance of your detector
# ie in case of eta range 3.2, that's where you want your maxEta cut to be 
TrackTruthSelectionTool.maxEta     = 4.0
TrackTruthSelectionTool.maxPt      = -1
TrackTruthSelectionTool.minPt      = 1000 # We want to look at the efficiency for 1000 MeV truth particles
TrackTruthSelectionTool.maxBarcode = int(200e3)
TrackTruthSelectionTool.pdgId      = -1
TrackTruthSelectionTool.requireCharged = True
TrackTruthSelectionTool.requireStatus1 = True
TrackTruthSelectionTool.maxProdVertRadius = 260. #max prod. vertex radius of secondaries [mm]
# to select particle within BCL acceptance
#TrackTruthSelectionTool.radiusCylinder = 857 #BCL approximate radius
#TrackTruthSelectionTool.minZCylinder = 1167 #BCL min z
#TrackTruthSelectionTool.maxZCylinder = 1361 #BCL min z
TrackTruthSelectionTool.OutputLevel = INFO
ToolSvc += TrackTruthSelectionTool

#-------------------------------------------------------------
# Set up InDetPhysValMonitoring tool
#-------------------------------------------------------------
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
InDetPhysValMonitoringTool = InDetPhysValMonitoringTool("InDetPhysValMonitoringTool")
InDetPhysValMonitoringTool.useTrackSelection = True
InDetPhysValMonitoringTool.FillITkResolutionPlots = True
InDetPhysValMonitoringTool.FillAdditionalITkPlots = True
InDetPhysValMonitoringTool.TrackSelectionTool = InDetTrackSelectorTool
#InDetPhysValMonitoringTool.TruthSelectionTool = TrackTruthSelectionTool
InDetPhysValMonitoringTool.TruthParticleContainerName = "TruthParticles"
#InDetPhysValMonitoringTool.PileupPlots = False  
# this will run over hardscatter tracks only: 
#InDetPhysValMonitoringTool.PileupSwitch = "HardScatter"
InDetPhysValMonitoringTool.OutputLevel = INFO
ToolSvc += InDetPhysValMonitoringTool

monMan.AthenaMonTools += [InDetPhysValMonitoringTool]

# set up output file 
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["physval DATAFILE='physval.root' OPT='RECREATE'"]

# set out message verbosity 
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = WARNING
ServiceMgr.MessageSvc.defaultLimit = 10000

# max. number of events to process
theApp.EvtMax = -1

# dump configuration
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")

# monitor CPU/memory etc.
from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

