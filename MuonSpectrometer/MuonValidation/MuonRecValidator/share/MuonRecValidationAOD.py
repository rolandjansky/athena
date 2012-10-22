#==============================================================
#
# Job options file for MuonRecValidationAOD
#
# MuonValidationTeam
# Dec. 2007
#==============================================================
# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# import the data types 
import EventKernel.ParticleDataType

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
 
# load additional libraries for back navigation
from AnalysisExamples.AnalysisFlags import AnalysisFlags
if AnalysisFlags.DoNavigation:
   include( "RecExCommon/AllDet_detDescr.py" ) 
   svcMgr.EventSelector.BackNavigation = True

include( "AthenaCommon/AthenaCommonFlags.py" )
from AthenaCommon.DetFlags import DetFlags
#--- Detector flags -------------------------------------------
# - Select detectors
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn() 
#==============================================================
doMuonRecValidationAOD=True
#==============================================================
# MuonGeoModel Initialization
#--- GeoModel stuff -----------------------------------------
GeoModelSvc = Service( "GeoModelSvc")
GeoModelSvc.IgnoreTagDifference = True
#==============================================================  
#--------------------------------------------------------------
# Particle properties
#--------------------------------------------------------------
# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()
#==============================================================
#--------------------------------------------------------------
# The POOL converters
#--------------------------------------------------------------
include( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
#==============================================================
# For the cond DB
include("IOVDbSvc/IOVRecExCommon.py") 
#============================================================== 
# Options for reading from POOL (use POOL implicit collection)
#==============================================================  
# ------ Event selector:
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
# if file not in catalog put it there
svcMgr.PoolSvc.AttemptCatalogPatch=True 
   
# ATLAS release
atlrel="v13003003"

if atlrel == "v13002001" :
   include ( "MuonRecValidator/InputCollections_13002001.py" )
elif atlrel == "v13002002" :   
   include ( "MuonRecValidator/InputCollections_13002002.py" )
elif atlrel == "v13002003" :   
   include ( "MuonRecValidator/InputCollections_13002003.py" )
elif atlrel == "v13003001" :   
   include ( "MuonRecValidator/InputCollections_13003001.py" )
elif atlrel == "v13003002" :   
   include ( "MuonRecValidator/InputCollections_13003002.py" )
elif atlrel == "v13003003" :    
   include ( "MuonRecValidator/InputCollections_13003003.py" )

# StoreGate dump of AOD keys
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True  #true will dump data store contents

### get the StoreGateSvc pointer
#from StoreGate.Bindings import StoreGate
#sg = StoreGate.pointer("StoreGateSvc")
#sg.dump()

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
from MuonRecValidator.MuonRecValidatorConf import MuonRecValidatorAOD
MuonRecValidatorAOD=MuonRecValidatorAOD(name = "MuonRecValidatorAOD")
MuonRecValidatorAOD.ConvertFromESD = False #If you want to run on ESDs, set this parameter to True. Also modify the container names accordingly
# The user analysis cuts - default values to changed by the user as needed
MuonRecValidatorAOD.MaximalConeDistance = 0.05
MuonRecValidatorAOD.MinimalMuonPt = 20000.
#==============================================================
# For AOD, track Containers name according to the 
# https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD#Track_Particles_TrackParticle 
# https://twiki.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD#Loosely_Identified_Particles_Par
# wiki pages
# For ESD, track Containers name according to the 
# https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForESD
# wiki page
#MuonRecValidatorAOD.TrackContainerNames = ["StacoTrackParticles"]
MuonRecValidatorAOD.TrackContainerNames = ["MooreTrackParticles","MuonboyMuonSpectroOnlyTrackParticles", "TrackParticleCandidate" ]
MuonRecValidatorAOD.MuonCollectionNames = ["StacoMuonCollection","MuidMuonCollection"]
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

MuonRecValidatorAOD.AnalyseDiMuonFinalState = True,
MuonRecValidatorAOD.DiMuonBinning_vs_Pt = [20000., 60000., 3]
MuonRecValidatorAOD.DiMuonBinning_vs_Eta= [-3.0, 3.0, 3]
MuonRecValidatorAOD.EventDumping = True
MuonRecValidatorAOD.OutputLevel = INFO
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += MuonRecValidatorAOD 
print MuonRecValidatorAOD 
 
#==============================================================
atlastag='trig1_misal1_valid1';
sampletype='PythiaZmumu';
dstnum='005145'
recotype='AOD'
release=atlrel
RootTupleOutputFileName = 'MuonRecValidator_'+str(atlastag)+'.'+str(dstnum)+'.'+str(sampletype)+'.recon.'+str(recotype)+'.'+str(release)+'.aan.root' 
#-------------------------------------------------------------- 
# histograms
#-------------------------------------------------------------- 
if os.path.exists(RootTupleOutputFileName):
   os.remove(RootTupleOutputFileName)

#Root Ntuple output file and name
MuonRecValidatorAOD.OutputFile = RootTupleOutputFileName
#==============================================================
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
theApp.EvtMax = -1
###################### Detail time measurement and auditors
#-------------------------------------------------------------- 
#Detail time measurement and auditors write out a summary of the time spent
#-------------------------------------------------------------- 
# write out a summary of the time spent
from GaudiSvc.GaudiSvcConf import AuditorSvc
## useful debugging/info tools
svcMgr.AuditorSvc.Auditors  += [ "ChronoAuditor", "MemStatAuditor"] 
svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True
svcMgr.ChronoStatSvc.ChronoDestinationCout = True
svcMgr.ChronoStatSvc.PrintUserTime     = True
svcMgr.ChronoStatSvc.PrintSystemTime   = True
svcMgr.ChronoStatSvc.PrintEllapsedTime = True 
svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "MuonRecValidationAOD_chronoStats.ascii"
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
if doMuonRecValidationAOD:
   from PerfMonComps.PerfMonFlags import jobproperties
   #by default, PerfMon is disabled: enable it    
   #see https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
   jobproperties.PerfMonFlags.doMonitoring = True
   jobproperties.PerfMonFlags.doPersistencyMonitoring = True
   jobproperties.PerfMonFlags.OutputFile = "MuonRecValidationAOD.pmon.gz"
   from PerfMonComps.JobOptCfg import PerfMonSvc
   svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )
   
#--------------------------------------------------------------
print "List of all top algorithms"
print theApp.TopAlg
#================================================================
