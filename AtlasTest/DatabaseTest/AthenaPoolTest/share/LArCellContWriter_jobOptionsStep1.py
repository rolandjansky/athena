###############################################################
#
# Job options file
#
## @file LArCellContWriter_jobOptionsStep1.py
##
## @brief For Athena POOL test: write out first file with LArCellContainers
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
# MC Event Selector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

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
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application: 
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
theApp.TopAlg   += [ "LArCellContFakeWriter" ]


#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "CaloAthenaPool/CaloAthenaPool_joboptions.py" )
#include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

# Stream's output file
Stream1 = Algorithm( "Stream1" )
Stream1.OutputFile =   "SimplePoolFile1.root"
# List of DO's to write out
Stream1.ItemList+=["EventInfo#*"]
Stream1.ItemList+=["CaloCellContainer#*"]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = INFO
LArCellContFakeWriter = Algorithm( "LArCellContFakeWriter" )
LArCellContFakeWriter.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
