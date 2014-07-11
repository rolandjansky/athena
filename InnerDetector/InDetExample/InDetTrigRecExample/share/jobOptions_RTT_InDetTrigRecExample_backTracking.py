# migrated to new steering
#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# control running truth ?
doTruth         = True #if TRUE running FakeRoIfromKine, if FALSE running FakeLvl1RoIatFixedEtaPhi

# produce an atlantis data file
doJiveXML       = False

# controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = False
doWriteAOD      = False

# --- read BS
doReadBS        = False
if doReadBS:
  doTruth = False

# Monitoring flags
doMonitoring = True

# do auditors ?
doAuditors   = False

#timing measurements
doTimer = False

#CBNT
doCBNT = False
CBNTAthenaAware = True #must be True, otherwise you don't get an ntuple

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
DetDescrVersion = "ATLAS-CSC-01-02-00"

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EventDataModel monitor (debug mode only) ?
  doEdmMonitor    = True
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False
  
#--------------------------------------------------------------
# --- setup class with default values
#--------------------------------------------------------------
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
InDetTrigFlags.doTruth = doTruth
InDetTrigFlags.doNewTracking = True
InDetTrigFlags.doiPatRec = False


#--------------------------------------------------------------
# load Global Flags and set defaults
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import GlobalFlags
#default is atlas geometry
GlobalFlags.DetGeo.set_atlas()
#set defaults
GlobalFlags.DataSource.set_geant4()
if 'doReadBS' in dir() and doReadBS:
  GlobalFlags.InputFormat.set_bytestream()
else:
  GlobalFlags.InputFormat.set_pool()    
#default is zero luminosity
#GlobalFlags.Luminosity.set_zero()
# --- printout
GlobalFlags.Print()

# --- set geometry version (kudge for new jobproperties)
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion
print globalflags.DetDescrVersion()

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
if doCBNT:
	DetFlags.Tile_setOn() #needed for doExtrap in CBNT
	DetFlags.LAr_setOn() #needed for doExtrap in CBNT
	include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
# ---- switch parts of ID off/on as follows
#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.TRT_setOff()
#DetFlags.makeRIO.SCT_setOff()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# load ServiceMgr, ToolSvc, topSequence
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#
from AthenaCommon.AppMgr import ToolSvc

#--------------------------------------------------------------
# change MessageSvc
#--------------------------------------------------------------
  	 
# output level
ServiceMgr.MessageSvc.OutputLevel  = OutputLevel
# increase the number of letter reserved to the alg/tool name from 18 to 30
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

#--------------------------------------------------------------
# Load POOL support, setup for reconstruction
#--------------------------------------------------------------

# setup POOL access in ATHENA
if 'doReadBS' in dir() and doReadBS:
  include("InDetTrigRecExample/InDetTrigReadBS_jobOptions.py")
else:
  import AthenaPoolCnvSvc.ReadAthenaPool

# --- GeoModel
include("AtlasGeoModel/SetGeometryVersion.py" )
include("AtlasGeoModel/GeoModelInit.py" )
# ---- Beam Spot service
if InDetTrigFlags.useBeamConstraint():
  include("InDetBeamSpotService/BeamCondSvc.py" )



# particle property service
include( "PartPropSvc/PartPropSvc.py" )

if InDetTrigFlags.doTruth():
  include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

#------------------------------------------------------------
# Event Data Model Monitor works only in dbg build
#------------------------------------------------------------

if doEdmMonitor:
  from TrkValAlgs.TrkValAlgsConf import Trk__EventDataModelMonitor
  InDetTrigEdmMonitor = Trk__EventDataModelMonitor (name = 'InDetTrigEdmMonitor')

  topSequence += InDetTrigEdmMonitor
  print          InDetTrigEdmMonitor
  
#--------------------------------------------------------------
# Load Inner Detector reconstruction
#--------------------------------------------------------------

# ID reconstruction
from InDetTrigRecExample.EFInDetConfig import *

if not doReadBS:
  electronAlgs = TrigEFIDOutsideIn_Photon()
else:
  electronAlgs = TrigEFIDOutsideIn_Photon()
  
#muonAlgs = TrigEFIDInsideOut_Muon() #ok
#tauAlgs = TrigEFIDInsideOut_Tau() #ok
#electronAlgs = TrigEFIDInsideOut_Bjet() #ok
#electronAlgs = TrigEFIDInsideOut_Bphysics() #ok
#electronAlgs = TrigEFIDInsideOut_FullScan()
#electronAlgs = TrigEFIDInsideOut_MinBias()
#electronAlgs = TrigEFIDOutsideIn_Photon() #ok

