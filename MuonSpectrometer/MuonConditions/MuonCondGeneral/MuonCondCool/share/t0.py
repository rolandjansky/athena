# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###############################################################
#
#
#==============================================================
#use McEventSelector
theApp.setup( MONTECARLO )
theApp.EvtMax = 5
# WriteCoolStrFiles.py
# demonstrates writing of two files into a common CLOB folder in COOL
# the COOL folder must be created BEFOREHAND using make_coolstrfile.py
# see the mainpage.h doc for more details
#
# define two instances of the ReadWriteCoolStrFile algorithm to manipulate
# the data files
#
theApp.Dlls+=["MuonCondCool"]

theApp.TopAlg+=["MuonCalib::MdtReadWriteCoolStr/RWInstA","MuonCalib::MdtReadWriteCoolStr/RWInstA2","MuonCalib::MdtReadWriteCoolStr/RWInstA3","MuonCalib::MdtReadWriteCoolStr/RWInstA4","MuonCalib::MdtReadWriteCoolStr/RWInstA5","MuonCalib::MdtReadWriteCoolStr/RWInstA6"]

# more instances to load more than one file
# parameters for first instance - goes to channel 1 in /TEST/COOLSTR
RWA=Algorithm("RWInstA")
# setting both write and read to true means objects will be created and
# then read back from TDS for verification
RWA.Write=True
RWA.Read=True
# supply your own datafile here
RWA.WFile="T0BOL370_7_1.dat"
#RWA.WFile="Rt_BML_1_1.dat"
RWA.Channel=7
# use CLOB technology
RWA.Tech=1
#RWA.Tech=1
RWA.Folder="/MDT/COOL1.3/T0"
RWA.CalT0=True
##################################################3
RWA2=Algorithm("RWInstA2")
# setting both write and read to true means objects will be created and
# then read back from TDS for verification
RWA2.Write=True
RWA2.Read=True
# supply your own datafile here
RWA2.WFile="T0BOL370_7_2.dat"
#RWA.WFile="Rt_BML_1_1.dat"
RWA2.Channel=8
# use CLOB technology
RWA2.Tech=1
#RWA.Tech=1
RWA2.Folder="/MDT/COOL1.3/T0"
RWA2.CalT0=True
##################################################3
RWA3=Algorithm("RWInstA3")
# setting both write and read to true means objects will be created and
# then read back from TDS for verification
RWA3.Write=True
RWA3.Read=True
# supply your own datafile here
RWA3.WFile="T0BOL370_7_3.dat"
#RWA.WFile="Rt_BML_1_1.dat"
RWA3.Channel=9
# use CLOB technology
RWA3.Tech=1
#RWA.Tech=1
RWA3.Folder="/MDT/COOL1.3/T0"
RWA3.CalT0=True
##################################################3
RWA4=Algorithm("RWInstA4")
# setting both write and read to true means objects will be created and
# then read back from TDS for verification
RWA4.Write=True
RWA4.Read=True
# supply your own datafile here
RWA4.WFile="T0BML370_7_1.dat"
#RWA.WFile="Rt_BML_1_1.dat"
RWA4.Channel=4
# use CLOB technology
RWA4.Tech=1
#RWA.Tech=1
RWA4.Folder="/MDT/COOL1.3/T0"
RWA4.CalT0=True
##################################################3
RWA5=Algorithm("RWInstA5")
# setting both write and read to true means objects will be created and
# then read back from TDS for verification
RWA5.Write=True
RWA5.Read=True
# supply your own datafile here
RWA5.WFile="T0BML370_7_2.dat"
#RWA.WFile="Rt_BML_1_1.dat"
RWA5.Channel=5
# use CLOB technology
RWA5.Tech=1
#RWA.Tech=1
RWA5.Folder="/MDT/COOL1.3/T0"
RWA5.CalT0=True
##################################################3
RWA6=Algorithm("RWInstA6")
# setting both write and read to true means objects will be created and
# then read back from TDS for verification
RWA6.Write=True
RWA6.Read=True
# supply your own datafile here
RWA6.WFile="T0BML370_7_3.dat"
#RWA.WFile="Rt_BML_1_1.dat"
RWA6.Channel=6
# use CLOB technology
RWA6.Tech=1
#RWA.Tech=1
RWA6.Folder="/MDT/COOL1.3/T0"
RWA6.CalT0=True
##################################################3




# to see more of what is going on
MdtCoolStrSvc=Service("MuonCalib::MdtCoolStrSvc")
MdtCoolStrSvc.OutputLevel=3

# data is written to conditions database using OutputConditionsAlg
# include basic setup
#include("OutputConditionsAlg_jobOptions.py")
include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
include("AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py")
# set options specific to this use of OutputConditionsAlg

# in the case of CoolStrFileSvc, which uses COOL inline payload, the
# output POOL file contains nothing useful and  can be thrown away
#theApp.OutStream = [""]
#ToolSvc = Service( "ToolSvc" )
#ToolSvc.ConditionsAlgStream.OutputFile="dummy.root"

#Service("CoolStrFileSvc").OutputLevel=1
#include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
ToolSvc = Service( "ToolSvc" )
ToolSvc.ConditionsAlgStream.OutputFile="dummy.root"




# list of things to be written - since we are using two channels in the
# same folder, only one write operation is needed for everything
#
OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#/MDT/COOL1.3/T0" ]
OutputConditionsAlg.WriteIOV=True
# set the interval of validity for the file here
# putting nothing falls back on default which is to be valid for all run/event
OutputConditionsAlg.Run1=100345
OutputConditionsAlg.Event1=0
OutputConditionsAlg.Run2=100379
OutputConditionsAlg.Event2=999999999
OutputConditionsAlg.IOVTagList=["/MDT/COOL1.3/T0 <tag>HEAD</tag>"]


# specify the COOL datatbase connection string
include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc=Service("IOVDbSvc")

###############
# specify a conditions DB tag here (or omit for HEAD tag only)
#OutputConditionsAlg.IOVTagList=["MDT/T0_mytag"]
#IOVDbSvc.Folders+=["/CAL/MDT/BARREL/ALIGN <tag>HEAD</tag>"]

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
# this example is for a local SQlite file mysqlfile.db, COOL DB named COOLTEST
IOVDbSvc.dbConnection="oracle://devdb10;schema=ATLAS_MDT;user=ATLAS_MDT;dbname=CAL"
try:
    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:"
    IOVDbSvc.dbConnection  += CAL
    IOVDbSvc.dbConnection  += ":ATLAS_MDT:mdt_oracle"
except Exception:
    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:CAL:ATLAS_MDT:mdt_oracle"
