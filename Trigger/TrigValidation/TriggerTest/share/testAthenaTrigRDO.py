### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/ReleaseData/v3/testfile/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root"]

#
# flags doFastCaloAccess and memFreeze are used TriggerTestCommon.py
#

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
#scan for RTT files (only if dsName and fileRange set)
include("TriggerTest/TrigScanFiles.py")

if ('enableCostMonitoring' in dir()):
    enableCostMonitoring = True
else: 
    enableCostMonitoring = False

#RTT runs with costMonitoring on - test whether it is available in a given release
#test whether a package is useable in this release
import imp
try:
    imp.find_module('TrigCostD3PDMaker')
except:
    print 'CostMonitoring packages not available, setting  enableCostMonitoring=False'
    enableCostMonitoring=False

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

# set up trigger monitoring                                                                                                                                                        
if not ('RunningRTT' in dir()):
    TriggerFlags.enableMonitoring=[ 'Validation', 'Time' , 'Log' ]
else:
    TriggerFlags.enableMonitoring=[ 'Validation', 'Time' ]

# set trigger menu
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 
#else: 
#    TriggerFlags.triggerMenuSetup='default' 
TriggerFlags.L1PrescaleSet='' 
TriggerFlags.HLTPrescaleSet='' 
TriggerFlags.doHLT=True


# pre set up trigger monitoring
if 'enableCostMonitoring' in dir() and bool(enableCostMonitoring) == True:
    import TriggerRelease.Modifiers
    getattr(TriggerRelease.Modifiers,'enableCostMonitoring')().preSetup()
    getattr(TriggerRelease.Modifiers,'enableCostForCAF')().preSetup()
#

#------------ This is a temporary fix ---------------
#from RecExConfig.RecConfFlags import recConfFlags
#recConfFlags.AllowIgnoreConfigError=False
#athenaCommonFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------
# post set up trigger monitoring
if 'enableCostMonitoring' in dir() and bool(enableCostMonitoring) == True:
    import TriggerRelease.Modifiers
    getattr(TriggerRelease.Modifiers,'enableCostMonitoring')().postSetup()
    getattr(TriggerRelease.Modifiers,'enableCostForCAF')().postSetup()
    getattr(TriggerRelease.Modifiers,'enableCostD3PD')().postSetup()
    # Check if we are debugging the cost mon output - false by default
    if 'enableCostDebug' in dir() and bool(enableCostDebug) == True:
        getattr(TriggerRelease.Modifiers,'enableCostDebug')().postSetup()
#
#------------------------------------------------------------
include("TriggerTest/TriggerTestCommon.py")
#include("TriggerTest/TriggerTestFastCalo.py")
#------------------------------------------------------------

