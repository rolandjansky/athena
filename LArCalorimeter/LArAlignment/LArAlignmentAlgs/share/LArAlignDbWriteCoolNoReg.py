###############################################################
#
# Job options file for LArAlignDbAlg:  see doc in README
#
#==============================================================
#use McEventSelector
theApp.setup( MONTECARLO )

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
#include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

theApp.Dlls += [ "LArAlignmentAlgs" ]

# include the converters
include ( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

#DO NOT USE UNTIL ONE INITS SERVICES AUTOMATICALLY WHEN CALLED:
#  ApplicationMgr.ExtSvc += { "IOVSvc", "IOVDbSvc" }
theApp.TopAlg = [ "LArAlignDbAlg" ]
#/--------------------------------------------------------------
#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
LArAlignDbAlg = Algorithm( "LArAlignDbAlg" )
LArAlignDbAlg.OutputLevel      = 2


#IOVDbSvc = Service( "IOVDbSvc" )
#IOVDbSvc.OutputLevel      = 2

# Set time to register - used for IOVDbTestAmdbCorrection
LArAlignDbAlg.RegTime       = 0

#StoreGateSvc.OutputLevel    = 2

#--------------------------------------------------------------
# For IOVDb: specify dbConnection with COOL and oracle specified
#
#   Default is to use oracle schema named TESTCOOL, to override this use
#     athena  'TESTCOOLDB="<your test db name>"' LArAlignDbAlgWriteCool.py
#
#try:
#    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:"
#    IOVDbSvc.dbConnection  += TESTCOOLDB
#    IOVDbSvc.dbConnection  += ":ATLAS_COOLTEST:pwd4cool"
#except:
#    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOLTEST:pwd4cool"

#--------------------------------------------------------------
# Choose the following to write out cond objects . 
LArAlignDbAlg.WriteCondObjs = True
# Choose the following to register cond objects in the IOVDB. 
LArAlignDbAlg.RegisterIOV   = False
# Do not register AttributeList - only for cool version
LArAlignDbAlg.ReadWriteCool = True

#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE"
# Number of events to be processed (default is 10)
#ApplicationMgr.EvtMax = 1
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber         = 1
EventSelector.EventsPerRun      = 2
EventSelector.FirstEvent        = 1
EventSelector.InitialTimeStamp  = 0
EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 5

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Define the output Db parameters (the default value are shown)
#--------------------------------------------------------------
PoolSvc = Service( "PoolSvc" )
# PoolSvc.DbServer  =  "db1.usatlas.bnl.gov"
# PoolSvc.DbAccount =  "athena"
# PoolSvc.DbPassword = ""
# PoolSvc.DbType    = "mysql"
# PoolSvc.ConnectionType = "MySQLCollection"
# PoolSvc.FullConnection = "mysql://athena:insider@db1.usatlas.bnl.gov/pool_collection"
#PoolSvc.Collection = "NewPoolTry"
#PoolSvc.DbOpen = "overwrite" // "read", "update" "create" "overwrite"
#PoolSvc.DbOpen = "update" # "read", "update" "create" "overwrite"

#--------------------------------------------------------------
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
#--------------------------------------------------------------

theApp.OutStream = []

ToolSvc = Service( "ToolSvc" )
ToolSvc.CondStream1.OutputFile = "LArAlignmentFile.root"