# EF Track hypothesis algorithm for electrons
#from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e10, EFTrackHypo_e15i, EFTrackHypo_e25i, EFTrackHypo_e60
#theEFTrackHypo_e10 =  EFTrackHypo_e10()
#theEFTrackHypo_e15i =  EFTrackHypo_e15i()
#theEFTrackHypo_e25i =  EFTrackHypo_e25i()
#theEFTrackHypo_e60 =  EFTrackHypo_e60()

#--------------------------------------------------------------
# Region Selector and Trigger 
#--------------------------------------------------------------

include( "TriggerRelease/TriggerFlags.py" )
include( "RegionSelector/RegionSelector_jobOptions.py" )
include( "InDetRegionSelector/InDetRegionSelectorTable_jobOptions.py" ) #include it temporarily!

theApp.Dlls += [ "TrigSteeringTest" ]
theApp.Dlls += [ "TrigTimeAlgs" ]

########################
# ConfigSvc:
########################

#######################################################
# define/generate menu files for HLT
#######################################################

from TriggerMenuPython.TriggerPythonConfig import *
menu = TriggerPythonConfig("newMenu.xml", "lvl1Menu.xml")
############################################################# Electrons/Photons
# fake L1
item = LVL1MenuItem('L1_EM10', ctpid='1')
item.addCondition(name='EM10', multi='1')
menu.addLVL1Item( item )
menu.addEMThreshold(name='EM10', value='7', range_begin='1', range_end='2' )

# sequence
menu.addSequence( 'EM10', ['PESA::dummyAlgo/Egamma'], 'LVL2Out_Electron' )
menu.addSequence( 'LVL2Out_Electron', electronAlgs.getSequence(), 
'EFInDet_Electron' )

#chains
chain = HLTChain('DummyL2_Electron', '1', 'L1_EM10', 'L2')
chain.addHLTSignature( 'LVL2Out_Electron' )
chain.addTriggerTypeBit(0)
chain.addStreamTag('fake')
menu.addHLTChain(chain)

chain = HLTChain('SingleMC_Electron', '4', 'DummyL2_Electron', 'EF')
chain.addHLTSignature( 'EFInDet_Electron' )
chain.addTriggerTypeBit(4)
chain.addStreamTag('electrons')
menu.addHLTChain(chain)

menu.writeConfigFiles()



###    Setup  TrigConfigSvc      ###
####################################
from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
log.info("setting up TrigConfigSvc:")
svc = SetupTrigConfigSvc()
svc.hltXmlFile = 'newMenu.xml'
svc.l1XmlFile  = 'lvl1Menu.xml'

try:
  svc.SetStates( 'xml' )
except:
  log.warning( 'failed to set state of TrigConfigSvc ...')
try:
  svc.InitialiseSvc()
except:
  log.warning( 'failed to activate TrigConfigSvc ...')
    
ServiceMgr.TrigConfigSvc.OutputLevel=VERBOSE
ServiceMgr.HLTConfigSvc.OutputLevel=VERBOSE
ServiceMgr.LVL1ConfigSvc.OutputLevel=VERBOSE
    


#######################################################
from TrigSteering.TrigSteeringConfig import TrigSteer_L2,TrigSteer_EF

steeringL2 = TrigSteer_L2("TrigSteer_L2", hltFile="newMenu.xml")

if doTruth:
  from TrigFake.TrigFakeConf import FakeLvl1RoIfromKine
  steeringL2.LvlConverterTool = FakeLvl1RoIfromKine("FakeLvl1RoIfromKine")
  steeringL2.LvlConverterTool.FakeEmTauRoiParticleIDs=[11,-11]
  #steeringL2.LvlConverterTool.FakeMuonRoiParticleIDs=[13,-13]
  #steeringL2.LvlConverterTool.FakeJetRoiParticleIDs=[15,-15]
  steeringL2.LvlConverterTool.FakeEmTauRoiPtMin=5*GeV
  steeringL2.LvlConverterTool.McEventCollectionName = "TruthEvent"
  steeringL2.LvlConverterTool.OutputLevel = DEBUG
  steeringL2.OutputLevel = VERBOSE
  topSequence += steeringL2
