######################################################################################
#
# testAthenaXMLConfigRDO.py
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

TriggerFlags.readHLTconfigFromXML=True
TriggerFlags.readLVL1configFromXML=True
if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 
#else:
#    TriggerFlags.triggerMenuSetup = 'Physics_pp_v5'
# NB menu should be the same as in testAthenaDBConfigRDO.py since they are compared.
TriggerFlags.L1PrescaleSet='' 
TriggerFlags.HLTPrescaleSet='' 
TriggerFlags.doHLT=True

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

 

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False

from PerfMonComps import PerfMonFlags
jobproperties.PerfMonFlags.doMonitoring = True

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc").ActivateHistory=False

#from AthenaCommon.AlgSequence import AlgSequence
#print AlgSequence
#print ServiceMgr



