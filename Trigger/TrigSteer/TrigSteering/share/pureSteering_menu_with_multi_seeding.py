from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1            
from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
from TriggerMenu.l1.Lvl1MenuItems import LVL1MenuItem
from TriggerMenu.l1.TriggerTypeDef import TT
from TriggerMenu.l1.Lvl1Condition import ThrCondition, Lvl1InternalTrigger
from TriggerMenu.l1.Logic import Logic

# L1 necessary
Lvl1Flags.thresholds = ['EM3','EM25i']
Lvl1Flags.items = ['L1_EM3','L1_EM25i']

l1menu = TriggerConfigLVL1(outputFile = 'l1menu.xml', menuName = 'menutest', topoMenu = None)
LVL1MenuItem.l1configForRegistration = l1menu

bgrp = Logic(Lvl1InternalTrigger('BGRP0')) & Logic(Lvl1InternalTrigger('BGRP1'))

thr = l1menu.registerThr('EM3','EM').addThrValue(3)
LVL1MenuItem('L1_EM3').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )

thr = l1menu.registerThr('EM25i','EM').addThrValue(25)
LVL1MenuItem('L1_EM25i').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )


from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain
menu = TriggerPythonConfig('pureSteering_menu.xml')

from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__dummyAlgoHLTErrorCode, PESA__dummyMonitoredFex, PESA__dummyAlgoForCalibration
Egamma = PESA__dummyAlgo('Egamma_L2')                                     # create configurable instance
Egamma2 = PESA__dummyAlgo('Egamma2_L2')                                   # create configurable another instance
EgammaAdv3 = PESA__dummyAlgo("EgammaAdv3")
EgammaAdv_L2 = PESA__dummyAlgo("EgammaAdv_L2")
menu.addSequence("EM15i" , [ Egamma, Egamma2, EgammaAdv3 ] , "em15i" )  # use bot here, mixing with old fassioned way
menu.addSequence("em15i" , EgammaAdv_L2, "em15i'" )    # create configurable on the fly


# chains
# passage based on EM3
chain = HLTChain( chain_name="L2_Ored", chain_counter="100", lower_chain_name="L1_MU6,L1_EM25i", level="HLT", prescale="1", pass_through="0")
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25i", chain_counter="101", lower_chain_name="L1_EM25i", level="HLT", prescale="1", pass_through="0")
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_mu6", chain_counter="102", lower_chain_name="L1_MU6", level="HLT", prescale="1", pass_through="0")
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)





menu.writeConfigFiles();
menu.dot(algs=True)