else:
  from TrigFake.TrigFakeConf import FakeLvl1RoIatFixedEtaPhi
  steeringL2.LvlConverterTool = FakeLvl1RoIatFixedEtaPhi("FakeLvl1RoIatFixedEtaPhi")
  steeringL2.LvlConverterTool.FakeEmRoiPhi=0.0
  steeringL2.LvlConverterTool.FakeEmRoiEta=0.0
  steeringL2.LvlConverterTool.FakeEmRoiLabel="EM10"
  if not doTimer:
    steeringL2.LvlConverterTool.OutputLevel = DEBUG
    steeringL2.OutputLevel = VERBOSE
  else:
    steeringL2.LvlConverterTool.OutputLevel = ERROR
    steeringL2.OutputLevel = ERROR	
  topSequence += steeringL2
  
steeringEF = TrigSteer_EF("TrigSteer_EF", hltFile="newMenu.xml")
if doTimer:
  steeringEF.OutputLevel = ERROR
else:
  steeringEF.OutputLevel = VERBOSE
topSequence += steeringEF

#fix to get rid of EF exceptions
excSvc=Service("ExceptionSvc")
excSvc.Catch="LIST"
excSvc.Algorithms=[ "TrigSteer_EF=SUCCESS"]

# -------------------------------------------------------------
theApp.EvtMax  = 1000
if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.EventSelector.InputCollections = ["rfio:/castor/cern.ch/user/e/enzuobon/RTT/RTTdata/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00001.pool.root.4"]
  #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
else:
  ByteStreamInputSvc.FullFileName = ["/afs/cern.ch/atlas/project/trigger/pesa-sw/data/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.bs-lvl1sim.v12000301_tid003138._00001.pool.root"]
# ------------------------------------------------------------

# switch off history service
ServiceMgr.StoreGateSvc.ActivateHistory = False
ServiceMgr.StoreGateSvc.Dump            = False

# ------------------------------------------------------------
# get stack dump
# ------------------------------------------------------------

gbl.AthenaServices.SetFatalHandler(438)
         
if doAuditors:
 # load AuditorSvc
 theAuditorSvc = AuditorSvc()
 
 # write out summary of the memory usage
 #   number of events to be skip to detect memory leak
 #   20 is default. May need to be made larger for complete jobs.
 theAuditorSvc.Auditors += [ "ChronoAuditor"]
 ServiceMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 5
 # memory auditors
 theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
 MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
 MemStatAuditor.OutputLevel = WARNING
 
# write out a short message upon entering or leaving each algorithm
if doNameAuditor:
  theAuditorSvc.Auditors     += [ "NameAuditor" ]
  theApp.AuditAlgorithms = True
  theApp.AuditServices   = True
  #
  # Display detailed size and timing statistics for writing and reading
  #
if doWriteESD or doWriteAOD:
  AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
  AthenaPoolCnvSvc.UseDetailChronoStat = True
     
#--------------------------------------------------------------
# run JiveXML
#--------------------------------------------------------------
if doJiveXML:
  include ("JiveXML/JiveXML_jobOptionBase.py")
  include ("JiveXML/DataTypes_InDet.py")

  from InDetJiveXML.InDetJiveXMLConf import JiveXML__SiClusterRetriever
  theSiClusterRetriever = JiveXML__SiClusterRetriever (name = "SiClusterRetriever")
  theSiClusterRetriever.SCTClusters = "SCT_TrigClusters"
  print theSiClusterRetriever
  
  from InDetJiveXML.InDetJiveXMLConf import JiveXML__SiSpacePointRetriever
  theSiSpacePointRetriever = JiveXML__SiSpacePointRetriever (name = "SiSpacePointRetriever")
  theSiSpacePointRetriever.PixelSpacePoints = "PixelTrigSpacePoints"
  theSiSpacePointRetriever.SCTSpacePoints = "SCT_TrigSpacePoints"
  
  from InDetJiveXML.InDetJiveXMLConf import JiveXML__TRTRetriever
  theTRTRetriever = JiveXML__TRTRetriever (name = "TRTRetriever")
  theTRTRetriever.TRTClusters = "TRT_TrigDriftCircles"

  from TrkJiveXML.TrkJiveXMLConf import JiveXML__TrackRetriever
  theTrackRetriever = JiveXML__TrackRetriever (name = "TrackRetriever")
  theTrackRetriever.DoWriteHLT = True

  if doTruth:
    readG3 = False
    include ("JiveXML/DataTypes_Truth.py")
    
# ------------------------------------------------------------
# persistency
# ------------------------------------------------------------

