###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# POOL related services.
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

PoolSvc = Service( "PoolSvc" )
PoolSvc.WriteCatalog = "xmlcatalog_file:PoolFileCatalog.xml"

#--------------------------------------------------------------
# Event selector.
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = [
    "AOD.pool.root"
    ]

EventSelector.Query = "n_electron > 2 && e_electron_1 > 20000 && e_electron_2 > 18000"

#--------------------------------------------------------------
# POOLconverters
#--------------------------------------------------------------
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )
include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
include( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include( "ParticleEventAthenaPool/ParticleEventAthenaPool_joboptions.py" )
include( "AnalysisTriggerEventAthenaPool/AnalysisTriggerEventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Load libraries and declare top algorithms.
#--------------------------------------------------------------
from EventTagAlgs.EventTagGlobal import EventTagGlobal
EventTagGlobal.AttributeList = "SimpleTag"

include( "EventTagAlgs/EventTag_jobOptions.py" )

#--------------------------------------------------------------
# Event collection registration.
#--------------------------------------------------------------
# Run OutputStream as an algorithm
theApp.TopAlg   += [ "RegistrationStream/RegStream1" ]
RegStream1 = Algorithm( "RegStream1" )
RegStream1.OutputCollection = "EventTagCollection"
RegStream1.Connection = ""
#RegStream1.Connection = "mysql://CollTester:CtAl_204@atlasdbdev.cern.ch/CollectionTest"
#RegStream1.Connection = "mysql://athena:insider@db1.usatlas.bnl.gov/pool_collection"
# List of DO's to register
RegStream1.ItemList += [ "DataHeader#*" ]
# Key name of AthenaAttributeList used for the tag:
RegStream1.ItemList += [ "AthenaAttributeList#SimpleTag" ]

#-------------------------------------------------------------
# Event related parameters.
#-------------------------------------------------------------
theApp.EvtMax = 20

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 1
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 3
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = TRUE 

#==============================================================
#
# End of job options file
#
###############################################################
