#==============================================================
#
# Job options file for ZmumuValidation
#
# MuonValidationTeam
# July 2007
#==============================================================
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

include( "AthenaCommon/AthenaCommonFlags.py" )
from AthenaCommon.DetFlags import DetFlags
#--- Detector flags -------------------------------------------
# - Select detectors
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
#==============================================================
useSTACO = True
#==============================================================
# MuonGeoModel Initialization
#--- GeoModel stuff -----------------------------------------
GeoModelSvc = Service( "GeoModelSvc")
GeoModelSvc.IgnoreTagDifference = True
#==============================================================  
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" ) 
#--------------------------------------------------------------
# Particle properties
#-------------------------------------------------------------- 
include( "PartPropSvc/PartPropSvc.py" ) 
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
EventSelector = Service( "EventSelector" ) 
EventSelector.InputCollections += [
##Release 13.0.10
"rfio:/castor/cern.ch/user/m/muonprod/v13001001_tid010895/valid1_misal1_valid1.005145.PythiaZmumu/recon.ESD/ESD.010895._00001.pool.root.1" 
] 
#==============================================================
# The user analysis algorithm to be executed
if not "AnalysisTools" in theApp.Dlls:
   theApp.Dlls += ["AnalysisTools"]
if not "UserAnalysis" in theApp.Dlls:
   theApp.Dlls += ["UserAnalysis"]
if not "MuonInSituPerformance" in theApp.Dlls:
   theApp.Dlls += ["MuonInSituPerformance"]

#============================================================== 
from MuonInSituPerformance.MuonInSituPerformanceConf import ZmumuValidator
ZmumuValidator=ZmumuValidator(name = "ZmumuValidator")
ZmumuValidator.OutputLevel = INFO
#If you want to run on ESDs, set this parameter to True. Also modify the container names accordingly
ZmumuValidator.ConvertFromESD = True

if ZmumuValidator.ConvertFromESD:
   ZmumuValidator.CnvTool.McEvents = "TruthEvent"
   ZmumuValidator.CnvTool.TruthParticlesOutput = "SpclMC_ESD"
   ZmumuValidator.TruthParticles = "SpclMC_ESD"
#==============================================================
# For AOD, track Containers name according to the 
# https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD#Track_Particles_TrackParticle 
# wiki page
# For ESD, track Containers name according to the 
# https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForESD wiki page
# track Containers name according to the
# https://twiki.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD#Loosely_Identified_Particles_Par
# wiki page
#==============================================================
if ZmumuValidator.ConvertFromESD:
   if useSTACO:
      ZmumuValidator.MuonContainer = "StacoCombinedMuonContainer" # for MuonContainer
      ZmumuValidator.MuonSpectrometerContainer = "MuonboyTrackParticles" # for TrackParticle Container in MuonStandAlone, no MuonContainer
      ZmumuValidator.InnerTrackContainer ="TrackParticleCandidate"    
   else:
      ZmumuValidator.MuonContainer = "MuidCombinedMuonContainer"
      ZmumuValidator.MuonSpectrometerContainer = "MooreTrackParticles" # for TrackParticle Container in MuonStandAlone, no MuonContainer
      ZmumuValidator.InnerTrackContainer ="TrackParticleCandidate" 
   
# Trigger_Level_3_MS   Muon Spectrometer standalone track (non energy loss recovery applied)
# Trigger_Level_3_MV   Muid StandAlone track (MS vertex extrapolated with energy loss correction)
# Trigger_Level_3_CB   Muid Combined track (MS+ID fitted)

ZmumuValidator.TriggerContainer += [ "Trigger_Level_3_MS" ]
ZmumuValidator.TriggerContainer += [ "Trigger_Level_3_MV" ]
ZmumuValidator.TriggerContainer += [ "Trigger_Level_3_CB" ]

# The user analysis cuts - default values to changed by the user as needed
ZmumuValidator.MaximalConeDistance = 0.05
ZmumuValidator.MinimalMuonPt = 5000.
#==============================================================
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
#==============================================================
# Number of Events to process
theApp.EvtMax = 100
theApp.EvtSel = "EventSelector"
#==============================================================
sampletype='PythiaZmumu';
atlastag='misal1_valid1';
dstnum='005145'
recotype='ESD'
RootTupleOutputFileName = 'MuonInSituPerformance_'+str(atlastag)+'.'+str(dstnum)+'.recon.'+str(recotype)+'.'+str(sampletype)+'.aan.root' 
#-------------------------------------------------------------- 
# histograms
#-------------------------------------------------------------- 
if os.path.exists(RootTupleOutputFileName):
   os.remove(RootTupleOutputFileName)   
#==============================================================
 
# Root Ntuple output file and name
if ZmumuValidator.ConvertFromESD:
   # Root Ntuple output file and name
   ZmumuValidator.OutputFile = RootTupleOutputFileName
#==============================================================
THistSvc = Service ( "THistSvc" )
THistSvc.Output = ["AANT DATAFILE='%s' OPT='NEW'" % RootTupleOutputFileName]
theApp.TopAlg += [ "AANTupleStream" ]
#==============================================================
print '\n************************************************************\n'
print " \nUsing following names: "
print "   filename: ", RootTupleOutputFileName   
print '\n************************************************************\n'
# HiggsAnalysisNtupleWriter.py
###################### Detail time measurement and auditors
#--------------------------------------------------------------
#Detail time measurement and auditors write out a summary of the time spent
#-------------------------------------------------------------- 
theApp.Dlls += [ "GaudiAlg","GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor" ]
#-------------------------------------------------------------- 
# Use auditors
theApp.AuditAlgorithms=True
#--------------------------------------------------------------  
include ("LSFTimeKeeper/LSFTimeKeeperOptions.py")
LSFTimeKeeperSvc= Service("LSFTimeKeeperSvc")
LSFTimeKeeperSvc.OutputLevel=INFO
#--------------------------------------------------------------  
AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE
#--------------------------------------------------------------  
#--------------------------------------------------------------
# Application: load TagInfoMgr
#--------------------------------------------------------------
include( "EventInfoMgt/EventInfoMgt_jobOptions.py" )
TagInfoMgr = Service( "TagInfoMgr" )
TagInfoMgr.OutputLevel = 2
#--------------------------------------------------------------
print "List of all top algorithms"
print theApp.TopAlg
#================================================================
  
 