if doWriteAOD: 
   # create stream
   theApp.TopAlg += [ "AthenaOutputStream/StreamAOD" ]
   StreamAOD = Algorithm( "StreamAOD" )
   StreamAOD.EvtConversionSvc = "AthenaPoolCnvSvc"

   # Define the output file name
   StreamAOD.OutputFile    = "InDetTrigRecAOD.root"
   
   #force read of input data to write in output data
   StreamAOD.ForceRead = True

   #  EventInfo - needs to be always saved.
   StreamAOD.ItemList=[]
   StreamAOD.ItemList+=["EventInfo#*"]
   include ( "InDetTrigRecExample/WriteTrigInDetAOD.py" )

if doWriteESD:
   # create stream
   theApp.TopAlg += [ "AthenaOutputStream/StreamESD" ]
   StreamESD = Algorithm( "StreamESD" )
   StreamESD.EvtConversionSvc = "AthenaPoolCnvSvc"

   # Define the output file name
   StreamESD.OutputFile    = "InDetTrigRecESD.root"
   
   #force read of input data to write in output data
   StreamESD.ForceRead = True

   #  EventInfo - needs to be always saved.
   StreamESD.ItemList=[]
   StreamESD.ItemList+=["EventInfo#*"]
   include ( "InDetTrigRecExample/WriteTrigInDetESD.py" )

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
if doMonitoring or doTimer:
    ServiceMgr.THistSvc.Output = [ "EXPERT DATAFILE='expert-monitoring.root'  OPT='RECREATE'" ]

if doTimer:
    for algo in steeringEF.getChildren():
      print "HERE", algo.getFullName()
      algo.doTiming = True
 
#######################################################33
#
# Testing options
#######################################################33
if not doTruth:
  PixelClustering_EF.EtaHalfWidth = 3.0
  PixelClustering_EF.PhiHalfWidth = 3.14159
  #
  SCTClustering_EF.EtaHalfWidth = 3.0
  SCTClustering_EF.PhiHalfWidth = 3.14159
  #
  TRTDriftCircleMaker_EF.EtaHalfWidth = 3.0
  TRTDriftCircleMaker_EF.PhiHalfWidth = 3.14159
  #
  SiTrigSpacePointFinder_EF.EtaHalfWidth = 3.0
  SiTrigSpacePointFinder_EF.PhiHalfWidth = 3.14159
  #
  SiTrigTrackFinder_EF.EtaHalfWidth = 3.0
  SiTrigTrackFinder_EF.PhiHalfWidth = 3.14159

# ------------------------------------------------------------
# useful debugging/info tools
# ------------------------------------------------------------
for algo in steeringEF.getChildren():
  if not doTimer:
    algo.OutputLevel = DEBUG
  else:
    algo.OutputLevel = ERROR

Algorithm("AANTupleStream").OutputLevel=DEBUG
ServiceMgr.THistSvc.OutputLevel=DEBUG

if doCBNT:
  THistSvc = Service ( "THistSvc" )
  theApp.Dlls   += [ "AnalysisTools" ]
  AANTupleStream = Algorithm( "AANTupleStream" )
  #AANTupleStream.ExtraRefNames = [ "StreamESD","StreamRDO" ]
  AANTupleStream.ExistDataHeader = False
  AANTupleStream.TreeName = "CollectionTree"
  AANTupleStream.WriteInputDataHeader = True
  RootNtupleOutput = "ntuple.root"
  #RootNtupleOutput = "timing-histograms.root"
  exec 'AANTupleStream.OutputName= \'%s\' ' % RootNtupleOutput
  AANTupleStream.OutputName = RootNtupleOutput
  
  exec 'THistSvc.Output += ["AANT DATAFILE=\'%s\' OPT=\'RECREATE\'"] ' % RootNtupleOutput
 
  include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
#  Algorithm("CBNT_AthenaAware").OutputLevel=DEBUG
  CBNT_AthenaAware.OutputLevel=DEBUG
  
  #theApp.TopAlg += ["CBNT_AthenaAware"]
  theApp.TopAlg += [ "AANTupleStream" ]
  #include( "RecExCommon/CBNT_Truth_jobOptions.py" )
  include("TrigNtInDet/jobOfragment_TrigNtInDet.py")

  if (TriggerFlags.doID):
    theCBNTAA_TrigEFTrackParticle.doDetailedTruth = True
    theCBNTAA_TrigEFTrackParticle.doTruthCBNT = True
    theCBNTAA_TrigEFTrackParticle.OutputLevel=DEBUG
    theCBNTAA_TrigEFVxPrimary.OutputLevel=DEBUG
    theCBNTAA_TrigEFTrackParticle.doExtrap = True

print topSequence
