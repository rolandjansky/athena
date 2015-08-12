from TriggerMenuPython.TriggerPythonConfig import *

menu = TriggerPythonConfig('pureNewSteering_menu.xml', 'l1.xml' )


# L1 necessary
menu.addItem('EM15i', prescale='1', ctpid='2')
menu.addEMThreshold('EM15i', value='15', emiso='10', hadiso='11' )

item = LVL1MenuItem('2EM15i', prescale='1', ctpid='2')
item.addAndedCondition(name='EM15i', multi='2')
menu.addLVL1Item(item)

menu.addItem('EM25i',  prescale='50', ctpid='3', priority='HIGH')
menu.addEMThreshold('EM25i', value='20', emiso='10', etamin='-20', etamax='20' )

menu.addItem('MU06', prescale='0', ctpid='122')
menu.addMuonThreshold('MU06', value='6')

item = LVL1MenuItem('2MU06', prescale='1', ctpid='2')
item.addAndedCondition(name='MU06', multi='2')
menu.addLVL1Item(item)


menu.addItem('MU20', prescale='0', ctpid='123')
menu.addMuonThreshold('MU20', value='21')

menu.addItem('J200', prescale='0', ctpid='56')
menu.addJetThreshold('J200', value='200')

menu.addItem('TAU25', prescale='0', ctpid='23')
menu.addTauThreshold('TAU25', value='20')

from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs

menu.addSequence("EM15i", PESA__dummyAlgo("EgammaAdv"), "em15i'" )
menu.addSequence("EM25i", PESA__dummyAlgo("EgammaAdv2"),"em25i'" )
menu.addSequence("MU20" , PESA__dummyAlgo("MuonAdv") ,  "mu20'" )
menu.addSequence("MU06" ,  PESA__dummyAlgo("MuonAdv2"), "mu06'" )
menu.addSequence("J200" , PESA__dummyAlgo("JetAdv") ,   "j200'" )
menu.addSequence("TAU25" ,PESA__dummyAlgo("TauAdv") ,   "tau25'" )

# chains

chain1 = HLTChain( chain_name="trick_chain", chain_counter="1", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain1.addHLTSignature( ["em25i'", "mu20'", "mu20'", "j200'", "tau25'", "tau25'"] )
chain1.addTriggerTypeBit('4')
chain1.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain1)

chain2 = HLTChain( chain_name="em25i_chain", chain_counter="2", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain2.addHLTSignature( ["em25i'", "em25i'", "mu20'", "mu20'", "j200'", "j200'"] )
chain2.addTriggerTypeBit('4')
chain2.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain2)

menu.writeConfigFiles();
menu.dot(algs=False)
