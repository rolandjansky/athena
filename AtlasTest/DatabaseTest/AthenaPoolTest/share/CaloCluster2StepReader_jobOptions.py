###############################################################
#
# Job options file
#
## @file CaloCluster2StepReader_jobOptions.py
##
## @brief For Athena POOL test: read back CaloClusterContainers and
## LArCellContainers using element links from across two files
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
# Define input
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections        = [ "SimplePoolFile2.root" ]
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20
#--------------------------------------------------------------
# Application: AthenaPoolTest InDetRawData options
#--------------------------------------------------------------
theApp.Dlls     += [ "AthenaPoolTest" ]
theApp.TopAlg   += [ "CaloClusterFakeReader" ]

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
CaloClusterFakeReader = Algorithm( "CaloClusterFakeReader" )
CaloClusterFakeReader.OutputLevel = DEBUG

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
