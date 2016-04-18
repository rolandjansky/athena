
# L1

# there seems to be 10 cases to be tested (X signifies prescaled item/chain, 0 means unprescalled item/chain, 
#L1 X X X X 0 0 0 
#L2 O X X O X 0 X X 0 X 
#EF 0 0 X X 0 X X 0 X X

# to complete the test we need 2 items which we can put on/off as needed in LVL11 seeds file

from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1            
from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
from TriggerMenu.l1.Lvl1MenuItems import LVL1MenuItem
from TriggerMenu.l1.TriggerTypeDef import TT
from TriggerMenu.l1.Lvl1Condition import ThrCondition, Lvl1InternalTrigger
from TriggerMenu.l1.Logic import Logic

l1menu = TriggerConfigLVL1(outputFile = 'l1menu.xml', menuName = 'menutest', topoMenu = None)
LVL1MenuItem.l1configForRegistration = l1menu


Lvl1Flags.thresholds = ['EM3','EM15i','EM25i']
Lvl1Flags.items = ['L1_EM3','L1_EM15i','L1_EM25i']

bgrp = Logic(Lvl1InternalTrigger('BGRP0')) & Logic(Lvl1InternalTrigger('BGRP1'))

thr = l1menu.registerThr('EM3','EM').addThrValue(3)
LVL1MenuItem('L1_EM3').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )

thr = l1menu.registerThr('EM15i','EM').addThrValue(15)
LVL1MenuItem('L1_EM15i').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )

thr = l1menu.registerThr('EM25i','EM').addThrValue(25)
LVL1MenuItem('L1_EM25i').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )



from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain

menu = TriggerPythonConfig('pureSteering_menu.xml')

from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__dummyAlgoHLTErrorCode, PESA__dummyMonitoredFex, PESA__dummyAlgoForCalibration

# L2
# examples below show all the posibilities of configuring Sequences
Egamma = PESA__dummyAlgo('Egamma_L2')                                     # create configurable instance
Egamma2 = PESA__dummyAlgo('Egamma2_L2')                                   # create configurable another instance
EgammaAdv3 = PESA__dummyAlgo("EgammaAdv3")
EgammaAdv_L2 = PESA__dummyAlgo("EgammaAdv_L2")
menu.addSequence("EM15i" , [ Egamma, Egamma2, EgammaAdv3 ] , "L2_em15i" )  # use bot here, mixing with old fassioned way
menu.addSequence("L2_em15i" , EgammaAdv_L2, "L2_em15i'" )    # create configurable on the fly

defalgo = PESA__dummyAlgo('Em25')                                               # no arg given ...default name

menu.addSequence("EM25i" , defalgo, "L2_em25i" )

EgammaAdv_g2 = PESA__dummyAlgo("EgammaAdv_g2")
menu.addSequence("L2_em25i" ,EgammaAdv_g2 ,"L2_em25i'" )





# EF

gamma_EF = PESA__dummyAlgo("gamma_EF")
gammaAdv_EF = PESA__dummyAlgo("gammaAdv_EF")
menu.addSequence("L2_em15i'", gamma_EF, "EF_g15" )
menu.addSequence("EF_g15", gammaAdv_EF, "EF_g15'" )

electron_EF = PESA__dummyAlgo("electron_EF")
electronAdv_EF = PESA__dummyAlgo("electronAdv_EF")
menu.addSequence("L2_em15i'", electron_EF , "EF_e15i" )
menu.addSequence("EF_e15i", electronAdv_EF , "EF_e15i'" )


menu.addSequence("L2_em25i'", gamma_EF , "EF_g25" )
menu.addSequence("EF_g25", gammaAdv_EF , "EF_g25'" )

menu.addSequence("L2_em25i'", electron_EF , "EF_e25i" )
menu.addSequence("EF_e25i", electronAdv_EF , "EF_e25i'" )


# chains
# passage based on EM3
chain = HLTChain( chain_name="L2_e3", chain_counter="100", lower_chain_name="L1_EM3", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "L2_em25i" ).addHLTSignature( "L2_em25i'" )
chain.addHLTSignature( "L2_em15i" ).addHLTSignature( "L2_em15i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e3", chain_counter="100", lower_chain_name="L2_e3", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)




# unprescalled seeded from prescalled item
chain = HLTChain( chain_name="L2_e25", chain_counter="1", lower_chain_name="L1_EM25i", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "L2_em25i" ).addHLTSignature( "L2_em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e25i", chain_counter="1", lower_chain_name="L2_e25", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)


# seeded from prescalled items & L2 chain is prescalled
chain = HLTChain( chain_name="L2_e15_prescalled", chain_counter="2", lower_chain_name="L1_EM15i", level="L2", prescale="2", pass_through="0")
chain.addHLTSignature( "L2_em25i" ).addHLTSignature( "L2_em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)



chain = HLTChain( chain_name="EF_e15_on_prescalled", chain_counter="2", lower_chain_name="L2_e15_prescalled", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)



# seeded from prescalled items & L2 & EF chains are prescalled
chain = HLTChain( chain_name="EF_e15_prescalled", chain_counter="3", lower_chain_name="L2_e15_prescalled", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)



# seeded from prescalled items & L2 & EF chains are prescalled
chain = HLTChain( chain_name="EF_e25_prescalled_on_unprescalled", chain_counter="4", lower_chain_name="L2_e25", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)



# seeded from prescalled items & L2 & EF chains are prescalled
chain = HLTChain( chain_name="L2_e15_pass", chain_counter="5", lower_chain_name="L1_EM15i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "L2_em25i" ).addHLTSignature( "L2_em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons")

menu.addHLTChain(chain)



chain = HLTChain( chain_name="EF_e15_on_pass", chain_counter="5", lower_chain_name="L2_e15_pass", level="EF", prescale="1", pass_through="1")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)



# unseeded & prescalled

chain = HLTChain( chain_name="L2_unseeded", chain_counter="11", lower_chain_name="", level="L2", prescale="3", pass_through="0")
chain.addTriggerTypeBit('11')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")

menu.addHLTChain(chain)





chain = HLTChain( chain_name="EF_on_unseeded", chain_counter="11", lower_chain_name="L2_unseeded", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")

menu.addHLTChain(chain)




# EF

chain = HLTChain( chain_name="EF_unseeded", chain_counter="12", lower_chain_name="", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
chain.addGroup("electrons")

menu.addHLTChain(chain)







menu.writeConfigFiles();
menu.dot(algs=True)
