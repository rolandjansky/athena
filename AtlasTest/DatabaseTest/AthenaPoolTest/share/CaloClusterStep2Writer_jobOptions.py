###############################################################
#
# Job options file
#
## @file CaloClusterStep2Writer_jobOptions.py
##
## @brief For Athena POOL test: write out CaloClusterContainers to
## a second file with ElemenetLinks to LArCellContainers in first file
##
## @author RD Schaffer <R.D.Schaffer@cern.ch>
#
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

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
# Define input
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections        = [ "SimplePoolFile1.root" ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Application: AthenaPoolTest InDetRawData options
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
#theApp.TopAlg   += [ "LArCellContFakeWriter" ]
theApp.TopAlg   += [ "CaloClusterFakeWriter" ]

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include( "CaloAthenaPool/CaloAthenaPool_joboptions.py" )
#include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

# Run OutputStream as an algorithm
theApp.TopAlg   += [ "AthenaOutputStream/Stream1" ]
# Must make sure that no OutStream's have been declared
theApp.OutStream    = []; 
# Stream's output file
Stream1 = Algorithm( "AthenaOutputStream/Stream1" )
Stream1.WritingTool = "AthenaOutputStreamTool"
Stream1.OutputFile  = "SimplePoolFile2.root"
# List of DO's to write out
#Stream1.ItemList += ["CaloCellContainer#*"]
Stream1.ItemList += ["CaloClusterContainer#*"]
Stream1.ItemList += ["CaloCellLinkContainer#*"]
Stream1.ItemList += ["CaloShowerContainer#*"]
Stream1.ItemList += ["EventInfo#*"]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.debugLimit  = 100000
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = INFO
#LArCellContFakeWriter = Algorithm( "LArCellContFakeWriter" )
#LArCellContFakeWriter.OutputLevel = DEBUG
CaloClusterFakeWriter = Algorithm( "CaloClusterFakeWriter" )
CaloClusterFakeWriter.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
