# this is for writing objects in the TRTCond namespace to DB
# must run within some RecExample

# setup IOVDbSvc
include("IOVDbSvc/IOVDbSvc_jobOptions.py")

WriteIOV = False
ReadIOV  = True

if WriteIOV : 
    # .. connect to development server
    #IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOLTEST:pwd4cool"
    # .. connect to local sqlite file
    IOVDbSvc.dbConnection  = "impl=cool;techno=sqlite;schema=trtcalibration.db;X:CMCP200"

#    read from Db and write out a text file
if ReadIOV:
    IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=trtcalibration.db;DUMMY:CMCP200</dbConnection> /TRT/Calib/RT<tag>RtRelation_TRT-Cosmics-Pit-01</tag>"]
    IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=trtcalibration.db;DUMMY:CMCP200</dbConnection> /TRT/Calib/T0<tag>StrawT0_TRT-Cosmics-Pit-01</tag>"]

# define the job
from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite

if ReadIOV:
    TRTCondWrite = TRTCondWrite( name = "TRTCondWrite" )
    
# .. read a text file, create folders in TDS and write to Db
if WriteIOV:
    # .. add the OutputConditionsAlg (finalize) job
    include("RegistrationServices/OutputConditionsAlg_jobOptions.py")
    # .. add a job reading a txt file and creating folders in TDS
    TRTCondWrite=TRTCondWrite( name = "TRTCondWrite",
                               CalibInputFile="trtpitbarrel_simcalib.txt")    
    # .. write to this POOL file
    ToolSvc.ConditionsAlgStream.OutputFile="trtcalibration.root"
    # .. the following container objects
    OutputConditionsAlg.ObjectList=["TRTCond::RtRelationMultChanContainer#/TRT/Calib/RT","TRTCond::StrawT0MultChanContainer#/TRT/Calib/T0"]
    # .. register them in conditions database
    OutputConditionsAlg.WriteIOV=True
    # .. with these user tags
    OutputConditionsAlg.IOVTagList=["RtRelation_TRT-Cosmics-Pit-01","StrawT0_TRT-Cosmics-Pit-01"]
    #OutputConditionsAlg.Run1=3
    #OutputConditionsAlg.LB1=4
    #OutputConditionsAlg.Run2=9999
    #OutputConditionsAlg.LB2=999999

topSequence+=TRTCondWrite

# specify POOL converter package containing the converters
include ( "InDetConditionsAthenaPool/InDetConditionsAthenaPool_joboptions.py" )

