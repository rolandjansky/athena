#-------------------------------------------------
# ReadMuonDigitsFromRDOFileOptions.py
#-------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Muon_setOn() 
#-------------------------------------------------
# AthenaPool converters
#-------------------------------------------------
#-------------------------------------------------
#Pool Cnv
include("MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py")
#-------------------------------------------------
#Pool input
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
#-------------------------------------------------
# EventInfo Converters
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
#-------------------------------------------------
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
#-------------------------------------------------
#to initialize converters for TracRecord
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" ) 
#------------------------------------------------- 
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
EventSelector.InputCollections += [
                                    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007231.digit.mu_pt50GeV/mc12.007231.digit.mu_pt50GeV._00001.pool.root", 
				    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007211.digit.mu_pt10GeV/mc12.007211.digit.mu_pt10GeV._00003.pool.root",
                                    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007211.digit.mu_pt10GeV/mc12.007211.digit.mu_pt10GeV._00004.pool.root",
                                    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007211.digit.mu_pt10GeV/mc12.007211.digit.mu_pt10GeV._00005.pool.root",
                                    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007211.digit.mu_pt10GeV/mc12.007211.digit.mu_pt10GeV._00006.pool.root",
                                    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007211.digit.mu_pt10GeV/mc12.007211.digit.mu_pt10GeV._00007.pool.root",
                                    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007211.digit.mu_pt10GeV/mc12.007211.digit.mu_pt10GeV._00008.pool.root",
                                    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007211.digit.mu_pt10GeV/mc12.007211.digit.mu_pt10GeV._00009.pool.root",
                                    "rfio:/castor/cern.ch/atlas/preprodtests/1200/digit/mc12.007211.digit.mu_pt10GeV/mc12.007211.digit.mu_pt10GeV._00010.pool.root"  
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
# job opts to activate the LSF-based TimeKeeper
include ("LSFTimeKeeper/LSFTimeKeeperOptions.py")
LSFTimeKeeperSvc= Service("LSFTimeKeeperSvc")
LSFTimeKeeperSvc.OutputLevel=DEBUG
#------------------------------------------------- 
AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
theApp.TopAlg   += [ "EventInfoReader" ]
include( "EventInfoMgt/EventInfoMgt_jobOptions.py" )
#--- GeoModel stuff -------------------------------------------
# MuonGeoModel Initialization 
#---- for P.03 layout
#Layout = "Q02_initial_pro"
include ( "DetDescrCnvSvc/DetStore_joboptions.py" )
DetDescrVersion = "ATLAS-DC3-06"
include ( "AtlasGeoModel/GeoModelCommon.py" )
include ( "AtlasGeoModel/MuonGeoModel.py" )
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.Detectors += [ "MuonDetectorTool" ]
GeoModelSvc.IgnoreTagDifference = True
#GeoModelSvc.MuonDetectorTool.LayoutName = Layout
#GeoModelSvc.MuonDetectorTool.IncludeInertMaterials = 1
#GeoModelSvc.MuonDetectorTool.LightGeometry_only =0

DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_Q.02.xml"
DetDescrCnvSvc.DecodeIdDict = TRUE
include( "AmdcAth/AmdcAth_jobOptions.py" )

# converters for RDOs -> Ditits
#MuonLayout = Layout
include( "MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py" )
include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )

#--------------------------------------------------------------
# Dump the digits
#--------------------------------------------------------------
ReadMuonDigits = Algorithm( "ReadMuonDigits" )
ReadMuonDigits.isGeoModel = TRUE
theApp.TopAlg += ["ReadMuonDigits"]
ReadMuonDigits.DumpMDTDigits = TRUE
ReadMuonDigits.DumpRPCDigits = TRUE
ReadMuonDigits.DumpTGCDigits = TRUE
ReadMuonDigits.DumpCSCDigits = TRUE
 
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 3
# Number of events to be processed (default is 10)
theApp.EvtMax = 50
theApp.EvtSel = "EventSelector"
EventInfoReader = Algorithm( "EventInfoReader" )
EventInfoReader.OutputLevel = 2
TagInfoMgr = Service( "TagInfoMgr" )
TagInfoMgr.OutputLevel = 2

print "List of all top algorithms"
print theApp.TopAlg
#================================================================
 
 

