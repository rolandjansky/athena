from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1            
from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
from TriggerMenu.l1.Lvl1MenuItems import LVL1MenuItem
from TriggerMenu.l1.TriggerTypeDef import TT
from TriggerMenu.l1.Lvl1Condition import ThrCondition, Lvl1InternalTrigger
from TriggerMenu.l1.Logic import Logic

#
# L1
#
l1menu = TriggerConfigLVL1(outputFile = 'l1menu.xml', menuName = 'menutest', topoMenu = None)
LVL1MenuItem.l1configForRegistration = l1menu

Lvl1Flags.thresholds = [ 'MU06', 'MU20' ]
Lvl1Flags.items = [ 'L1_MU06', 'L1_MU20', 'L1_MU21' ]

bgrp = Logic(Lvl1InternalTrigger('BGRP0')) & Logic(Lvl1InternalTrigger('BGRP1'))

thr = l1menu.registerThr('MU06','MUON').addThrValue(6)
LVL1MenuItem('L1_MU06').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.muon )

thr = l1menu.registerThr('MU20','MUON').addThrValue(20)
LVL1MenuItem('L1_MU20').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.muon )
LVL1MenuItem('L1_MU21').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.muon )


# fix CTP IDs
l1menu.registeredItems['L1_MU06'].setCtpid( 106 )
l1menu.registeredItems['L1_MU20'].setCtpid( 120 )
l1menu.registeredItems['L1_MU21'].setCtpid( 121 )

# overwrite menu
l1menu.generateMenu()        
l1menu.writeXML()





from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain
menu = TriggerPythonConfig('pureSteering_menu.xml')

# HLT
# chain1 = HLTChain( chain_name='HLT_e25', chain_counter='1', lower_chain_name='L1_EM25i', level='HLT', prescale='1', pass_through='1')
# chain1.addHLTSignature( 'em25i' ).addHLTSignature( 'em25i'' )
# chain1.addTriggerTypeBit('4')
# chain1.addStreamTag('electrons', prescale='1')
# chain1.addStreamTag('IDCalibration', prescale='1', type='calibration')
# chain1.addGroup('electrons')
# menu.addHLTChain(chain1)

chain = HLTChain( chain_name='HLT_mu6', chain_counter='1', lower_chain_name='L1_MU06', level='HLT', prescale='2', pass_through='0')
chain.addTriggerTypeBit('4')
chain.addStreamTag('muons', prescale='1')
chain.addGroup('muons')
menu.addHLTChain(chain)

chain = HLTChain( chain_name='HLT_mu20', chain_counter='2', lower_chain_name='L1_MU20', level='HLT', prescale='2', pass_through='0')
chain.addTriggerTypeBit('4')
chain.addStreamTag('muons', prescale='1')
chain.addGroup('muons')
menu.addHLTChain(chain)

chain = HLTChain( chain_name='HLT_mu21', chain_counter='3', lower_chain_name='L1_MU21', level='HLT', prescale='2', pass_through='0')
chain.addTriggerTypeBit('4')
chain.addStreamTag('muons', prescale='1')
chain.addGroup('muons')
menu.addHLTChain(chain)

chain = HLTChain( chain_name='HLT_mu20_21', chain_counter='4', lower_chain_name='L1_MU20 L1_MU21', level='HLT', prescale='2', pass_through='0')
chain.addTriggerTypeBit('4')
chain.addStreamTag('muons', prescale='1')
chain.addGroup('muons')
menu.addHLTChain(chain)


menu.writeConfigFiles();
menu.dot(algs=True)

