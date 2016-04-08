#### DB upload #############################################################
import commands

nightlyPaths=os.environ['XMLPATH'].split(':')
for p in nightlyPaths:
    if os.path.exists(p+"/TriggerMenuXML/") is True:
        TriggerMenuXMLPath=p+"/TriggerMenuXML/"
        print "TriggerMenuXML directory is found in "+TriggerMenuXMLPath
        success = True
        break
    else:
        success = False
                
if success is True:
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    menu=TriggerFlags.menuVersion()
    print "Uploading the menus to DB"
    out = commands.getoutput("get_files -symlink XMLUploadToDB.sh")
    print out
    command_str='./XMLUploadToDB.sh %s %s' % (TriggerMenuXMLPath, menu )
    print 'Executing: '+command_str
    out = commands.getoutput(command_str)
    print out
    print "End of the upload to DB"
    
####real jobO###############################################################
#here a real jobO starts

if not ('EvtMax' in dir()):
    EvtMax=5
if not ('OutputLevel' in dir()):
    OutputLevel=INFO
    LVL1OutputLevel=INFO
    HLTOutputLevel=INFO
    
###############################
doCBNT=False

doTrigger=True
doESD=False

TriggerModernConfig=True

doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False 
doESD=False 
doTAG=False

doTruth=True

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.readMenuFromTriggerDb=True
TriggerFlags.readConfigFromTriggerDb=False
TriggerFlags.triggerDbKeys=[2, 2, 2, 1]


TriggerMenuSQLitePath="./TriggerMenuSQLiteFile_" + TriggerFlags.menuVersion() + ".sqlite"
print "TriggerMenuSQLite file is "+TriggerMenuSQLitePath
TriggerFlags.triggerDbConnection="sqlite_file:"+TriggerMenuSQLitePath

TriggerFlags.enableMonitoring = [ 'Validation', 'Time' ]

#------------ This is a temporary fix ---------------
from RecExConfig.RecConfFlags import recConfFlags
recConfFlags.AllowIgnoreConfigError=False
athenaCommonFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

#jobproperties.PerfMonFlags.doMonitoring = True

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

#from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence
print ServiceMgr

#import sys
#sys.exit(0)

