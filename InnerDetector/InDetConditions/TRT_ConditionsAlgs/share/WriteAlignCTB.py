
# options for writing AlignableTransforms to CondDB
# using the TRT_TestWrite algorithm
# The input is a text file (examples are found in this dir)
#
# Access to IOVDbSvc
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )


#include converters
include ( "InDetConditionsAthenaPool/InDetConditionsAthenaPool_joboptions.py" )
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
include ( "InDetRecExample/InDetPoolConvertors.py" )


#load relevant libraries
theApp.Dlls    += [ "TRT_ConditionsAlgs" ]
theApp.Dlls    += [ "InDetConditionsAthenaPoolPoolCnv" ]
theApp.Dlls    += [ "DetDescrCondAthenaPoolPoolCnv" ]
#top algorithm
theApp.TopAlg += [ "TRT_TestWrite" ]

#debug level
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 2000
MessageSvc.infoLimit        = 2000

#configure IOVRegistrationSvc
include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
regSvc = Service( "IOVRegistrationSvc" )
regSvc.RecreateFolders = False

#configure IOVDbSvc
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel       = 2
#
# connect to development server
#IOVDbSvc.dbConnection = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOLTEST:pwd4cool"
#
# to standalone sqlite db
IOVDbSvc.dbConnection = "impl=cool;techno=sqlite;schema=TrtAlign_07.db;DUMMY:COOLTEST"
#
# to testbeam production server
#IOVDbSvc.dbConnection = "impl=conddb;techno=mysql;atlmysql01.cern.ch:conditions_ctb_2004:condwriter:xxxxx"

# or do not connect any output db

# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
PoolSvc = Service( "PoolSvc" )
PoolSvc.DbOpen = "update"
#PoolSvc.WriteCatalog="file:Alignment_2102540_2102999.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102464_2102539.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102411_2102463.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102372_2102410.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102312_2102371.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102298_2102311.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102226_2102297.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102182_2102225.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102151_2102181.xml"
#PoolSvc.WriteCatalog="file:Alignment_2102121_2102150.xml"
#PoolSvc.WriteCatalog="file:Alignment_2100000_2102120.xml"
PoolSvc.WriteCatalog="file:TrtAlign_07.xml"

PoolSvc.FileOpen = "update"
#
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
theApp.OutStream = []
ToolSvc = Service( "ToolSvc" )
#ToolSvc.CondStream1.OutputFile = "Alignment_2102540_2102999.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102464_2102539.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102411_2102463.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102372_2102410.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102312_2102371.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102298_2102311.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102226_2102297.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102182_2102225.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102151_2102181.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2102121_2102150.root"
#ToolSvc.CondStream1.OutputFile = "Alignment_2100000_2102120.root"
ToolSvc.CondStream1.OutputFile = "TrtAlign_07.root"
#
# Configure the database tool
# output stream
from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTAlignDbTool
MyTRTAlignDbTool = TRTAlignDbTool (name='MyAlignDbTool')

# use COOL container
ToolSvc.TRT_CondDbTool.useCOOL = True
# no IOVCallback
ToolSvc.TRT_CondDbTool.readFromTextFiles = True
# no reading in the tool initialize - we do it in the alg
ToolSvc.TRT_CondDbTool.alignTextFile = ""
ToolSvc.TRT_CondDbTool.calibTextFile = ""

#
theApp.EvtMax = 2
#

# TRT_Testwrite options
TRT_TestWrite = Algorithm( "TRT_TestWrite" )

# create new AlignableTransForms
TRT_TestWrite.Create=TRUE

# write them to ConDB/file
TRT_TestWrite.Write=TRUE


# here is the input file
TRT_TestWrite.AlignInputFile="newalign_2102540_2102999.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102464_2102539.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102411_2102463.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102372_2102410.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102312_2102371.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102298_2102311.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102226_2102297.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102182_2102225.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102151_2102181.txt"
#TRT_TestWrite.AlignInputFile="newalign_2102121_2102150.txt"
#TRT_TestWrite.AlignInputFile="newalign_2100000_2102120.txt"


# and the output file
#TRT_TestWrite.PoolOutputFile= "Alignment_2102540_2102999.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102464_2102539.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102411_2102463.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102372_2102410.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102312_2102371.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102298_2102311.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102226_2102297.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102182_2102225.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102151_2102181.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2102121_2102150.root"
#TRT_TestWrite.PoolOutputFile= "Alignment_2100000_2102120.root"
TRT_TestWrite.PoolOutputFile= "TrtAlign_07.root"

# just to check, we try to reproduce the input file from TDS
TRT_TestWrite.WriteAlignTextFile="alignout.txt"

##################################################
# choose whether to register the data with IOV
TRT_TestWrite.RegisterIOV=True
##################################################

# validity range
TRT_TestWrite.ValidRun1=2102540
TRT_TestWrite.ValidRun2=2102999
TRT_TestWrite.ValidEvent1=0
TRT_TestWrite.ValidEvent2=9999999

# DB tag
TRT_TestWrite.VersionTag="TrtAlign07"


