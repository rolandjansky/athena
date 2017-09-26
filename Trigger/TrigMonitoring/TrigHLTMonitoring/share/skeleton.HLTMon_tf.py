# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Provides job setup for running RAW to HIST or AOD to HIST
"""

import logging
logging.basicConfig(level = logging.DEBUG)
monLog = logging.getLogger('HLTMon_tf')
monLog.info('************* Starting HLTMon transform ***********')

from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#Common job options disable most RecExCommon by default. Re-enable below on demand

rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False
rec.doTruth=False

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth.set_Value_and_Lock(False)

monDict = {
        'General':False,
        'Egamma':False,
        'Calo':False,
        'Muon':False,
        'IDtrk':False,
        'MET':False,
        'Tau':False,
        'Jet':False,
        'Bphys':False,
        'Bjet':False,
        'MinBias':False,
        'IDJpsiMon':False,
        'MaM':True,
        'MaM_ApplyMCK':True,
        'MaM_UseReproDB':False
        }

if hasattr(runArgs,"inputBSFile"):
    rec.readRDO.set_Value_and_Lock(True)
    rec.doAOD=True
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBSFile )

if hasattr(runArgs,"inputAODFile"):
    rec.readAOD.set_Value_and_Lock(True)
    rec.readRDO.set_Value_and_Lock(False)

    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )

if hasattr(runArgs,"outputHIST_TEMPFile"):
    rec.doMonitoring.set_Value_and_Lock(False)
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.doMonitoring.set_Value_and_Lock(False)
    DQMonFlags.histogramFile.set_Value_and_Lock( runArgs.outputHIST_TEMPFile )
    DQMonFlags.doHLTMon.set_Value_and_Lock(True)

if hasattr(runArgs,"monFlags"):
    for flag in runArgs.monFlags:
        onoff = flag[:2]
        monType = flag[2:]
        # Note that we can add new validation types on the fly
        if onoff == 'do':
            monDict[monType] = True
        elif onoff == 'no':
            monDict[monType] = False
        else:
            monLog.warning("Ignored unrecognised validation control string for {0}: {1}".format(monType, flag))

monLog.info("Validation switches are set to: {0}".format(monDict))
# Schedule individual validations
if not 'HLTMonFlags' in dir():
    from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
for monType, enabled in monDict.iteritems():
    monLog.info('Enabling %s %s',monType,enabled)
    HLTMonFlags.set_All_Off()
    if enabled:
        exec "HLTMonFlags.do%s = True" % (monType)
monLog.info(HLTMonFlags)

if hasattr(runArgs,"preExec"):
    monLog.info('transform preExec')
    for cmd in runArgs.preExec:
        monLog.info(cmd)
        exec(cmd)

# Main job options for job setup
include ('TrigHLTMonitoring/TrigHLTMonCommon_jobOptions.py')
