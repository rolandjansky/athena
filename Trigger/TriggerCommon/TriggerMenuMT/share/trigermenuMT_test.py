#
# Minimal example to build a trigger menu
#
from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1            
from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
from TriggerMenu.l1.Lvl1MenuItems import LVL1MenuItem
from TriggerMenu.l1.TriggerTypeDef import TT
from TriggerMenu.l1.Lvl1Condition import ThrCondition, Lvl1InternalTrigger
from TriggerMenu.l1.Logic import Logic

# Define L1 menu thresholds and items
Lvl1Flags.RemapThresholdsAsListed = True
Lvl1Flags.thresholds = ['EM3','MU4']
Lvl1Flags.items = ['L1_EM3','L1_MU4']

l1menu = TriggerConfigLVL1( outputFile = 'l1menu.xml', 
                            menuName = 'menutest',
                            topoMenu = None )

LVL1MenuItem.l1configForRegistration = l1menu

# Register L1 menu
bgrp = Logic(Lvl1InternalTrigger('BGRP0')) & Logic(Lvl1InternalTrigger('BGRP1'))

EM3 = l1menu.registerThr('EM3','EM').addThrValue(3)
LVL1MenuItem('L1_EM3'       ).setLogic( ThrCondition(EM3) & bgrp ).setTriggerType( TT.calo )

MU4 = l1menu.registerThr('MU4','MUON').addThrValue(4)
LVL1MenuItem('L1_MU4'       ).setLogic( ThrCondition(MU4) & bgrp ).setTriggerType( TT.muon )

# Generate and write L1 menu
l1menu.generateMenu()        
l1menu.writeXML()


from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenu.menu.HLTObjects import HLTChain
hltmenu = TriggerPythonConfig('hltmenu.xml')

from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
defalgo = PESA__dummyAlgo('Em3')
hltmenu.addSequence("EM3" , defalgo, "em3" )

chain = HLTChain( chain_name="HLT_e3", chain_counter="1", lower_chain_name="EM3", level = "HLT", prescale="1", pass_through="1")
chain.addHLTSignature( "em3" ).addHLTSignature( "em3" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1', obeyLB="yes")
chain.addStreamTag('IDCalibration', prescale='1', obeyLB="no", type="calibration")
chain.addGroup("electrons")
hltmenu.addHLTChain(chain)

hltmenu.writeConfigFiles();

import sys
sys.exit(0)
