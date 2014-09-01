###############################################################
#
# Job options file
#
## @file LArCellContReader_jobOptionsRegStep1.py
##
## @brief For Athena POOL test: read in LArCellCont
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
# Define the output Db parameters (the default value are shown)
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.CollectionType    =  "ExplicitROOT" 
EventSelector.InputCollections  = [ "NewEventCollection1" ]
#EventSelector.RefName           = "ESD_ref" 
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application: AthenaPoolTest options
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
theApp.TopAlg   += [ "LArCellContFakeReader", "AthenaPoolTestAttrReader" ]


#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "CaloAthenaPool/CaloAthenaPool_joboptions.py" )
#include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = INFO
#AthenaPoolTestAttrReader = Algorithm ( "AthenaPoolTestAttrReader" )
#AthenaPoolTestAttrReader.OutputLevel = DEBUG
LArCellContFakeReader = Algorithm( "LArCellContFakeReader" )
LArCellContFakeReader.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
