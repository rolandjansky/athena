######################################################################################
#
# testAthenaDBConfigRDO.py
#
######################################################################################


### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]


from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

########Another Temporary fix ##############
if not ('DetDescrVersion' in dir()):
    TileUseCOOL=False
else:
    if  DetDescrVersion.find('CSC')  > -1 or DetDescrVersion.find('DC3') > -1 :
        TileUseCOOL=False
###############################

# flags for RecExCommon 
doTrigger=True
rec.doESD.set_Value_and_Lock(False)
TriggerModernConfig=True
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doESD.set_Value_and_Lock(False)
doTAG=False
rec.doCBNT=False
rec.doTruth=True

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.readMenuFromTriggerDb=True
TriggerFlags.readConfigFromTriggerDb=False
#what does this do??? 
TriggerFlags.triggerDbKeys=[1, 1, 1, 1] 

if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 
#else:
#    TriggerFlags.triggerMenuSetup = 'Physics_pp_v5'
TriggerFlags.L1PrescaleSet='' 
TriggerFlags.HLTPrescaleSet='' 
TriggerFlags.doHLT=True
# NB menu should be the same as in testAthenaXMLConfigRDO.py since they are compared.


#this part is needed to pick up a sqlite file from release  -- aint workn right now - check with mortiz/catrin/joerg how to fix that
foundSQLiteFile = False
fileName = "TriggerMenuSQLiteFile_" + TriggerFlags.menuVersion() + ".sqlite"
nightlyPaths=os.environ['DATAPATH'].split(':')
for p in nightlyPaths:
    if os.path.exists(p+"/" +fileName):
        TriggerMenuSQLitePath=p+"/TriggerMenuSQLiteFile_" + TriggerFlags.menuVersion() + ".sqlite"
        TriggerFlags.triggerDbConnection="sqlite_file:"+TriggerMenuSQLitePath
        print "TriggerMenuSQLite file is found in "+TriggerMenuSQLitePath
        foundSQLiteFile = True
        break

if not foundSQLiteFile:
    print "TriggerMenuSQLite '" + fileName + "' file is NOT found in DATAPATH, exiting."
    import sys
    sys.exit(1)

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

 

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False

jobproperties.PerfMonFlags.doMonitoring = True

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

#from AthenaCommon.AlgSequence import AlgSequence
#print AlgSequence
#print ServiceMgr



