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

from TrigUpgradeTest.EmuStepProcessingConfig import generateL1DecoderAndChains

# signatures
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree


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
makeHLTTree(HLTChains)
   

from AthenaCommon.AlgSequence import dumpMasterSequence
dumpMasterSequence()

theApp.EvtMax = 4

