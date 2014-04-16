#--------------------------------------------------------------
# ReadMuonDigitTestOptions.py
#--------------------------------------------------------------
include( "AthenaCommon/AthenaCommonFlags.py" )
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOn()
#--------------------------------------------------------------
# AthenaPool converters
#--------------------------------------------------------------
#Pool Cnv
include("MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py")
#--------------------------------------------------------------
#Pool input
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
#--------------------------------------------------------------
# EventInfo Converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
#--------------------------------------------------------------
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
#--------------------------------------------------------------
#to initialize converters for TracRecord
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )
#--------------------------------------------------------------
include( "MdtCalibSvc/MdtCalibrationSvc_jobOptions.py" )
#--------------------------------------------------------------
# Options for reading from POOL (use POOL implicit collection)
#--------------------------------------------------------------
# Pool input
EventSelector = Service( "EventSelector" )
#--------------------------------------------------------------
# Options for reading from POOL (use POOL implicit collection)
#--------------------------------------------------------------
# Pool input
EventSelector = Service( "EventSelector" )

##For nominal geometry and new magnetic field, nominal
EventSelector.InputCollections += [ "rfio:/castor/cern.ch/user/m/muonprod/1203/digit_nom_mf_geom/mc12.007211.digit_nom_mf_geom.mu_pt10GeV/mc12.007211.digit_nom_mf_geom.mu_pt10GeV._00001.pool.root", 
]
#-------------------------------------------------
Stream1 = Algorithm( "Stream1" )
Stream1.OutputLevel    = 2
#-------------------------------------------------
################ Detail time measurement and auditors
# write out a summary of the time spent
theApp.Dlls += [ "GaudiAlg","GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor" ]
# Use auditors
theApp.AuditAlgorithms=True
#-------------------------------------------------
# For the cond DB
include("IOVDbSvc/IOVRecExCommon.py") 
#-------------------------------------------------
# job opts to activate the LSF-based TimeKeeper
include ("LSFTimeKeeper/LSFTimeKeeperOptions.py")
LSFTimeKeeperSvc= Service("LSFTimeKeeperSvc")
LSFTimeKeeperSvc.OutputLevel=DEBUG
#-------------------------------------------------
AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE
#--------------------------------------------------------------
#--- GeoModel stuff -------------------------------------------
# MuonGeoModel Initialization
#for nominal geometry and new magnetic field, nominal
# 'ATLAS-CSC-01-00-00' --> same as ATLAS-CSC-00-00-00 but using bmagatlas04_test1.data
# MuonGeoModel Initialization
Layout = 'R.02.01.Initial'
include ( "DetDescrCnvSvc/DetStore_joboptions.py" )
DetDescrVersion='ATLAS-CSC-01-00-00'
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )
GeoModelSvc = Service( "GeoModelSvc")
GeoModelSvc.IgnoreTagDifference = True
#--------------------------------------------------------------
# AmdcAth_jobOptions needs to come after MuonGeoModel
include ("AmdcAth/AmdcAth_jobOptions.py")
#--------------------------------------------------------------
# Convert Digits to RDOs
# job options fragments for the decoding :: muon digits --> muon RDO! --->PRD - Dll's
MuonLayout = Layout
include( "MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py" )
include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )
#include( "CscClusterization/G4CscClusterOptions.py" )
#include( "RpcClusterization/RpcClusterJobOptions.py" )
#-------------------------------------------------
include( "MuonDigitTest/MuonDigTests_options.py" )
#--------------------------------------------------------------
THistSvc = Service( "THistSvc")
#--------------------------------------------------------------
# setup ROOT TTree registration svc
THistSvc.Output += ["MuonDigits DATAFILE='MuonDigitsVal_ideal0_mupt10GeV.aan.root' OPT='NEW'"]
#--------------------------------------------------------------
### writing the AANTuple
#--------------------------------------------------------------
#theApp.Dlls   += [ "AnalysisTools" ]
#theApp.TopAlg += [ "AANTupleStream" ]
#AANTupleStream = Algorithm( "AANTupleStream" )
#AANTupleStream.OutputName = 'MuonDigitsVal.aan.root'
#AANTupleStream.WriteInputDataHeader = True
#AANTupleStream.OutputLevel = 3
#--------------------------------------------------------------
# Configuration of MDTDigitsTestAlg properties
#--------------------------------------------------------------
MDTDigitsTestAlg = Algorithm( "MDTDigitsTestAlg" )
#event SVC key
MDTDigitsTestAlg.McEventKey = "TruthEvent"
MDTDigitsTestAlg.WriteNtuple = True
MDTDigitsTestAlg.MDTDigitsTestAlg = True
#--------------------------------------------------------------
# Configuration of RPCDigitsTestAlg properties
#--------------------------------------------------------------
RPCDigitsTestAlg = Algorithm( "RPCDigitsTestAlg" )
#event SVC key
RPCDigitsTestAlg.McEventKey = "TruthEvent"
RPCDigitsTestAlg.WriteNtuple = True
#--------------------------------------------------------------
# Configuration of CSCDigitsTestAlg properties
#--------------------------------------------------------------
CSCDigitsTestAlg = Algorithm( "CSCDigitsTestAlg" )
#event SVC key
CSCDigitsTestAlg.McEventKey = "TruthEvent"
CSCDigitsTestAlg.WriteNtuple = True
#--------------------------------------------------------------
# Configuration of TGCDigitsTestAlg properties
#--------------------------------------------------------------
TGCDigitsTestAlg = Algorithm( "TGCDigitsTestAlg" )
#event SVC key
TGCDigitsTestAlg.McEventKey = "TruthEvent"
TGCDigitsTestAlg.WriteNtuple = True
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 3
MessageSvc.defaultLimit = 9999999
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000
theApp.EvtSel = "EventSelector"
#--------------------------------------------------------------
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT" 
#--------------------------------------------------------------
# Application: load TagInfoMgr
#--------------------------------------------------------------
include( "EventInfoMgt/EventInfoMgt_jobOptions.py" )
TagInfoMgr = Service( "TagInfoMgr" )
TagInfoMgr.OutputLevel = 2

print "List of all top algorithms"
print theApp.TopAlg
#================================================================
 
 
