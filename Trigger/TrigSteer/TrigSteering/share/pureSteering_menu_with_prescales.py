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







from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__dummyHypo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__dummyAlgoHLTErrorCode, PESA__dummyMonitoredFex, PESA__dummyAlgoForCalibration

# L2
# examples below show all the posibilities of configuring Sequences
defalgo = PESA__dummyAlgo('Em25')                                               # no arg given ...default name

menu.addSequence("EM25i" , defalgo, "em25i" )

EgammaAdv_g2 = PESA__dummyAlgo("EgammaAdv_g2")
menu.addSequence("em25i" ,EgammaAdv_g2 ,"em25i'" )
EgammaAdv_g3 = PESA__dummyAlgo("EgammaAdv_g3")
EverRecjectingHypo = PESA__dummyHypo("g25fail")
EverRecjectingHypo.PreScale=1000000
menu.addSequence("em25i" ,[EgammaAdv_g3, EverRecjectingHypo ] ,"L2_e25fail" )

# EF 
EFeg1 = PESA__dummyAlgo("EFeg1")
EFeg2 = PESA__dummyAlgo("EFeg2")
menu.addSequence("em25i'" ,EFeg1 ,"EF_e25i" )
menu.addSequence("EF_e25i" ,EFeg2 ,"EF_e25i'" )
EverRecjectingEFHypo = PESA__dummyHypo("efg25fail")
EverRecjectingEFHypo.PreScale=1000000
menu.addSequence("EF_e25i" ,[EFeg2, EverRecjectingEFHypo] ,"EF_e25fail" )


# chains

# L2
chain = HLTChain( chain_name="L2_e25", chain_counter="1", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_unp")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_pres2", chain_counter="2", lower_chain_name="EM25i", level="L2", prescale="2", pass_through="0", rerun_prescale="1")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_pres")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_pt", chain_counter="3", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="1")
chain.addHLTSignature( "em25i" ).addHLTSignature("em25i'").addHLTSignature( "L2_e25fail" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_pt")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_pt2", chain_counter="4", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="2")
chain.addHLTSignature( "em25i" ).addHLTSignature("em25i'").addHLTSignature( "L2_e25fail" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_pt2")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_pt2_pres2", chain_counter="5", lower_chain_name="EM25i", level="L2", prescale="2", pass_through="2")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_tricky")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_pres1.5", chain_counter="6", lower_chain_name="EM25i", level="L2", prescale="1.5", pass_through="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_float")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_pres_inf", chain_counter="7", lower_chain_name="EM25i", level="L2", prescale="0", pass_through="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_inf")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_disabled_by_negative_prescale", chain_counter="8", lower_chain_name="EM25i", level="L2", prescale="-1", pass_through="1", rerun_prescale="1")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addStreamTag('electrons', prescale='1', obeyLB="0")
chain.addGroup("electrons_inf")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_run_but_not_contributing_to_stream", chain_counter="9", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="0", rerun_prescale="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addStreamTag('exotic', prescale='0', obeyLB="0")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="L2_e25_prescaled_passthough", chain_counter="10", lower_chain_name="EM25i", level="L2", prescale="7", pass_through="2", rerun_prescale="0")
chain.addHLTSignature( "em25i" ).addHLTSignature( "em25i'" )
chain.addStreamTag('exotic', prescale='1', obeyLB="0")
chain.addGroup("electrons")
menu.addHLTChain(chain)



