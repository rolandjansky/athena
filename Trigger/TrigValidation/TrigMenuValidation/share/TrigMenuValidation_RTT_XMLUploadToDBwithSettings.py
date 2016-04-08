#### DB upload #############################################################
import commands

print "Uploading the menus to DB"
out = commands.getoutput("get_files -symlink XMLDumperFromAthenaWithSettings.sh")
print out
from TriggerJobOpts.TriggerFlags import TriggerFlags
menu=TriggerFlags.menuVersion()
command_str='./XMLDumperFromAthenaWithSettings.sh %s' % menu 
out = commands.getoutput(command_str)
print out
print "End of the upload to DB"

#import sys
#sys.exit(0)

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
TriggerFlags.triggerDbKeys=[1, 1, 1, 1]


TriggerMenuSQLitePath="./TriggerMenuSQLiteFileWithSettings_" + TriggerFlags.menuVersion() + ".sqlite"
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

