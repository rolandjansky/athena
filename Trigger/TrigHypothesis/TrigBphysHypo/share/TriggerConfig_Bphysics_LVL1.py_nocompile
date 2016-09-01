include.block("TriggerRelease/TriggerConfig_Bphysics.py")

if TriggerFlags.doLVL1:


    if not thresholdFile.isDefined("JT04"):
        thresholdFile.addJetThreshold(name="JT04",value="4*GeV",window="4")

    if not menuFile.isDefined("L1_BJT15"):
        item = LVL1MenuItem('JT04', ctpid='110')
        item.addAndedCondition(name='JT04', multi='1')
        triggerPythonConfig.addLVL1Item(item)
        menuFile.addItem(itemname="L1_BJT15",mult="1",prescale="1",thresholdname="JT04")

    if not thresholdFile.isDefined("MU06"):
        thresholdFile.addMuonThreshold(name="MU06",value="6*GeV",opl="NO",confirm="1")

        if not menuFile.isDefined("L1_MU6"):
            menuFile.addItem(itemname="L1_MU6",mult="1",prescale="1", thresholdname="MU06")

        if not menuFile.isDefined("L1_2MU06"):
            item = LVL1MenuItem('L1_2MU06', prescale='1', ctpid='2')
            item.addAndedCondition(name='MU06', multi='2')
            triggerPythonConfig.addLVL1Item(item)

#    l1_mu6 = triggerPythonConfig.addLvl1Threshold(name='MU06', type='MUON', mapping=0, slot='SLOT9', connector='CON0')\
#     .addMuonThresholdValue(6, -49, 49, 0, 64)
#    cond_mu6 = l1_mu6.condition(multi=1)
#    item = LVL1MenuItem('L1_MU06', ctpid='32',group='1', prescale=1, priority='LOW').setLogic(cond_mu6)
#    triggerPythonConfig.addLVL1Item(item)
#    l1_jt4 = triggerPythonConfig.addLvl1Threshold(name='JT04', type='JET', mapping=0, slot='SLOT8', connector='CON1')\
#     .addJetThresholdValue(4, -49, 49, 0, 64, 4, 1 )
#    cond_jt4 = l1_jt4.condition(multi=1)
#    item = LVL1MenuItem('L1_JT04', ctpid='116',group='1', prescale=1, priority='LOW').setLogic(cond_jt4)
#    triggerPythonConfig.addLVL1Item(item)

    l1_mu6 = triggerPythonConfig.Lvl1Thresholds().thresholdOfName('MU06')
    cond_mu6 = l1_mu6.condition(multi=1)
    l1_jt4 = triggerPythonConfig.Lvl1Thresholds().thresholdOfName('JT04')
    cond_jt4 = l1_jt4.condition(multi=1)
    l1_em3 = triggerPythonConfig.Lvl1Thresholds().thresholdOfName('EM03')
    cond_em3 = l1_em3.condition(multi=1)


    item = LVL1MenuItem('L1_MU06JT04', ctpid='115',group='1', prescale=1, priority='LOW').setLogic(cond_mu6&cond_jt4)
    triggerPythonConfig.addLVL1Item(item)
    item = LVL1MenuItem('L1_MU06EM03', ctpid='116',group='1', prescale=1, priority='LOW').setLogic(cond_mu6&cond_em3)
    triggerPythonConfig.addLVL1Item(item)



