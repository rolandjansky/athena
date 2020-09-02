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
from DecisionHandling.EmuStepProcessingConfig import generateL1DecoderAndChainsManually, generateL1DecoderAndChainsByMenu
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
defaultOptions = [a for a in dir(opt) if not a.startswith('__')]
for option in defaultOptions:
    if option in globals():
        setattr(opt, option, globals()[option])
        log.info(' %20s = %s' % (option, getattr(opt, option)))
    else:        
        log.info(' %20s = (Default) %s' % (option, getattr(opt, option)))

TriggerFlags.generateMenuDiagnostics=True

topSequence = AlgSequence()

if opt.doMenu is True:
    generateL1DecoderAndChainsByMenu(topSequence)
else:
    generateL1DecoderAndChainsManually(topSequence)

generateJSON()

from TrigConfigSvc.TrigConfigSvcCfg import getHLTConfigSvc, getL1ConfigSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags

ConfigFlags.Trigger.triggerMenuSetup = TriggerFlags.triggerMenuSetup = "LS2_v1"
svcMgr += conf2toConfigurable( getHLTConfigSvc(ConfigFlags))
svcMgr += conf2toConfigurable( getL1ConfigSvc(ConfigFlags))


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

