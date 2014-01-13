# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
costLog = logging.getLogger('RunTrigCost.py')

#----------------------------------------------------------------------
# Set global flags and input files

EvtMax = -1
if   ('eventMax' in dir()): EvtMax = eventMax
elif ('setEvent' in dir()): EvtMax = setEvent

if ('PoolRDOInput' not in dir()):
    from TrigCostMonitor.TrigCostMonitorConfig import readInputFiles
    if ('setInput' in dir()) and type(setInput) == str:
        PoolRDOInput = readInputFiles(filename)
    else:
        PoolRDOInput = readInputFiles('input_files.txt')    

#----------------------------------------------------------------------
# RecExCommon flags
#from TrigSteering.TrigSteeringConfig import TrigSteer_L2
#TrigSteer_L2 = Algorithm( "HLT::TrigSteer/TrigSteer_L2" )
#TrigSteer_L2.cachingMode=0
doTrigger=True
TriggerModernConfig=True
doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doESD=False
doAOD=False
doESD=False
doTAG=False
doCBNT=False
doTruth=False
OutputLevel=INFO
DetDescrVersion="ATLAS-GEO-08-00-01"

if ('setGEO' in dir()):
    DetDescrVersion = setGEO
    costLog.info('Updated DetDescrVersion to: '+setGEO)

#----------------------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")

#----------------------------------------------------------------------
# Trigger flags

if ('setMenu' not in dir()):
    setMenu = 'Physics_pp_v2'
    costLog.info('Use default trigger menu: '+setMenu)
# set trigger menu
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
TriggerFlags.triggerMenuSetup = 'MC_pp_v2_tight_mc_prescale'
TriggerFlags.L1PrescaleSet = ''
TriggerFlags.HLTPrescaleSet = ''

#TriggerFlags.triggerMenuSetup = setMenu
#TriggerFlags.readLVL1configFromXML=False
#TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.inputLVL1configFile=''
TriggerFlags.inputHLTconfigFile=''
TriggerFlags.doLVL1=True
TriggerFlags.doLVL2=True
TriggerFlags.doEF=True
#TriggerFlags.L1PrescaleSet  = 'L1Prescales100_' +TriggerFlags.triggerMenuSetup()
#TriggerFlags.HLTPrescaleSet = 'HLTPrescales100_'+TriggerFlags.triggerMenuSetup()
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



def MuonOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.BphysicsSlice.signatures = ['mu10_Upsimumu_FS']
    TriggerFlags.BjetSlice.signatures = ['2b20_medium_3L1J20','2b10_medium_L1_2J10J50']
    TriggerFlags.EgammaSlice.signatures = ['e20_medium']
    TriggerFlags.JetSlice.signatures = ['j240_a4tc_EFFS_l2cleanph']
    TriggerFlags.METSlice.signatures = []
    TriggerFlags.MinBiasSlice.signatures = []
    TriggerFlags.MuonSlice.signatures = ['mu15_mu10_EFFS']
    TriggerFlags.TauSlice.signatures = []
    TriggerFlags.CombinedSlice.signatures = ['tau16_loose_e15_medium']

from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(MuonOnly)
    
costLog.info('TriggerFlags.useL1CaloCalibration='+str(TriggerFlags.useL1CaloCalibration))
costLog.info('Print TriggerFlags')
print TriggerFlags
    
#----------------------------------------------------------------------
# Add "always" chains to menu
#def addAlwaysAcceptChain():
#    TriggerFlags.MinBiasSlice.enableSignature("always")
#from TriggerMenuPython.GenerateMenu import GenerateMenu
#GenerateMenu.overwriteSignaturesWith(addAlwaysAcceptChain)

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
