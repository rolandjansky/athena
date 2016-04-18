
# L1 menu
include('TrigSteering/pureSteering_l1menu.py')

from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain

menu = TriggerPythonConfig('pureSteering_menu.xml')

from TrigSteeringTest.TrigSteeringTestConf import DummyFexWithAcceptInput



ac_all = DummyFexWithAcceptInput("ac_all")
ac_half= DummyFexWithAcceptInput("ac_half")
ac_half.prescale = 2
ac_half.OutputLevel=DEBUG

ac_all.prescale = 1
ac_all.OutputLevel=DEBUG

# build 4 sequences 
menu.addSequence("EM15i" , [ ac_half ] , "em15_half" )
menu.addSequence("EM25i" , [ ac_half ] , "em25_half" )

menu.addSequence("EM15i" , [ ac_all ] , "em15_all" )
menu.addSequence("EM25i" , [ ac_all ] , "em25_all" )

chain = HLTChain( chain_name="L2_em15_half", chain_counter="1", lower_chain_name="L1_EM15i", level="HLT", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addHLTSignature('em15_half')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_em15_all", chain_counter="10", lower_chain_name="L1_EM15i", level="HLT", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addHLTSignature('em15_all')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_em25_half", chain_counter="2", lower_chain_name="L1_EM15i", level="HLT", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addHLTSignature('em25_half')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_em25_all", chain_counter="20", lower_chain_name="L1_EM15i", level="HLT", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addHLTSignature('em25_all')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)

menu.writeConfigFiles();
menu.dot(algs=True)
