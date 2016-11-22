#configuration file for level 1 menu
include.block("TrigJetHypo/TriggerConfig_Jet_Level1.py")

#if TriggerFlags.doLVL1:

# add the menu items used in the LVL1 trigger decision

# for the time being, the mult 1,2,3,4 items are all based on the same
# threshold - in the future the lower multiplicty items will use higher
# thresholds
# JT04

# thresholdFile.addJetThreshold(name="JT04",value="4*GeV",window="4")
triggerPythonConfig.addJetThreshold('JT04', value='4', range_begin='0', range_end='2', window="4" )


# JT15
#    menuFile.addItem(itemname="L1_J35",mult="1",prescale="1", thresholdname="JT15")
#    thresholdFile.addJetThreshold(name="JT15",value="15*GeV", window="8")
triggerPythonConfig.addJetThreshold('JT15', value='15', range_begin='3', range_end='5', window="8")
item = LVL1MenuItem('L1_J35', ctpid='96')
item.addAndedCondition(name='JT15', multi='1')
triggerPythonConfig.addLVL1Item(item)



# JT20
#    menuFile.addItem(itemname="L1_J45",mult="1",prescale="1", thresholdname="JT20")
#    thresholdFile.addJetThreshold(name="JT20",value="20*GeV", window="8")
triggerPythonConfig.addJetThreshold('JT20', value='20', range_begin='6', range_end='8', window="8")
item = LVL1MenuItem('L1_J45', ctpid='97')
item.addAndedCondition(name='JT20', multi='1')
triggerPythonConfig.addLVL1Item(item)



# 2JT20
#   menuFile.addItem(itemname="L1_2J45",mult="2",prescale="1", thresholdname="JT20")
#    thresholdFile.addJetThreshold(name="JT20",value="20*GeV", window="8")

item = LVL1MenuItem('L1_2J45', ctpid='98')
item.addAndedCondition(name='JT20', multi='2')
triggerPythonConfig.addLVL1Item(item)



# 3JT20
# menuFile.addItem(itemname="L1_3J45",mult="3",prescale="1", thresholdname="JT20")
#    thresholdFile.addJetThreshold(name="JT20",value="20*GeV", window="8")

item = LVL1MenuItem('L1_3J45', ctpid='99')
item.addAndedCondition(name='JT20', multi='3')
triggerPythonConfig.addLVL1Item(item)



# 4JT20
# menuFile.addItem(itemname="L1_4J45",mult="4",prescale="1", thresholdname="JT20")
#    thresholdFile.addJetThreshold(name="JT20",value="20*GeV", window="8")

item = LVL1MenuItem('L1_4J45', ctpid='100')
item.addAndedCondition(name='JT20', multi='4')
triggerPythonConfig.addLVL1Item(item)



# JT35
# menuFile.addItem(itemname="L1_J60",mult="1",prescale="1", thresholdname="JT35")
# thresholdFile.addJetThreshold(name="JT35",value="35*GeV", window="8")

triggerPythonConfig.addJetThreshold('JT35', value='35', range_begin='9', range_end='11', window="8")    
item = LVL1MenuItem('L1_J60', ctpid='101')
item.addAndedCondition(name='JT35', multi='1')
triggerPythonConfig.addLVL1Item(item)



# JT55
# menuFile.addItem(itemname="L1_J80",mult="1",prescale="1", thresholdname="JT55")
# thresholdFile.addJetThreshold(name="JT55",value="55*GeV", window="8")

triggerPythonConfig.addJetThreshold('JT55', value='55', range_begin='12', range_end='14', window="8")
item = LVL1MenuItem('L1_J80', ctpid='102')
item.addAndedCondition(name='JT55', multi='1')
triggerPythonConfig.addLVL1Item(item)



# JT80
# menuFile.addItem(itemname="L1_J170",mult="1",prescale="1", thresholdname="JT80")
# thresholdFile.addJetThreshold(name="JT80",value="80*GeV", window="8")

triggerPythonConfig.addJetThreshold('JT80', value='80', range_begin='15', range_end='17', window="8")
item = LVL1MenuItem('L1_J170', ctpid='103')
item.addAndedCondition(name='JT80', multi='1')
triggerPythonConfig.addLVL1Item(item)



# JT150
# menuFile.addItem(itemname="L1_J300",mult="1",prescale="1", thresholdname="JT150")
# thresholdFile.addJetThreshold(name="JT150",value="150*GeV", window="8")

triggerPythonConfig.addJetThreshold('JT150', value='150', range_begin='18', range_end='20', window="8")
item = LVL1MenuItem('L1_J300', ctpid='104')
item.addAndedCondition(name='JT150', multi='1')
triggerPythonConfig.addLVL1Item(item)



#    menuFile.addAndedConditionItems(itemname="L1_FJ30",thresholdList=["FL10", "FR10"] )
#
#    #JFL01
#    thresholdFile.addFwdJetLThreshold(name="FL10",value="10*GeV",window="8")
#
#    item = LVL1MenuItem('L1_FJ30', ctpid='10')
#    item.addAndedCondition(name='FL10', multi='1')
#    triggerPythonConfig.addLVL1Item(item)
#    triggerPythonConfig.addJetThreshold('FL10', value='10', range_begin='18', range_end='19', window="8")
#
#
#    #JFR01
#    thresholdFile.addFwdJetRThreshold(name="FR10",value="10*GeV",window="8")
#
#    item = LVL1MenuItem('L1_FJ30', ctpid='10')
#    item.addAndedCondition(name='FR10', multi='1')
#    triggerPythonConfig.addLVL1Item(item)
#    triggerPythonConfig.addJetThreshold('FR10', value='10', range_begin='18', range_end='19', window="8")
