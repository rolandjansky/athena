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

Lvl1Flags.thresholds = ['TE10','EM15i','EM25i','MU06','MU20','J50','J60','J90','J200','HA25','XE60','HA50','XE30','XE60']
Lvl1Flags.items = ['L1_EM15i','L1_2EM15i','L1_EM25i','L1_MU06','L1_2MU06','L1_MU20','L1_J50','L1_J60','L1_3J90','L1_J200','L1_TAU25','L1_XE30','L1_XE60','L1_J50_XE60']

bgrp = Logic(Lvl1InternalTrigger('BGRP0')) & Logic(Lvl1InternalTrigger('BGRP1'))

thr = l1menu.registerThr('TE10','TE').addThrValue(10)
LVL1MenuItem('L1_TE10').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )

thr = l1menu.registerThr('EM15i','EM').addThrValue(15)
LVL1MenuItem('L1_EM15i').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )
LVL1MenuItem('L1_2EM15i').setLogic( ThrCondition(thr).x(2) & bgrp ).setTriggerType( TT.calo )

thr = l1menu.registerThr('EM25i','EM').addThrValue(25)
LVL1MenuItem('L1_EM25i').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )

thr = l1menu.registerThr('MU06','MUON').addThrValue(6)
LVL1MenuItem('L1_MU06').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.muon )
LVL1MenuItem('L1_2MU06').setLogic( ThrCondition(thr).x(2) & bgrp ).setTriggerType( TT.muon )

thr = l1menu.registerThr('MU20','MUON').addThrValue(20)
LVL1MenuItem('L1_MU20').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.muon )



thr = l1menu.registerThr('J200','JET').addThrValue(200)
LVL1MenuItem('L1_J200').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )

thr = l1menu.registerThr('J90','JET').addThrValue(90)
LVL1MenuItem('L1_J90').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )
LVL1MenuItem('L1_3J90').setLogic( ThrCondition(thr).x(3) & bgrp ).setTriggerType( TT.calo )

thr = l1menu.registerThr('J60','JET').addThrValue(60)
LVL1MenuItem('L1_J60').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )

thr_j50 = l1menu.registerThr('J50','JET').addThrValue(50)
LVL1MenuItem('L1_J50').setLogic( ThrCondition(thr) & bgrp ).setTriggerType( TT.calo )

thr_ha25 = l1menu.registerThr('HA25','TAU').addThrValue(25)
LVL1MenuItem('L1_TAU25').setLogic( ThrCondition(thr_ha25) & bgrp ).setTriggerType( TT.calo )

thr_xe30 = l1menu.registerThr('XE30','XE').addThrValue(30)
LVL1MenuItem('L1_XE30').setLogic( ThrCondition(thr_xe30) & bgrp ).setTriggerType( TT.calo )

LVL1MenuItem('L1_TAU25_XE30').setLogic( ThrCondition(thr_ha25) & ThrCondition(thr_xe30) & bgrp ).setTriggerType( TT.calo )

thr_xe60 = l1menu.registerThr('XE60','XE').addThrValue(60)
thr_ha50 = l1menu.registerThr('HA50','TAU').addThrValue(50)
LVL1MenuItem('L1_XE60').setLogic( ThrCondition(thr_xe60) & bgrp ).setTriggerType( TT.calo )
LVL1MenuItem('L1_J50_XE60').setLogic( ThrCondition(thr_j50) & ThrCondition(thr_xe60) & bgrp ).setTriggerType( TT.calo )

# Generate and write L1 menu
l1menu.generateMenu()        
l1menu.writeXML()
