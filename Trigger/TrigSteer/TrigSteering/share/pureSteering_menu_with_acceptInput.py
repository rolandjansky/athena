from TriggerMenuPython.TriggerPythonConfig import *

menu = TriggerPythonConfig('pureSteering_menu.xml', 'l1.xml' )



# L1 necessary

# menu.addEMThreshold("TM10", value="10", emiso='10', hadiso='11' )
# TE, XE
thr_te10 = menu.registerLvl1Threshold('TE10', 'TE', mapping=0, slot='SLOT8', connector='CON2')\
           .addEnergyThresholdValue(10, -49, 49, 0, 64)


# menu.addItem('EM15i', prescale='1', ctpid='2')
# menu.addEMThreshold('EM15i', value='15', emiso='10', hadiso='11' )

thr_em15i = menu.registerLvl1Threshold(name='EM15i', type='EM', mapping=3, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(13, -49, 49, 0, 64, 4, 3, 2)

item_em15i = LVL1MenuItem('L1_EM15i', ctpid=0x3).setLogic(thr_em15i.condition(1))
# item = LVL1MenuItem('2EM15i', prescale='1', ctpid='2')
# item.addAndedCondition(name='EM15i', multi='2')
# menu.addLVL1Item(item)
item_2em15i = LVL1MenuItem('2EM15i', ctpid=0xc).setLogic(thr_em15i.condition(2))


# menu.addItem('EM25i',  prescale='50', ctpid='3', priority='HIGH')
# menu.addEMThreshold('EM25i', value='20', emiso='10', etamin='-20', etamax='20' )

thr_em25i = menu.registerLvl1Threshold(name='EM25i', type='EM', mapping=6, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(25, -49, 49, 0, 64, 4, 3, 2)
item_em25i = LVL1MenuItem('EM25i', ctpid=0x6).setLogic(thr_em25i.condition(1))

# menu.addItem('MU06', prescale='0', ctpid='122')
# menu.addMuonThreshold('MU06', value='6')

thr_mu6  = menu.registerLvl1Threshold(name='MU06', type='MUON', mapping=1, slot='SLOT9', connector='CON0')\
           .addMuonThresholdValue(5, -49, 49, 0, 64)
item_mu6  = LVL1MenuItem('MU06',  ctpid=0x21).setLogic(thr_mu6.condition(1))


#item = LVL1MenuItem('2MU06', prescale='1', ctpid='2')
#item.addAndedCondition(name='MU06', multi='2')
#menu.addLVL1Item(item)

item_2mu6  = LVL1MenuItem('2MU06',  ctpid=0x27).setLogic(thr_mu6.condition(2))


#menu.addItem('MU20', prescale='0', ctpid='123')
#menu.addMuonThreshold('MU20', value='21')

thr_mu20 = menu.registerLvl1Threshold(name='MU20', type='MUON', mapping=4, slot='SLOT9', connector='CON0')\
           .addMuonThresholdValue(20, -49, 49, 0, 64)
item_mu20 = LVL1MenuItem('MU20', ctpid=0x24).setLogic(thr_mu20.condition(1))

# menu.addItem('J200', prescale='0', ctpid='56')
# menu.addJetThreshold('J200', value='200')


thr_j200 = menu.registerLvl1Threshold('J200', 'JET', mapping=7, slot='SLOT8', connector='CON0')\
                   .addJetThresholdValue(100, -49, 49, 0, 64, 8)

item_j200 = LVL1MenuItem('J200', ctpid=0x67, group='1', prescale=1).setLogic(thr_j200.condition(1))
#menu.addItem('J90', prescale='20', ctpid='56')
#menu.addJetThreshold('J90', value='90')

thr_j90 = menu.registerLvl1Threshold('J90', 'JET', mapping=6, slot='SLOT8', connector='CON0')\
          .addJetThresholdValue(90, -49, 49, 0, 64, 8)

#item = LVL1MenuItem('3J90', prescale='1', ctpid='2')
#item.addAndedCondition(name='MU06', multi='3')
#menu.addLVL1Item(item)

item_3j90 = LVL1MenuItem('3J90', ctpid=0x6a, group='1', prescale=1).setLogic(thr_j90.condition(3))


#menu.addItem('J65', prescale='30', ctpid='56')
#menu.addJetThreshold('J65', value='60')

thr_j60 = menu.registerLvl1Threshold('J60', 'JET', mapping=5, slot='SLOT8', connector='CON0')\
          .addJetThresholdValue(60, -49, 49, 0, 64, 8)
item_j60  = LVL1MenuItem('J60',  ctpid=0x65, group='1', prescale=50).setLogic(thr_j60.condition(1))


#menu.addItem('J50', prescale='0', ctpid='56')
#menu.addJetThreshold('J50', value='40')

thr_j50 = menu.registerLvl1Threshold('J50', 'JET', mapping=4, slot='SLOT8', connector='CON0')\
          .addJetThresholdValue(50, -49, 49, 0, 64, 8)
item_j50  = LVL1MenuItem('J50',  ctpid=0x64, group='1', prescale=500).setLogic(thr_j50.condition(1))

#menu.addItem('TAU25', prescale='0', ctpid='23')
#menu.addTauThreshold('TAU25', value='20')

thr_tau25 = menu.registerLvl1Threshold('HA25', 'TAU', mapping=5, slot='SLOT7', connector='CON2')\
            .addTauThresholdValue(25, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)

item_tau25 = LVL1MenuItem('TAU25', ctpid=0x45).setLogic(thr_tau25.condition(1))

# item = LVL1MenuItem('TAU25+XE30', prescale='1', ctpid='2')
# item.addAndedCondition(name='TAU25', multi='1')
# item.addAndedCondition(name='XE30', multi='1')
# menu.addLVL1Item(item)

#menu.addItem('XE30', prescale='0', ctpid='23')
#menu.addEtMissThreshold('XE30', value='20')


thr_xe30 = menu.registerLvl1Threshold('XE30', 'XE', mapping=5, slot='SLOT8', connector='CON2')\
           .addEnergyThresholdValue(30, -49, 49, 0, 64)

item_xe30 = LVL1MenuItem('XE30', ctpid=0x83, group='1', prescale=256).setLogic(thr_xe30.condition(1))

item_tau25_xe30 = LVL1MenuItem('TAU25+XE30', ctpid=0xd8)\
                  .setLogic(thr_tau25.condition(1)&thr_xe30.condition(1))



#item = LVL1MenuItem('J50+XE60', prescale='1', ctpid='2')
#item.addAndedCondition(name='J50', multi='1')
#item.addAndedCondition(name='XE60', multi='1')
#menu.addLVL1Item(item)

thr_xe60 = menu.registerLvl1Threshold('XE60', 'XE', mapping=6, slot='SLOT8', connector='CON2')\
           .addEnergyThresholdValue(60, -49, 49, 0, 64)

thr_tau50 = menu.registerLvl1Threshold('HA50', 'TAU', mapping=7, slot='SLOT7', connector='CON2')\
            .addTauThresholdValue(50, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)

item_j50_xe60 = LVL1MenuItem('J50+XE60', ctpid=0xd9)\
                   .setLogic(thr_j50.condition(1)&thr_xe60.condition(1))

#menu.addItem('XE60', prescale='0', ctpid='23')
#menu.addEtMissThreshold('XE60', value='50')

item_xe60 = LVL1MenuItem('XE60', ctpid=0x86, group='1', prescale=1).setLogic(thr_xe60.condition(1))


for a in dir():
    if a.startswith('item_'):
        eval("menu.addLvl1Item( %s)" % (a))







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

chain = HLTChain( chain_name="L2_em15_half", chain_counter="1", lower_chain_name="L1_EM15i", level="L2", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addHLTSignature('em15_half')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_em15_all", chain_counter="10", lower_chain_name="L1_EM15i", level="L2", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addHLTSignature('em15_all')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_em25_half", chain_counter="2", lower_chain_name="L1_EM15i", level="L2", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addHLTSignature('em25_half')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_em25_all", chain_counter="20", lower_chain_name="L1_EM15i", level="L2", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addHLTSignature('em25_all')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="H0_EF", chain_counter="2", lower_chain_name="", level="EF", prescale="1", pass_through="0")
chain.addTriggerTypeBit('1')
chain.addStreamTag('blaStream', prescale='1')
chain.addGroup("fakes")
menu.addHLTChain(chain)

menu.writeConfigFiles();
menu.dot(algs=True)
