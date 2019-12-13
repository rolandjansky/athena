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

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = "LS2_emu_v1"



from TrigUpgradeTest.EmuStepProcessingConfig import generateL1DecoderAndChains

# signatures
# steps: sequential AND of 1=Filter 2=Processing
# chainstep=single chain step
# global step=joint for all chains
# filters: one SeqFilter per step, per chain
# inputMakers: one per each first RecoAlg in a step (so one per step), one input per chain that needs that step

from AthenaCommon.AlgSequence import AlgSequence, AthSequencer, dumpSequence
topSequence = AlgSequence()
l1Decoder, HLTChains = generateL1DecoderAndChains()
topSequence += l1Decoder
##### Make all HLT #######
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
makeHLTTree( triggerConfigHLT=TriggerConfigHLT )

from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
generateJSON()

from TrigConfigSvc.TrigConfigSvcCfg import getHLTConfigSvc, getL1ConfigSvc
svcMgr += getHLTConfigSvc()
TriggerFlags.triggerMenuSetup = "LS2_v1"
svcMgr += getL1ConfigSvc()

   
print "EmuStepProcessing: dump top Sequence after CF/DF Tree build"
from AthenaCommon.AlgSequence import dumpMasterSequence, dumpSequence
dumpSequence( topSequence )
#dumpMasterSequence()

theApp.EvtMax = 4

