###############################################################
#
# Job options file
#
# Based on AthExStoreGateExamples
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------
from __future__ import print_function

# input parameters:
class opt:
    doMenu          = False # use either menu or manual chain building



from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigUpgradeTest.EmuStepProcessingConfig import generateL1DecoderAndChainsManually, generateL1DecoderAndChainsByMenu
from AthenaCommon.AlgSequence import AlgSequence
from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON

# signatures
# steps: sequential AND of 1=Filter 2=Processing
# chainstep=single chain step
# global step=joint for all chains
# filters: one SeqFilter per step, per chain
# inputMakers: one per each first RecoAlg in a step (so one per step), one input per chain that needs that step

log = logging.getLogger('EmuStepProcessingTest.py')
log.info('Setup options:')
defaultOptions = [a for a in dir(opt)]
for option in defaultOptions:
    if option in globals():
        setattr(opt, option, globals()[option])
        print(' %20s = %s' % (option, getattr(opt, option)))
    else:        
        print(' %20s = (Default) %s' % (option, getattr(opt, option)))



topSequence = AlgSequence()
if opt.doMenu is True:
    generateL1DecoderAndChainsByMenu(topSequence)
else:
    generateL1DecoderAndChainsManually(topSequence)

generateJSON()

from TrigConfigSvc.TrigConfigSvcCfg import getHLTConfigSvc, getL1ConfigSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += getHLTConfigSvc()
TriggerFlags.triggerMenuSetup = "LS2_v1"
svcMgr += getL1ConfigSvc()

from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
if hasattr(svcMgr.THistSvc, "Output"):
    from TriggerJobOpts.HLTTriggerGetter import setTHistSvcOutput
    setTHistSvcOutput(svcMgr.THistSvc.Output)

print ("EmuStepProcessing: dump top Sequence after CF/DF Tree build")
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence( topSequence )
#dumpMasterSequence()

theApp.EvtMax = 4

TriggerFlags.generateMenuDiagnostics=True
