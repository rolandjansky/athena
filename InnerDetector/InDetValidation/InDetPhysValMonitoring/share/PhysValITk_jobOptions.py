# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
# uncomment this for testing: step 3, mu=0 file. For a quick run also set theApp.EvtMax to 10 or so
# todo: replace this with cergroup file, once we get some step3 files there 
FNAME=[ "/eos/user/l/lmijovic/atlas/nosyn/upgrade/inputs/step3_shared/step3prod_test_18082018/ttbar/step3/DAOD_IDTRKVALID_digi.pool.root" ]

# uncomment to set a single input file via the athena command line options (athena -c "INFILE='blah.root'" )
#if "INFILE" in globals():
#      FNAME= [INFILE]
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
# this flag is necessary to run correctly over DAODs.
# It is set automatically from the name
#as long as the names are following standard convention 
runDAOD = False # these are DAOD-s

# This codeblock is for handling inputFileName passed as environment variable
# try:
#   FNAME = os.environ["inputESDFile"]
# except KeyError:
#   try:
#       FNAME = os.environ["inputDAOD_IDTRKVALIDFile"]
#       runDAOD = True # these are DAOD-s
#   except KeyError:
#       try:
#          FNAME = os.environ["inputAODFile"]
#       except KeyError:
#          print "No input file specified in environment - Using the default input file mentioned in the script"

# if isinstance(FNAME,str):
# 	FNAME = FNAME.split(",")
#-----------------------------------------------------------------------

# make AthenaCommonFlags aware of which file we are using
# AthenaCommonFlags are used run-time configuration (InputFilePeeker)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# svcMgr.EventSelector.InputCollections = FNAME
# athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

if len(athenaCommonFlags.FilesInput.get_Value()) == 0:
   athenaCommonFlags.FilesInput = FNAME
#-----------------------------------------------------------------------

if "DAOD_IDTRKVALID" in svcMgr.EventSelector.InputCollections[0]:
      print "Set up for DAOD processing"
      runDAOD=True
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
            ["ATLAS-P2-ITK-17-00-00","InclBrl_4","InclinedAlternative"],
            ["ATLAS-P2-ITK-17-00-01","InclBrl_4","InclinedAlternative"]
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
#      if layoutOption=="InclinedAlternative" :
#         # in step3 we are using hit identifier translation to cope with the L0/L1 rings.
#         # If you want to run wo translation set this to False
#         print "Setting Layout translation helper to true"
#         ToolSvc.LayoutTranslationHelper.translateIdentifiersForInclinedAlternative=True
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
monMan.FileKey = "MyPhysVal"
topSequence += monMan

#-------------------------------------------------------------
# Set up track selection tool
#-------------------------------------------------------------
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool = InDet__InDetTrackSelectionTool("InDetTrackSelectorTool")
##### hard-coded cuts in r20.20, leave these as is, variations will not have an intended effect 
InDetTrackSelectorTool.minPt            = 400           # Mev
InDetTrackSelectorTool.maxD0            = 2              # mm
InDetTrackSelectorTool.maxZ0            = 250          # mm
InDetTrackSelectorTool.minNSiHits       = 6            # Pixel + SCT
InDetTrackSelectorTool.maxNPixelHoles   = 2             # Pixel only
InDetTrackSelectorTool.OutputLevel = INFO
#eta dependant hit cut below
#InDetTrackSelectorTool.vecEtaCutoffsForSiHitsCut = [0,1.0,1.2,1.8,2.2]
#InDetTrackSelectorTool.vecMinNSiHitsAboveEta = [11,11,11,13,10]

## reco-level cuts were done using 
## ... InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrackCutSvc
## ... in r20.20. This is not available in 21.9
## ... todo (LM): add reco-level selectors once we have sth in r21.9

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
# Note: Please set the following to < 1000 (e.g. 990) when running on 1 GeV single particles. 
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
svcMgr.THistSvc.Output += ["MyPhysVal DATAFILE='MyPhysVal.root' OPT='RECREATE'"]

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
