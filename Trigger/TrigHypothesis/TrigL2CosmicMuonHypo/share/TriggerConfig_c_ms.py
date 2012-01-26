include.block('TrigL2CosmicMuonHypo/TriggerConfig_c_ms.py')
from TriggerMenuPython.TriggerPythonConfig import *

# include('TrigL2CosmicMuon/TriggerConfig_TrigL2CosmicMuon.py')
# include('TrigL2CosmicMuonHypo/TriggerConfig_CosmicMuonHypo.py')
from TrigL2CosmicMuon.TrigL2CosmicMuon_Config import TrigL2CosmicMuon_Cosmic
from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config \
     import CosmicMuonHypo_Cosmic



# HLT algorithms for cosmic trigger
# TrigL2CosmicMuon = TriggerConfig_TrigL2CosmicMuon('L2')
# CosmicMuonHypo = TriggerConfig_CosmicMuonHypo('L2')
TrigL2CosmicMuon = TrigL2CosmicMuon_Cosmic()

# Chain: TrigL2CosmicMuon -> CosmicMuonHypo
l2chain = HLTChain(chain_id='c_ms_L2', \
                   chain_counter='1', \
                   lower_chain_id='L1_MU00', \
                   level='L2', \
                   prescale='1', \
                   pass_through='1')
l2chain.addTriggerTypeBit(1)
l2chain.addTriggerTypeBit(2)
l2chain.addStreamTag('muons', 'cosmics')
l2chain.addHLTSignature(['c_ms'])

sequence = TrigL2CosmicMuon.classAndInstanceName()
sequence += CosmicMuonHypo.classAndInstanceName()
print 'sequence line set: ', sequence
TE = triggerPythonConfig.addSequence('MU00', \
                                     appendLevel(sequence, 'L2'), \
                                     'c_ms')
triggerPythonConfig.addHLTChain(l2chain)
