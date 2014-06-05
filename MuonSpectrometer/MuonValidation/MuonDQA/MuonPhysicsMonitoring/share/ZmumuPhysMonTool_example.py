#==============================================================
###The setup is done via
###export STAGE_HOST=castoratlast3
###export STAGE_SVCCLASS=atlascernuserdisk
#==============================================================
# Job options file for Zmumu Physics Monitoring
# April 2008
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
#--------------------------------------------------------------
# detector description version: both RDO and BS default use this
#--------------------------------------------------------------
DetDescrVersion = "ATLAS-GEO-02-01-00"  #Jpsi
#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------
doReadBS = False
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
#Globalflags.DetGeo = 'atlas'
# --- set defaults
#globalflags.DataSource = 'geant4'
#globalflags.InputFormat = 'pool'

# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

#--- Detector flags -------------------------------------------
# - Select detectors
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.Calo_setOn()
from AthenaCommon.GlobalFlags import globalflags as GlobalFlags
include ("RecExCond/AllDet_detDescr.py")

# --- printout
DetFlags.Print()

from RecExConfig.RecFlags import rec
rec.doInDet=True
rec.doMuon=True
rec.doCalo=True
rec.doEgamma=True
rec.doMuonCombined=True
rec.doJetMissingETTag=False
rec.doTau=False
rec.doTruth = (globalflags.InputFormat() == 'pool')

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
svcMgr.EventSelector.InputCollections += [ '/afs/cern.ch/user/m/muoncomm/w0/MuonDQA/giannis/user.RichardHawkings.0108172.topmix_Muon.AOD.v1._00622.pool.root' ]
 
svcMgr.EventSelector.BackNavigation = False 
print "Dumping the list of files to be used as input"
print svcMgr.EventSelector.InputCollections
#==============================================================

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "PrimaryManager" )

# ************************
# Monitoring configuration
# ************************

## Setup the output file(s):
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
## The string "TestMon" in the argument below is the 'FileKey'
## used by Athena to access the output file internally
svcMgr.THistSvc.Output += ["MuonMon DATAFILE='MuonMon.root' OPT='RECREATE'"]
 
if not 'DQMonFlags' in dir():
   print "DataQualityMon_RecExCommon_Flags_jobOptions.py: DQMonFlags not yet imported - I import them now"
   from AthenaMonitoring.DQMonFlags import DQMonFlags
   from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
muonOutputLevel=INFO
## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
muPhysMonMan = topSequence.PrimaryManager
include ("MuonPhysicsMonitoring/MuonPhysicsDQA_options.py")
  
## FileKey must match that given to THistSvc
muPhysMonMan.FileKey = "MuonMon"
 
## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
muPhysMonMan.ManualDataTypeSetup = True
muPhysMonMan.DataType            = "monteCarlo"
muPhysMonMan.Environment         = "Tier0"
muPhysMonMan.ManualRunLBSetup    = True
muPhysMonMan.Run                 = 1
muPhysMonMan.LumiBlock           = 1

#==============================================================
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel = INFO
#==============================================================
# Number of Events to process
theApp.EvtMax = -1
theApp.EvtSel = "EventSelector" 
#==============================================================
###################### Detail time measurement and auditors
# Use auditors
svcMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]
AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

print "List of all top algorithms"
print theApp.TopAlg

#================================================================  

# ------------------------
# SET UP VKALVRT
# ------------------------
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
# Temporary hack related to magnetic field
TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(name = "BPhysVKalFitter", MagFieldSvc = "DefaultVKalField" )
ToolSvc += TrkVKalVrtFitter


# ##########################################
# ## Set up trigger configuration service and metadata service is relies on, for analysis job without RecExCommon

# #from AthenaCommon.GlobalFlags import GlobalFlags
# #GlobalFlags.DetGeo.set_atlas()

# ## set up trigger decision tool
# from TrigDecision.TrigDecisionConfig import TrigDecisionTool
# tdt = TrigDecisionTool()
# tdt.UseFDRMode = True#False
# ToolSvc += tdt

# # might be needed for TriggerConfigGetter...
# from RecExConfig.RecFlags  import rec
# rec.readAOD=True
# rec.doWriteAOD=False
# rec.doWriteESD=False

# # To read files with trigger config stored as in-file meta-data,
# # i.e. 13.0.40 and above: ds
# # To read AOD produced with 13.0.30 you need to change ds to aod:
# from TriggerJobOpts.TriggerFlags import TriggerFlags
# TriggerFlags.configurationSourceList = ['ds']

# # set up trigger config service
# from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
# cfg =  TriggerConfigGetter()

# ## END of trigger setup
# #################################################
