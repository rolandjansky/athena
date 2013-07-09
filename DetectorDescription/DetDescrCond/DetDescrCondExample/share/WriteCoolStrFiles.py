# WriteCoolStrFiles.py
# demonstrates writing of two files into a common CLOB folder in COOL
# the COOL folder must be created BEFOREHAND using make_coolstrfile.py
# see the mainpage.h doc for more details
#
# define two instances of the ReadWriteCoolStrFile algorithm to manipulate
# the data files
#
theApp.Dlls+=["DetDescrCondTools","DetDescrCondExample"]
theApp.TopAlg+=["ReadWriteCoolStrFile/RWInstA","ReadWriteCoolStrFile/RWInstB"]

# parameters for first instance - goes to channel 1 in /TEST/COOLSTR
RWA=Algorithm("RWInstA")
# setting both write and read to true means objects will be created and
# then read back from TDS for verification
RWA.Write=True
RWA.Read=True
# supply your own datafile here
RWA.WFile="mytest1.dat"
RWA.Channel=1
# use CLOB technology
RWA.Tech=1
RWA.Folder="/TEST/COOLSTR"

# parameters for second instance - goes to channel 2 in /TEST/COOLSTR
RWB=Algorithm("RWInstB")
RWB.Write=True
RWB.Read=True
# supply your own datafile
RWB.WFile="mytest2.dat"
RWB.Channel=2
# use CLOB technology (MUST be consistent with other channels in this folder)
RWB.Tech=1
RWB.Folder="/TEST/COOLSTR"

# to see more of what is going on
Service("CoolStrFileSvc").OutputLevel=1

# data is written to conditions database using OutputConditionsAlg
# include basic setup
include("RegistrationServices/OutputConditionsAlg_jobOptions.py")

# set options specific to this use of OutputConditionsAlg

# in the case of CoolStrFileSvc, which uses COOL inline payload, the
# output POOL file contains nothing useful and  can be thrown away
ToolSvc.ConditionsAlgStream.OutputFile="dummy.root"

# list of things to be written - since we are using two channels in the
# same folder, only one write operation is needed for everything
#
OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#/TEST/COOLSTR" ]
OutputConditionsAlg.WriteIOV=True
# set the interval of validity for the file here
# putting nothing falls back on default which is to be valid for all run/event
OutputConditionsAlg.Run1=0
OutputConditionsAlg.Event1=0
OutputConditionsAlg.Run2=9999
OutputConditionsAlg.Event2=99999
# specify a conditions DB tag here (or omit for HEAD tag only)
OutputConditionsAlg.IOVTagList=["TEST_COOLSTR_mytag"]

# specify the COOL datatbase connection string
include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc=Service("IOVDbSvc")
# this example is for a local SQlite file mysqlfile.db, COOL DB named COOLTEST
IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mysqlfile.db;X:COOLTEST:X:X"
