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
doMuonRecValidationAODRTT=True
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
 
# Collection of input files
include ( "MuonRecValidator/InputCollections_filetest2.py" )

#include ( "./InputCollections_filetest2.py" )
 
# StoreGate dump of AOD keys
svcMgr.StoreGateSvc.Dump = False  #true will dump data store contents
 
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
atlastag='test_tag';
sampletype='PythiaZmumu';
dstnum='005145'
recotype='AOD_RTT'
release='15_n1' #atlrel
RootTupleOutputFileName = 'V0102_RecVal_'+str(atlastag)+'.'+str(dstnum)+'.'+str(sampletype)+'.recon.'+str(recotype)+'.'+str(release)+'.aan.root' 
#-------------------------------------------------------------- 
# histograms
#-------------------------------------------------------------- 
if os.path.exists(RootTupleOutputFileName):
   os.remove(RootTupleOutputFileName)

#   Root Ntuple output file and name
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
svcMgr.MessageSvc.useColors = True
#==============================================================
# Number of Events to process
theApp.EvtMax = -1
###################### Detail time measurement and auditors
# Use auditors
 
#theApp.AuditAlgorithms=True
#theApp.Dlls += [ "GaudiAud" ]
 
#from AthenaCommon.AppMgr import theAuditorSvc
#from AthenaCommon.ConfigurableDb import getConfigurable
#theAuditorSvc += getConfigurable("ChronoAuditor")()
 
AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

#-----------------------
# And some final options
#-----------------------
from GaudiSvc.GaudiSvcConf import AuditorSvc
theAuditorSvc = svcMgr.AuditorSvc
theApp.AuditAlgorithms=True  
theApp.AuditServices=True
theApp.AuditTools=True  

from AthenaCommon import CfgMgr
theAuditorSvc += CfgMgr.AlgErrorAuditor()
theAuditorSvc += CfgMgr.ChronoAuditor()
theAuditorSvc += CfgMgr.NameAuditor()
theAuditorSvc += CfgMgr.MemStatAuditor()
 
if doMuonRecValidationAODRTT:
   from PerfMonComps.PerfMonFlags import jobproperties
   #by default, PerfMon is disabled: enable it    
   #see https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
   jobproperties.PerfMonFlags.doMonitoring = True
   jobproperties.PerfMonFlags.doPersistencyMonitoring = True
   jobproperties.PerfMonFlags.doDetailedMonitoring = True
   jobproperties.PerfMonFlags.doFastMon = False
   jobproperties.PerfMonFlags.OutputFile = "MuonRecValidationAOD_RTT.pmon.gz"
   include( "PerfMonComps/PerfMonSvc_jobOptions.py" )
   from PerfMonComps.JobOptCfg import PerfMonSvc
   svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )
