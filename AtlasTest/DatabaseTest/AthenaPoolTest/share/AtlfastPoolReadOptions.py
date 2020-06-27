###############################################################
#
# Job options file for running simpleTest
#
## @file AtlfastPoolReadOptions.py
##
## @brief For Athena POOL test: read back Atlfast objects
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
#
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

EventSelector = Service( "EventSelector" )
EventSelector.InputCollections        = [ "SimplePoolFile.root" ]
#--------------------------------------------------------------
# Other Dlls and services
#--------------------------------------------------------------
##include "AtlfastAlgs/GeneratorOptions_zebra_default.txt"
include( "PartPropSvc/PartPropSvc.py" )

theApp.Dlls += [ "GaudiAlg" ]	
theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  = [ "ChronoAuditor" ]
theApp.ExtSvc += ["AtRndmGenSvc"]
theApp.Dlls += [ "AtlfastAlgs","TestAtlfast"]
theApp.Dlls   += [ "AthenaPoolTest" ]
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Execution of algorithms given in sequencer
#
theApp.TopAlg = ["Sequencer/TopSequence"]
TopSequence = Algorithm( "TopSequence" )
TopSequence.Members = ["Sequencer/Generator","Sequencer/Atlfast"]
##include "AtlfastAlgs/AtlfastStandardOptions.txt"
include( "AtlfastAlgs/AtlfastDoNotTouch.py" )

#set up individual jobOptions
ElectronMaker = Algorithm( "ElectronMaker" )
ElectronMaker.DoSmearing		 = FALSE
MuonMaker = Algorithm( "MuonMaker" )
MuonMaker.DoSmearing                     = FALSE
PhotonMaker = Algorithm( "PhotonMaker" )
PhotonMaker.DoSmearing			 = FALSE
JetMaker = Algorithm( "JetMaker" )
JetMaker.DoSmearing       	         = FALSE
TrackMaker = Algorithm( "TrackMaker" )
TrackMaker.DoSmearing       	         = FALSE
GlobalEventDataMaker = Algorithm( "GlobalEventDataMaker" )
GlobalEventDataMaker.Luminosity		=  2	
ClusterMaker = Algorithm( "ClusterMaker" )
ClusterMaker.ProcessCells       = TRUE
ClusterMaker.ProcessTracks      = FALSE
ClusterMaker.Strategy           = "cone"
#--------------------------------------------------------------
# AtlfastPoolTest (values copied from
# AtlfastStandardOption.txt for CellMaker)
#-----------------------------------
Atlfast = Algorithm( "Atlfast" )
Atlfast.members += ["Atlfast::AtlfastPoolTest/AtlfastPoolTest" ]
AtlfastPoolTest = Algorithm( "AtlfastPoolTest" )
AtlfastPoolTest.EtaCoverage        = 5.0
AtlfastPoolTest.MinETCell          = 0.0*GeV
AtlfastPoolTest.GranBarrelEta      = 0.1
AtlfastPoolTest.GranBarrelPhi      = 0.1
AtlfastPoolTest.GranForwardEta     = 0.2
AtlfastPoolTest.GranForwardPhi     = 0.2
AtlfastPoolTest.FastShower         = FALSE
AtlfastPoolTest.InputLocation      = "/Event/McEventCollection"
AtlfastPoolTest.OutputLocation     = "/Event/AtlfastPoolTestCellsNew"; 
AtlfastPoolTest.OutputLevel = 2
Atlfast.members += ["Atlfast::IKinematicDumper/IKinematicDumper" ]
IKinematicDumper = Algorithm( "IKinematicDumper" )
IKinematicDumper.cellLocation     = "/Event/AtlfastPoolTestCells"
#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------
include( "FourMom/FourMomDict_joboptions.py" )

# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "AtlfastAthenaPool/AtlfastAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 3
StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE 

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
