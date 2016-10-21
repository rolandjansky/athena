######################################################################################
#
#
######################################################################################


#if not ('PoolESDInput' in dir()):
#    PoolESDInput=["../AthenaP1BStoESD/ESD.pool.root"] 

#if not ('DetDescrVersion' in dir()):
#    if os.getenv("CMTPATH").count('15.5') > 0:
#       DetDescrVersion = 'ATLAS-GEO-08-00-02'
#    else: 
#       DetDescrVersion = 'ATLAS-GEO-10-00-00' 

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not os.path.exists(fileList[0]):
    print "FATAL ERROR: file can not be found, earlier test probably failed" 
    print "Missing files:", fileList
    exit(1) 


# use auto configuration
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=fileList

#["/afs/cern.ch/user/s/strom/atnroot/15.6.X.Y-VAL/AtlasProduction/rel_5/NICOS_area/NICOS_qmtest156XYVALProd32BS5G4ProdOpt/trigp1test_testconfiguration_work/RAWtoESD_iniBeam_savedBS/data09_iniBeam.ESD.pool.root"]


from RecExConfig.RecFlags import rec
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)




if not ('EvtMax' in dir()):
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

###############################

rec.readAOD=True
rec.doCBNT=False


doTrigger=True

TriggerModernConfig=True

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD=False 
doTAG=False

rec.doTruth=True



#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
DBReplicaSvc = DBReplicaSvc(UseCOOLSQLite=False)
svcMgr += DBReplicaSvc

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

#------------ This is for ATN/RTT tests only ---------
TriggerFlags.triggerMenuSetup = 'default'
#-------------end of flag for tests-------------------

#------------ This is a temporary fix ---------------
from RecExConfig.RecConfFlags import recConfFlags
recConfFlags.AllowIgnoreConfigError=False
athenaCommonFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

from TrigValAlgs.TrigValAlgsConfig import TrigEDMChecker
TrigEDMChecker = TrigEDMChecker("TrigEDMChecker")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigEDMChecker
TrigEDMChecker.doDumpAll = True



#Very temporary hack to get things running
# This shouldn't work -- we set the wrong conditions and 
# and then things run okay.  Just dumping the edm as we do 
# shouldn't require any conditions.
#from IOVDbSvc.CondDB import conddb 
#conddb.setGlobalTag('OFLCOND-CSC-00-01-00') 


jobproperties.PerfMonFlags.doMonitoring = True

MessageSvc.debugLimit = 10000000
MessageSvc.infoLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

#from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence
print ServiceMgr



# disable printing of histograms in finalize
if hasattr(svcMgr,'THistSvc'):
    svcMgr.THistSvc.OutputLevel = WARNING

# THistSvc = Service ( "THistSvc" )
# THistSvc.OutputLevel=WARNING
