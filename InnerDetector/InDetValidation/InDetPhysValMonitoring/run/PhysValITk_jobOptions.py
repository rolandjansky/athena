# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: PhysVal_jobOptions.py 714189 2015-12-11 17:33:02Z sroe $

#Set to False if running on AOD or ESD - True for IDTRKVALDAOD
runDAOD = False

#-----------------------------------------------------------------------
# set up the input file:
# set up for reading pool.root format files (AOD,ESD,IDTRKVALDAOD)
import AthenaPoolCnvSvc.ReadAthenaPool

# read single file: change acc. to your file name here
FNAME=[ "AOD.pool.root" ]
# uncomment this for testing: step 1.2, mu=200 file:
#FNAME=[ "root://eosatlas//eos/atlas/user/l/lmijovic/shared_atlas/upgrade/inputs/mc15_14TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.recon.AOD.e1133_s2900_s2904_r7914/AOD.08762005._000011.pool.root.1" ]
# uncomment to read multiple files:
#import glob
#FNAME=glob.glob('indir/*pool.root*') 

#
# make AthenaCommonFlags aware of which file we are using
# AthenaCommonFlags are used run-time configuration (InputFilePeeker)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
svcMgr.EventSelector.InputCollections = FNAME
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections
#-----------------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']

xmlTags = [ ["ATLAS-P2-ITK-05","ExtBrl_32",""],
            ["ATLAS-P2-SFCAL-01-05-01","ExtBrl_32","GMX"],
            ["ATLAS-P2-ITK-06","ExtBrl_4",""],
            ["ATLAS-P2-SFCAL-01-06-01","ExtBrl_4","GMX"],
            ["ATLAS-P2-ITK-07","IExtBrl_4",""],
            ["ATLAS-P2-SFCAL-01-07-01","IExtBrl_4","GMX"],
            ["ATLAS-P2-ITK-08","InclBrl_4",""],
            ["ATLAS-P2-SFCAL-01-08-01","InclBrl_4","GMX"],
            ["ATLAS-P2-ITK-10-00-00","InclBrl_4","GMX"],
            ["ATLAS-P2-ITK-09-00-00","ExtBrl_4","GMX"],
            ["ATLAS-P2-ITK-10-00-01","InclBrl_4","GMX"],
            ["ATLAS-P2-ITK-09-00-01","ExtBrl_4","GMX"],
            ["ATLAS-P2-ITK-11-00-01","ExtBrl_4_33mm","GMX"],
            ["ATLAS-P2-ITK-12-00-01","IExtBrl_4","GMX"],]

for geoTag, layoutDescr, gmx in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      print "preIncludes for ",layoutDescr, " layout"
      from InDetRecExample.InDetJobProperties import InDetFlags
      include('InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py')
      include('InDetSLHC_Example/preInclude.SLHC_Setup_'+layoutDescr+'.py')
      if gmx=="GMX":
         print "preIncludes for GMX strip layout"
         include('InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py')
         if geoTag=="ATLAS-P2-ITK-10-00-00" or geoTag=="ATLAS-P2-ITK-09-00-00" :
            include('InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry.py')
         else:
            include('InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py')
      else : 
         include('InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry.py')
      break

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

         
from AthenaCommon.GlobalFlags import jobproperties
DetDescrVersion = jobproperties.Global.DetDescrVersion()

for geoTag, layoutDescr,gmx in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      print "postInclude for ",layoutDescr, " layout"
      include('InDetSLHC_Example/postInclude.SLHC_Setup_'+layoutDescr+'.py')
      break

#import TrkDetDescrSvc.AtlasTrackingGeometrySvc

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/InDetPVMPlotDefITK.xml"
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
theApp.EvtMax = -1

# dump configuration
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
