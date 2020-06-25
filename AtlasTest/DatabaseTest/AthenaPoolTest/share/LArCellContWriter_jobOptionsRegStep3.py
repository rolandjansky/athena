###############################################################
#
# Job options file
#
# Read AthenaPoolData objects and write out registration
#
## @file LArCellContWriter_jobOptionsRegStep2.py
##
## @brief For Athena POOL test:  write out explicit collection with LArCellCont
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
# For general flags
doAOD     = False
doTrigger = False
DetDescrVersion = "ATLAS-CSC-01-01-00"
include( "RecExCond/RecExCommon_flags.py" )

# Set local flags - only need LAr DetDescr
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOff()
      
# set up all detector description description 
include ("RecExCond/AllDet_detDescr.py")

#--------------------------------------------------------------
# Define the input parameters
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = [ "SimplePoolFile1.root" ]

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest", "RegistrationServices" ]
theApp.TopAlg   += [ "LArCellContFakeWriter", "AthenaPoolTestAttrWriter" ]

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "CaloAthenaPool/CaloAthenaPool_joboptions.py" )
#include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Event Collection Registration
#--------------------------------------------------------------
# Run OutputStream as an algorithm
theApp.TopAlg   += [ "RegistrationStream/RegStream1" ]
RegStream1 = Algorithm( "RegStream1" )
RegStream1.CollectionType   = "ExplicitROOT" 
RegStream1.OutputCollection = "NewEventCollection2"
RegStream1.WriteInputDataHeader = TRUE 
# print level
RegStream1.OutputLevel = 3
# List of DO's to register
RegStream1.ItemList += [ "DataHeader#*" ]
#RegStream1.ItemList += [ "DataHeader#*" ]
# Key name of AthenaAttributeList used for the tag:
RegStream1.ItemList += [ "AthenaAttributeList#SimpleTag" ]
# Add in ref to input file with name:
RegStream1.ExtraRefNames = [ "ESD" ]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = INFO
AthenaPoolTestAttrWriter = Algorithm ( "AthenaPoolTestAttrWriter" )
AthenaPoolTestAttrWriter.OutputLevel = DEBUG
LArCellContFakeWriter = Algorithm( "LArCellContFakeWriter" )
LArCellContFakeWriter.OutputLevel = DEBUG
StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE 

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
