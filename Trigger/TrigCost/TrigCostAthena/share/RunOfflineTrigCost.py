from AthenaCommon.Logging import logging
costLog = logging.getLogger('RunTrigCost.py')

if ('PoolRDOInput' not in dir()):
    from TrigCostMonitor.TrigCostMonitorConfig import readInputFiles
    if ('setInput' in dir()) and type(setInput) == str:
        PoolRDOInput = readInputFiles(filename)
    else:
        PoolRDOInput = readInputFiles('input_files.txt')    

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolRDOInput


#----------------------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")

from RecExConfig.RecFlags import rec

rec.doTrigger.set_Value_and_Lock(True)

rec.doTau.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)

#----------------------------------------------------------------------
# Set global flags and input files



#----------------------------------------------------------------------
# RecExCommon flags

if ('setGEO' in dir()):
    DetDescrVersion = setGEO
    costLog.info('Updated DetDescrVersion to: '+setGEO)


#----------------------------------------------------------------------
# Trigger flags

if ('setMenu' not in dir()):
    setMenu = 'Physics_pp_v1'
    costLog.info('Use default trigger menu: '+setMenu)

TriggerFlags.triggerMenuSetup = setMenu
TriggerFlags.readLVL1configFromXML=False
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.inputLVL1configFile=''
TriggerFlags.inputHLTconfigFile=''
TriggerFlags.doLVL1=True
TriggerFlags.doLVL2=True
TriggerFlags.doEF=True
TriggerFlags.L1PrescaleSet  = 'L1Prescales100_' +TriggerFlags.triggerMenuSetup()
TriggerFlags.HLTPrescaleSet = 'HLTPrescales100_'+TriggerFlags.triggerMenuSetup()
TriggerFlags.configurationSourceList=['xml']
TriggerFlags.enableMonitoring = [ 'Time', 'CostAthena' ]
TriggerFlags.disableRandomPrescale=False

if setMenu.count('simpleL1Calib') or setMenu.count('InitialBeam') or setMenu.count('Cosmic'):
    TriggerFlags.useL1CaloCalibration=False
    costLog.info('Update TriggerFlags.useL1CaloCalibration=False')

if ('setL1Calo' in dir()) and setL1Calo:
    TriggerFlags.useL1CaloCalibration=True
    costLog.info('Force TriggerFlags.useL1CaloCalibration=True')

if ('setSimpleL1' in dir()) and setSimpleL1:
    TriggerFlags.useL1CaloCalibration=False
    costLog.info('Force TriggerFlags.useL1CaloCalibration=False')    

if ('setPS' in dir()) and not setPS:
    TriggerFlags.L1PrescaleSet  = 'None'
    TriggerFlags.HLTPrescaleSet = 'None'
    costLog.info('Turn off L1 and HLT prescales')

costLog.info('TriggerFlags.useL1CaloCalibration='+str(TriggerFlags.useL1CaloCalibration))
costLog.info('Print TriggerFlags')
print TriggerFlags
    
#----------------------------------------------------------------------
# Add "always" chains to menu
def addAlwaysAcceptChain():
    TriggerFlags.MinBiasSlice.enableSignature("always")
from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(addAlwaysAcceptChain)


#----------------------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")

from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = False
jp.PerfMonFlags.doFastMon = False

#----------------------------------------------------------------------
# Setup cost algorithms and options for collecting cost data

from TrigCostMonitor.TrigCostMonitorConfig import setupCostAlg, setupCostJob, setupCostExtras
setupCostAlg()
setupCostJob()
setupCostExtras()

if ('setDebug' in dir()):
    from TrigCostMonitor.TrigCostMonitorConfig import setupCostDebug
    if type(setDebug) == bool:
        if setDebug:
            setupCostDebug()
            costLog.info("Running with default debug options")
    elif type(setDebug) == str:
        setupCostDebug(setDebug)
        costLog.info("Running with modified debug options: "+setDebug)

EvtMax = -1
if   ('eventMax' in dir()): EvtMax = eventMax
elif ('setEvent' in dir()): EvtMax = setEvent

if   ('eventMax' in dir()): theApp.EvtMax = eventMax
elif ('setEvent' in dir()): theApp.EvtMax = setEvent
else: theApp.EvtMax = -1