# coherence test. Idea of the test -- the chains should have pass factors as configured by the "prescale" params (2,5,10, -1)
# in addition the stream e25stream should show up only as many times as the chain of lowest prescale
# rate measurement per group (PRESCALEGROUP:em25) should be identical as L2_e25_coherent_prescale_0)
chain = HLTChain( chain_name="L2_e25_coherent_prescale_0", chain_counter="11", lower_chain_name="EM25i", level="L2", prescale="2", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_1", chain_counter="12", lower_chain_name="EM25i", level="L2", prescale="2.5", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_2", chain_counter="13", lower_chain_name="EM25i", level="L2", prescale="1", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_3", chain_counter="14", lower_chain_name="EM25i", level="L2", prescale="-1", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_4", chain_counter="15", lower_chain_name="EM25i", level="L2", prescale="0", pass_through="0", rerun_prescale="0")
chain.addStreamTag('e25stream', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:em25")
menu.addHLTChain(chain)


# the same on unseeded chains (both have the same PS value)
chain = HLTChain( chain_name="L2_e25_coherent_prescale_5", chain_counter="16", lower_chain_name="", level="L2", prescale="10", pass_through="0", rerun_prescale="0")
chain.addStreamTag('calib', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:uns")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="L2_e25_coherent_prescale_6", chain_counter="17", lower_chain_name="", level="L2", prescale="10", pass_through="0", rerun_prescale="0")
chain.addStreamTag('calib', prescale='1', obeyLB="0")
chain.addGroup("RATE:CPS:uns")
menu.addHLTChain(chain)


# EF
chain = HLTChain( chain_name="EF_e25i", chain_counter="1", lower_chain_name="L2_e25", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e25i_after_prescaled", chain_counter="2", lower_chain_name="L2_e25_pres2", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e25i_after_inf_prescaled", chain_counter="3", lower_chain_name="L2_e25_pres_inf", level="EF", prescale="1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="EF_e25i_prescaled", chain_counter="4", lower_chain_name="L2_e25", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="EF_e25i_prescaled_after_prescaled", chain_counter="5", lower_chain_name="L2_e25_pres2", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addTriggerTypeBit('4')
chain.addStreamTag('electrons', prescale='1')
chain.addStreamTag('express', type='express')
menu.addHLTChain(chain)



chain = HLTChain( chain_name="EF_e25i_rejecting_after_pass_through", chain_counter="6", lower_chain_name="L2_e25_pt2", level="EF", prescale="0", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25fail" )
chain.addStreamTag('electrons', prescale='1')
chain.addStreamTag('express', type='express', prescale='2')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="EF_e25i_disabled_by_negative_prescale", chain_counter="7", lower_chain_name="L2_e25_pt2", level="EF", prescale="-1", pass_through="1")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25i'" )
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="EF_e25_run_but_not_contributing_to_stream_tag", lower_chain_name="L2_e25_run_but_not_contributing_to_stream", chain_counter="10", level="EF", prescale="1", pass_through="1", rerun_prescale="0")
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('exotic', prescale='0', obeyLB="0")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="EF_e25_adding_stream_tag_in_70_percent", lower_chain_name="L2_e25_run_but_not_contributing_to_stream", chain_counter="11", level="EF", prescale="1", pass_through="0", rerun_prescale="0")
chain.addHLTSignature( "EF_e25i" ).addHLTSignature( "EF_e25i'" )
chain.addStreamTag('exp70', prescale='1.4', obeyLB="0")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e25_after_prescaled_passthough", chain_counter="12", lower_chain_name="L2_e25_prescaled_passthough", level="EF", prescale="1", pass_through="0", rerun_prescale="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25fail" )
chain.addStreamTag('exotic', prescale='1', obeyLB="0")
chain.addGroup("electrons")
menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e25_passhthrough_after_prescaled_passthough", chain_counter="13", lower_chain_name="L2_e25_prescaled_passthough", level="EF", prescale="1", pass_through="1", rerun_prescale="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25fail" )
chain.addStreamTag('exotic', prescale='1', obeyLB="0")
chain.addGroup("electrons")
menu.addHLTChain(chain)


chain = HLTChain( chain_name="EF_e25i_prescaling_after_pass_through", chain_counter="14", lower_chain_name="L2_e25_pt2", level="EF", prescale="2", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)

chain = HLTChain( chain_name="EF_e25i_off_after_pass_through", chain_counter="15", lower_chain_name="L2_e25_pt2", level="EF", prescale="-1", pass_through="0")
chain.addHLTSignature( "EF_e25i" )
chain.addHLTSignature( "EF_e25fail" )
chain.addStreamTag('electrons', prescale='1')
menu.addHLTChain(chain)


chain = HLTChain( chain_name="EF_no_l2_seed", chain_counter="16", lower_chain_name="", level="EF", prescale="1", pass_through="0")
chain.addStreamTag('test',  prescale='1')
chain.addTriggerTypeBit('99')
menu.addHLTChain(chain)




menu.writeConfigFiles();
menu.dot(algs=True)
