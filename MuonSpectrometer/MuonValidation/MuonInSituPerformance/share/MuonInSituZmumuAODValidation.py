#==============================================================
#
# Job options file for ZmumuValidation
#
# MuonValidationTeam
# July 2007
#==============================================================
## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#--------------------------------------------------------------
# Particle properties
#--------------------------------------------------------------
# Not needed for Roma analysis
from PartPropSvc.PartPropSvcConf import PartPropSvc 
svcMgr += PartPropSvc()
#==============================================================

#--- Detector flags -------------------------------------------
# - Select detectors
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
#==============================================================
useSTACO = True
#==============================================================
#--------------------------------------------------------------
# The POOL converters
#--------------------------------------------------------------
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" ) 
#============================================================== 
# Options for reading from POOL (use POOL implicit collection)
#==============================================================  
svcMgr.EventSelector.InputCollections += [ 
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00085.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00219.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00379.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00363.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00083.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00157.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00263.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00291.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00111.pool.root.4",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00074.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00336.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00131.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00094.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00247.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00214.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00345.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00232.pool.root.4",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00346.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00015.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00038.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00241.pool.root.2",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00381.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00029.pool.root.4",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00279.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00323.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00123.pool.root.3",
"rfio:/castor/cern.ch/user/m/mschott/testarea/Zmumu/Zmumu_4_AOD.023533._00359.pool.root.2" 
]
svcMgr.EventSelector.BackNavigation = False 
print "Dumping the list of files to be used as input"
print svcMgr.EventSelector.InputCollections
#==============================================================
# The user analysis algorithm to be executed
if not "AnalysisTools" in theApp.Dlls:
   theApp.Dlls += ["AnalysisTools"]

if not "UserAnalysis" in theApp.Dlls:
   theApp.Dlls += ["UserAnalysis"]

if not "McParticleTools" in theApp.Dlls:
   theApp.Dlls += ["McParticleTools"]

if not "MuonInSituPerformance" in theApp.Dlls:
   theApp.Dlls += ["MuonInSituPerformance"]
#============================================================== 
from MuonInSituPerformance.MuonInSituPerformanceConf import ZmumuValidator
ZmumuValidator=ZmumuValidator(name = "ZmumuValidator")
ZmumuValidator.OutputLevel = INFO

# Trigger_Level_3_MS   Muon Spectrometer standalone track (non energy loss recovery applied)
# Trigger_Level_3_MV   Muid StandAlone track (MS vertex extrapolated with energy loss correction)
# Trigger_Level_3_CB   Muid Combined track (MS+ID fitted)

ZmumuValidator.TriggerContainer += [ "Trigger_Level_3_MS" ]
ZmumuValidator.TriggerContainer += [ "Trigger_Level_3_MV" ]
ZmumuValidator.TriggerContainer += [ "Trigger_Level_3_CB" ]

# The user analysis cuts - default values to changed by the user as needed
#ZmumuValidator.MaximalConeDistance = 0.05
#ZmumuValidator.MinimalMuonPt = 5000.

### Run on AOD files
ZmumuValidator.ConvertFromESD = False

#==============================================================
# For AOD, track Containers name according to the 
# https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD#Track_Particles_TrackParticle 
# wiki page
# For ESD, track Containers name according to the 
# https://uimon.cern.ch/twiki/bin/view/Atlas/StoregateKeysForESD wiki page 
#==============================================================
# Track Containers name according to the
# https://twiki.cern.ch/twiki/bin/view/Atlas/StoregateKeysForAOD#Loosely_Identified_Particles_Par
# wiki page
#============================================================== 
if useSTACO:
  ZmumuValidator.MuonContainer = "StacoMuonCollection"
  ZmumuValidator.MuonSpectrometerContainer = "MuonboyTrackParticles" # for TrackParticle Container in MuonStandAlone, no MuonContainer
  ZmumuValidator.InnerTrackContainer ="TrackParticleCandidate"    
else:
  ZmumuValidator.MuonContainer = "MuidMuonCollection" # for MuonContainer
  ZmumuValidator.MuonSpectrometerContainer = "MooreTrackParticles" # for TrackParticle Container in MuonStandAlone, no MuonContainer
  ZmumuValidator.InnerTrackContainer ="TrackParticleCandidate" 
   
ZmumuValidator.MuonContainer = "MuidMuonCollection"
ZmumuValidator.MuonSpectrometerContainer = "MooreTrackParticles"

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += ZmumuValidator 
print ZmumuValidator 

#==============================================================
sampletype='PythiaZmumu';
atlastag='mc08';
dstnum='0023533'
recotype='AOD'
RootTupleOutputFileName = 'MuonInSituPerformance_'+str(atlastag)+'.'+str(dstnum)+'.recon.'+str(recotype)+'.'+str(sampletype)+'.aan.root' 
#-------------------------------------------------------------- 
# histograms
#-------------------------------------------------------------- 
if os.path.exists(RootTupleOutputFileName):
   os.remove(RootTupleOutputFileName)
   
#============================================================== 
# Root Ntuple output file and name
ZmumuValidator.OutputFile = RootTupleOutputFileName
print '\n************************************************************\n'
print " \nUsing following names: "
print "   filename: ", RootTupleOutputFileName   
print '\n************************************************************\n' 
#==============================================================
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel = INFO
#==============================================================
# Number of Events to process
theApp.EvtMax = 1000
#==============================================================
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["AANT DATAFILE='%s' OPT='NEW'" % RootTupleOutputFileName]
 
#==============================================================
###################### Detail time measurement and auditors
# Use auditors
svcMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]
AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = TRUE
print "List of all top algorithms"
print theApp.TopAlg
#================================================================
  
 
