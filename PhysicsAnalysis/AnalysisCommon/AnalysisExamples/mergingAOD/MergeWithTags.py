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

include( "PartPropSvc/PartPropSvc.py" )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 1000

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm"
#top algorithms to be run, and the libraries that house them

#
include( "EventTagAlgs/EventTag_jobOptions.py" )

# Pool Converters for AOD
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py" )

#
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
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

# list of output objects 
include( "ParticleEventAthenaPool/AOD_OutputList_jobOptions.py")

#theApp.OutStream     =["OutMerge"]
#theApp.OutStreamType ="AthenaOutputStream"
theApp.TopAlg  += ["AthenaOutputStream/OutMerge"]
OutMerge        = Algorithm( "OutMerge" )
OutMerge.Output = OUTPUT+".pool.root"
OutMerge.EvtConversionSvc = "AthenaPoolCnvSvc"
OutMerge.ItemList  = AOD_ItemList
OutMerge.ForceRead = TRUE;  #force read of output data objs
OutMerge.ExtendProvenanceRecord = TRUE

cName = "Rome"
theApp.TopAlg += [ "RegistrationStream/"+cName ]
Coll0 = Algorithm( cName )
Coll0.CollectionType   = "ExplicitROOT"
Coll0.OutputCollection = OUTPUT+".coll"
#Coll0.WriteInputDataHeader = TRUE
Coll0.ItemList   += [ "DataHeader#*" ]
Coll0.ItemList   += [ "AthenaAttributeList#SimpleTag" ]
Coll0.ExtraRefNames = [ "ESD" ]

