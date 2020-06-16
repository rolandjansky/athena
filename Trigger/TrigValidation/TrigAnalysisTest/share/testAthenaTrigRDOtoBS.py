### usually ATN tests runs with following RDO input:


#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
#acf.FilesInput=["/afs/cern.ch/atlas/offline/ReleaseData/v19/testfile/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.digit.RDO.e1513_s1499_s1504_d700_10evt.pool.root"]
#["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"] 
#DetDescrVersion="ATLAS-CSC-01-02-00"
jp.AthenaCommonFlags.BSRDOOutput="raw.data"

if not jp.AthenaCommonFlags.EvtMax.is_locked():
    jp.AthenaCommonFlags.EvtMax=5

from RecExConfig.RecFlags import rec
if not ('OutputLevel' in dir()):
#    rec.OutputLevel=DEBUG
    rec.OutputLevel=INFO
#scan for RTT files (only if dsName and fileRange set)
include("TriggerTest/TrigScanFiles.py")
###############################

doTrigger=True
#doTriggerConfigOnly=True 
rec.doESD=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False
#rec.doESD=False
rec.doESD.set_Value_and_Lock(False)
doTAG=False
rec.doCBNT=False
#rec.doTruth=True

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

#TriggerFlags.readHLTconfigFromXML=False
#TriggerFlags.readLVL1configFromXML=False

# set up trigger monitoring                                                                                                                                                        
if not ('RunningRTT' in dir()):
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time' , 'Log' ]
else:
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time' ]

TriggerFlags.writeBS=True
rec.doWriteBS = True

#------------ This is for ATN/RTT tests only ---------
#TriggerFlags.triggerMenuSetup = 'default'
#TriggerFlags.triggerMenuSetup = 'Physics_pp_v5'
if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 
TriggerFlags.L1PrescaleSet = ''
TriggerFlags.HLTPrescaleSet = ''
TriggerFlags.doHLT=True
#-------------end of flag for tests-------------------

#------------ This is a temporary fix ---------------
#from RecExConfig.RecConfFlags import recConfFlags
#recConfFlags.AllowIgnoreConfigError=False
#acf.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------


#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

from LumiBlockComps.LumiBlockMuWriterDefault import LumiBlockMuWriterDefault
LumiBlockMuWriterDefault()

#-----------------Monitoring and leak check-----------------
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "ntuple.root"

#include("TriggerTest/leakCheck.py")

#-----------------------------------------------------------

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

preExec='rec.doFloatingPointException=True'

printfunc (AlgSequence())
printfunc (ServiceMgr)

#-------------------------------------------------------------
# Disable overly verbose and problematic ChronoStatSvc print-out
#-------------------------------------------------------------
include("TriggerTest/disableChronoStatSvcPrintout.py")
