# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###############################################################
#
#
#==============================================================
#use McEventSelector
#theApp.setup( MONTECARLO )
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
theApp.EvtMax = 3
# WriteCoolStrFiles.py
# demonstrates writing of two files into a common CLOB folder in COOL
# the COOL folder must be created BEFOREHAND using make_coolstrfile.py
# see the mainpage.h doc for more details
#
# define two instances of the ReadWriteCoolStrFile algorithm to manipulate
# the data files
#
theApp.Dlls+=["MuonCondCool"]
#theApp.Dlls+=["DetDescrCondTools","DetDescrCondExample"]
theApp.TopAlg+=["MuonCalib::MdtReadWriteCoolStr/RWInstA"]
#"MuonCalib::MdtReadWriteCoolStr/RWInstA2","MuonCalib::MdtReadWriteCoolStr/RWInstA3","MuonCalib::MdtReadWriteCoolStr/RWInstA4","MuonCalib::MdtReadWriteCoolStr/RWInstA5","MuonCalib::MdtReadWriteCoolStr/RWInstA6"]

# more instances to load more than one file
# parameters for first instance - goes to channel 1 in /TEST/COOLSTR
RWA=Algorithm("RWInstA")
# setting both write and read to true means objects will be created and
# then read back from TDS for verification
RWA.Write=True
RWA.Read=True
# supply your own datafile here
#RWA.WFile="T0BIL_2_1.dat"
RWA.WFile="amdb.txt"
#RWA.WFile="Rt_BML_1_1.dat"
RWA.Channel=1
# use CLOB technology
RWA.Tech=1
#RWA.Tech=1
RWA.Folder="/MDT/BARREL/ALIGN"
RWA.AlignCorr=True

# to see more of what is going on
MdtCoolStrSvc=Service("MuonCalib::MdtCoolStrSvc")
MdtCoolStrSvc.OutputLevel=2

# data is written to conditions database using OutputConditionsAlg
# include basic setup
include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
include("AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py")
# set options specific to this use of OutputConditionsAlg
#include("OutputConditionsAlg_jobOptions.py")
# in the case of CoolStrFileSvc, which uses COOL inline payload, the
# output POOL file contains nothing useful and  can be thrown away
#theApp.OutStream = []
ToolSvc = Service( "ToolSvc" )
ToolSvc.ConditionsAlgStream.OutputFile="dummy.root"
# list of things to be written - since we are using two channels in the
# same folder, only one write operation is needed for everything
#
OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#/MDT/BARREL/ALIGN" ]
OutputConditionsAlg.WriteIOV=True
# set the interval of validity for the file here
# putting nothing falls back on default which is to be valid for all run/event
OutputConditionsAlg.Run1=100380
OutputConditionsAlg.Event1=20
OutputConditionsAlg.Run2=100380
OutputConditionsAlg.Event2=40

################
# specify the COOL datatbase connection string
include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc=Service("IOVDbSvc")

###############
# specify a conditions DB tag here (or omit for HEAD tag only)
#OutputConditionsAlg.IOVTagList=["MDT/T0_mytag"]
IOVDbSvc.Folders += ["/MDT/BARREL/ALIGN <tag>HEAD</tag>"]

# Private Application Configuration options
#--------------------------------------------------------------

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
IOVDbTestAlg = Algorithm( "IOVDbTestAlg" )
IOVDbTestAlg.OutputLevel    = 2
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel        = 2
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel    = 2
################

#service name="oracle://atlas_dd/atlasdd;"
IOVDbSvc.dbConnection="oracle://ATLAS_COOLPROD;schema=ATLAS_COOL_MDT;user=ATLAS_COOL_MDt;dbname=COMPROD;password=xxx"
# this example is for a local SQlite file mysqlfile.db, COOL DB named COOLTEST
#IOVDbSvc.dbConnection="oracle://devdb10;schema=ATLAS_MDT;user=ATLAS_MDT;dbname=CAL"
try:
    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;ATLAS_COOLPROD:"
    IOVDbSvc.dbConnection  += CAL
    IOVDbSvc.dbConnection  += ":ATLAS_COOL_MDT:xxx"
except Exception:
    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;ATLAS_COOLPROD:COMPROD:ATLAS_COOL_MDT:xxx"
#try:
#    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:"
#    IOVDbSvc.dbConnection  += CAL
#    IOVDbSvc.dbConnection  += ":ATLAS_MDT:mdt_oracle"
#except:
#    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:CAL:ATLAS_MDT:mdt_oracle"
