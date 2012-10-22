#==============================================================
#
# Job options file for MuonRecValidationAOD
#
# MuonValidationTeam
# Mar. 2007
#==============================================================

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()

include( "AthenaCommon/AthenaCommonFlags.py" )
from AthenaCommon.DetFlags import DetFlags
#--- Detector flags -------------------------------------------
# - Select detectors
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
#==============================================================
useSTACO = False
doMuonRecValidationESD=True
#==============================================================
# MuonGeoModel Initialization
#--- GeoModel stuff -----------------------------------------
GeoModelSvc = Service( "GeoModelSvc")
GeoModelSvc.IgnoreTagDifference = True
#==============================================================  
# Event AthenaPool
import AthenaPoolCnvSvc.ReadAthenaPool 
svcMgr.EventSelector.BackNavigation = True
#--------------------------------------------------------------
# Particle properties
#--------------------------------------------------------------
# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()   
#--------------------------------------------------------------
# The POOL converters
#-------------------------------------------------------------- 
include( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
#==============================================================
# the POOL converters
#==============================================================
include( "UserAnalysisEvent/UserAnalysisEventDict_joboptions.py" )
#==============================================================
# For the cond DB
include("IOVDbSvc/IOVRecExCommon.py") 
#============================================================== 
# Options for reading from POOL (use POOL implicit collection)
#==============================================================  
# Pool input 
svcMgr.EventSelector.InputCollections = [ 
##Release 13.0.10
"rfio:/castor/cern.ch/user/m/muonprod/v13001001_tid010895/valid1_misal1_valid1.005145.PythiaZmumu/recon.ESD/ESD.010895._00001.pool.root.1" 
]   
#==============================================================
# The user analysis algorithm to be executed
if not "AnalysisTools" in theApp.Dlls:
   theApp.Dlls += ["AnalysisTools"]

if not "UserAnalysis" in theApp.Dlls:
   theApp.Dlls += ["UserAnalysis"]

if not "McParticleTools" in theApp.Dlls:
   theApp.Dlls += ["McParticleTools"]

if not "MuonRecValidator" in theApp.Dlls:
   theApp.Dlls += ["MuonRecValidator"]
#==============================================================
#==============================================================
from MuonRecValidator.MuonRecValidatorConf import MuonRecValidatorAOD
MuonRecValidatorAOD=MuonRecValidatorAOD(name = "MuonRecValidatorAOD")
#If you want to run on ESDs, set this parameter to True. Also modify the container names accordingly
MuonRecValidatorAOD.ConvertFromESD = True

if MuonRecValidatorAOD.ConvertFromESD:
	MuonRecValidatorAOD.CnvTool.McEvents = "TruthEvent"
	MuonRecValidatorAOD.CnvTool.TruthParticlesOutput = "SpclMC_ESD"
	MuonRecValidatorAOD.TruthParticles = "SpclMC_ESD"

# The user analysis cuts - default values to changed by the user as needed
MuonRecValidatorAOD.MaximalConeDistance = 0.05
MuonRecValidatorAOD.MinimalMuonPt = 20000.
#==============================================================
# For AOD, track Containers name according to the 
# https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD#Track_Particles_TrackParticle 
# wiki page
# For ESD, track Containers name according to the 
# https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForESD wiki page
#MuonRecValidatorAOD.TrackContainerNames = ["StacoTrackParticles"]
MuonRecValidatorAOD.TrackContainerNames = ["MooreTrackParticles","MuonboyTrackParticles"]
#==============================================================
# Track Containers name according to the
# https://twiki.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD#Loosely_Identified_Particles_Par
# wiki page
#==============================================================
if MuonRecValidatorAOD.ConvertFromESD:
   if useSTACO:
      MuonRecValidatorAOD.MuonCollectionNames = ["StacoCombinedMuonContainer"]
   else:
      MuonRecValidatorAOD.MuonCollectionNames = ["MuidCombinedMuonContainer"]

#==============================================================
# ONLY in case you compare 2 versions (releases, files, etc..)
MuonRecValidatorAOD.VersionComparing = False
# otherwise put FALSE and if you want to produce comparison plots of ALL possible Muon Reco Alg
# by using one reco AOD file 
#==============================================================
MuonRecValidatorAOD.VersionComparingContainerName ="MuonboyTrackParticles"
#MuonRecValidator.VersionComparingCollectionName ="StacoMuonCollection"
#==============================================================
# ONLY in case you have choosen MuonRecValidator.VersionComparing = True
MuonRecValidatorAOD.VersionComparingName1 = "Reco_Ideal"
MuonRecValidatorAOD.VersionComparingName2 = "Reco_Misaligned"
#==============================================================
MuonRecValidatorAOD.EfficiencyBinning_vs_Pt  = [0.0, 100000., 20]
MuonRecValidatorAOD.EfficiencyBinning_vs_Eta = [-2.7, 2.7, 14]
MuonRecValidatorAOD.EfficiencyBinning_vs_Phi = [-3.2, 3.2, 16]

MuonRecValidatorAOD.ResolutionBinning_vs_Pt  = [0.0, 60000., 5]
MuonRecValidatorAOD.ResolutionBinning_vs_Eta = [-2.7, 2.7, 7]
MuonRecValidatorAOD.ResolutionBinning_vs_Phi = [-3.2, 3.2, 8]

MuonRecValidatorAOD.AnalyseDiMuonFinalState = False 
MuonRecValidatorAOD.DiMuonBinning_vs_Pt = [20000., 60000., 3]
MuonRecValidatorAOD.DiMuonBinning_vs_Eta= [-3.0, 3.0, 3]

MuonRecValidatorAOD.EventDumping = True
MuonRecValidatorAOD.OutputLevel = INFO
#==============================================================
sampletype='PythiaZmumu';
atlastag='misal1_valid1';
dstnum='005145'
recotype='ESD'
RootTupleOutputFileName = 'RootTuple_'+str(atlastag)+'.'+str(dstnum)+'.recon.'+str(recotype)+'.'+str(sampletype)+'.aan.root' 
#-------------------------------------------------------------- 
# histograms
#-------------------------------------------------------------- 
if os.path.exists(RootTupleOutputFileName):
   os.remove(RootTupleOutputFileName)
   
if MuonRecValidatorAOD.ConvertFromESD:
   #Root Ntuple output file and name
   MuonRecValidatorAOD.OutputFile = RootTupleOutputFileName
##########################################

# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["AANT DATAFILE='%s' OPT='NEW'" % RootTupleOutputFileName ]
#==============================================================
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit=1000000
#==============================================================
# Number of Events to process
theApp.EvtMax = 500
###################### Detail time measurement and auditors
#-------------------------------------------------------------- 
#Detail time measurement and auditors write out a summary of the time spent
#-------------------------------------------------------------- 
# useful debugging/info tools
theApp.Dlls += [ "GaudiAlg","GaudiAud" ]
# write out a summary of the time spent
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True
svcMgr.ChronoStatSvc.ChronoDestinationCout = True
svcMgr.ChronoStatSvc.PrintUserTime     = True
svcMgr.ChronoStatSvc.PrintSystemTime   = True
svcMgr.ChronoStatSvc.PrintEllapsedTime = True
svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "MuonRecValidationESD_chronoStats.ascii"
#-------------------------------------------------------------- 
# write out summary of the memory usage
#   number of events to be skip to detect memory leak
# Set to 50 to give some time to stabilise
ServiceMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 50
theApp.AuditAlgorithms=True 
#--------------------------------------------------------------  
# run again, but harmless crash at the end of the job
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditServices=True
#---------------------------------------------------------------------------  
# printout summary of messages
#---------------------------------------------------------------------------  
theAuditorSvc.Auditors += [ "AlgErrorAuditor" ]
# show summary of WAR NING ER ROR and FATAL messages
#---------------------------------------------------------------------------  
svcMgr.MessageSvc.showStats=True
svcMgr.MessageSvc.statLevel=WARNING
svcMgr.MessageSvc.enableSuppression = True
#--------------------------------------------------------------  
include ("LSFTimeKeeper/LSFTimeKeeperOptions.py")
LSFTimeKeeperSvc= Service("LSFTimeKeeperSvc")
LSFTimeKeeperSvc.OutputLevel=INFO
#--------------------------------------------------------------  
if doMuonRecValidationESD:
   from PerfMonComps.PerfMonFlags import jobproperties
   jobproperties.PerfMonFlags.doMonitoring = True
   jobproperties.PerfMonFlags.doPersistencyMonitoring = True
   jobproperties.PerfMonFlags.OutputFile = "MuonRecValidationESD.perfmon.root"
   from PerfMonComps.JobOptCfg import PerfMonSvc
   svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )
##--------------------------------------------------------------
## Application: load TagInfoMgr
##--------------------------------------------------------------
#include( "EventInfoMgt/EventInfoMgt_jobOptions.py" )
#TagInfoMgr = Service( "TagInfoMgr" )
#TagInfoMgr.OutputLevel = 2
#--------------------------------------------------------------
print "List of all top algorithms"
print theApp.TopAlg
#================================================================
  
 
 
