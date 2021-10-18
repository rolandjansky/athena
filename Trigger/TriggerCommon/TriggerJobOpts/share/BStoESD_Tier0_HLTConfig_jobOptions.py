# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
##############################################################
# BStoESD_Tier0_HLTConfig_jobOptions.py
# For DATA reconstruction
##############################################################

# Configuration depends on TriggerFlags.configForStartup():
#   HLToffline       : HLT is ran offline, configuration is read from JSON files
#   HLTonline        : Normal running, everything is taken from COOL

from RecExConfig.RecFlags import rec
from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.Include import include

from AthenaCommon.Logging import logging
_log = logging.getLogger("BStoESD_Tier0_HLTConfig_jobOptions.py")
_log.info("TriggerFlag.configForStartup = %s", tf.configForStartup())

assertMsg = 'This file is meant for Trigger configuration in RAWtoESD/RAWtoALL data reconstruction.'
assert rec.doTrigger(), assertMsg + ' Since rec.doTrigger is disabled, this file should not be included.'

# First check if HLT psk is ok, if not, turn trigger off.
if tf.configForStartup() != 'HLToffline':
    include( "TriggerJobOpts/TriggerConfigCheckHLTpsk.py" )

if rec.doTrigger():
    
    # configure the HLT config
    if not hasattr( ServiceMgr, "ByteStreamAddressProviderSvc" ):
        from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
        ServiceMgr += ByteStreamAddressProviderSvc()

    ConfigFlags.Trigger.readBS = True
    tf.doHLT= False # needed to not rerun the trigger

    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()

    from TriggerJobOpts.T0TriggerGetter import T0TriggerGetter
    triggerGetter = T0TriggerGetter()

    if rec.doWriteBS():
        include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )

del _log
## end of configure the HLT config
