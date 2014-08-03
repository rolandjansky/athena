# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This is the job options that does the merging

###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 1000

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm"
#top algorithms to be run, and the libraries that house them

DetDescrVersion="ATLAS-CSC-01-02-00"
include ("RecExCommon/AllDet_detDescr.py")

# Pool Converters for AOD
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

doTrigger=True
doWriteAOD = True
doTruth = True

#
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
EventSelector = Service( "EventSelector" )
include(mergeFile)
#==============================================================
#
# End of job options file
#
###############################################################
#--------------------------------------------------------------
#---   Secondary Write portion  ----- Don't change it !!!
#--------------------------------------------------------------
theApp.OutStream     =["Stream1"]
theApp.OutStreamType ="AthenaOutputStream"
Stream1 = Algorithm( "Stream1" )
Stream1.Output = OUTPUT
Stream1.EvtConversionSvc     ="AthenaPoolCnvSvc"
PoolSvc = Service( "PoolSvc" )
Stream1.ForceRead=TRUE;  #force read of output data objs

# list of output objects 
include( "ParticleBuilderOptions/AOD_OutputList_jobOptions.py")
Stream1.ItemList = AOD_ItemList


