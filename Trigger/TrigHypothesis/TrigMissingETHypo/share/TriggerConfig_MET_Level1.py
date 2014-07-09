include.block("TrigMissingETHypo/TriggerConfig_MET_Level1.py")

# 8 thresholds on MET + 4 thresholds on SumEt

# MET a)
th = triggerPythonConfig.addLvl1Threshold( name='XE15', type='XE', mapping=0, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="15", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_XE15', ctpid='128', group='1', prescale=30000)
item.addAndedCondition(name='XE15', multi='1')

triggerPythonConfig.addLVL1Item(item)

# MET b)
th = triggerPythonConfig.addLvl1Threshold( name='XE20', type='XE', mapping=1, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="20", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_XE20', ctpid='129', group='1', prescale=7000)
item.addAndedCondition(name='XE20', multi='1')

triggerPythonConfig.addLVL1Item(item)

# MET c)
th = triggerPythonConfig.addLvl1Threshold( name='XE25', type='XE', mapping=2, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="25", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_XE25', ctpid='130', group='1', prescale=1500)
item.addAndedCondition(name='XE25', multi='1') 

triggerPythonConfig.addLVL1Item(item)

# MET d)
th = triggerPythonConfig.addLvl1Threshold( name='XE30', type='XE', mapping=3, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="30", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_XE30', ctpid='131', group='1', prescale=200)
item.addAndedCondition(name='XE30', multi='1')

triggerPythonConfig.addLVL1Item(item)

# MET e)
th = triggerPythonConfig.addLvl1Threshold( name='XE40', type='XE', mapping=4, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="40", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_XE40', ctpid='132', group='1', prescale=20)
item.addAndedCondition(name='XE40', multi='1')

triggerPythonConfig.addLVL1Item(item)

# MET f)
th = triggerPythonConfig.addLvl1Threshold( name='XE50', type='XE', mapping=5, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="50", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_XE50', ctpid='133', group='1', prescale=2)
item.addAndedCondition(name='XE50', multi='1')

triggerPythonConfig.addLVL1Item(item)

# MET g)
th = triggerPythonConfig.addLvl1Threshold( name='XE70', type='XE', mapping=6, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="70", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_XE70', ctpid='134', group='1', prescale=1)
item.addAndedCondition(name='XE70', multi='1')

triggerPythonConfig.addLVL1Item(item)

# MET h)
th = triggerPythonConfig.addLvl1Threshold( name='XE80', type='XE', mapping=7, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="80", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_XE80', ctpid='135', group='1', prescale=1)
item.addAndedCondition(name='XE80', multi='1')

triggerPythonConfig.addLVL1Item(item)

# SumET a)
th = triggerPythonConfig.addLvl1Threshold( name='TE100', type='TE', mapping=0, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="100", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_TE100', ctpid='136', group='1', prescale=86000)
item.addAndedCondition(name='TE100', multi='1')
triggerPythonConfig.addLVL1Item(item)

# SumET b)
th = triggerPythonConfig.addLvl1Threshold( name='TE200', type='TE', mapping=1, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="200", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_TE200', ctpid='137', group='1', prescale=1800)
item.addAndedCondition(name='TE200', multi='1')
triggerPythonConfig.addLVL1Item(item)

# SumET c)
th = triggerPythonConfig.addLvl1Threshold( name='TE304', type='TE', mapping=2, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="304", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_TE304', ctpid='138', group='1', prescale=19)
item.addAndedCondition(name='TE304', multi='1')
triggerPythonConfig.addLVL1Item(item)

# SumET d)
th = triggerPythonConfig.addLvl1Threshold( name='TE380', type='TE', mapping=3, slot='SLOT8', connector='CON2' )
th.addEnergyThresholdValue(value="380", etamin="-49" , etamax="49" , phimin="0" , phimax="64", priority="0")

item = LVL1MenuItem('L1_TE380', ctpid='139', group='1', prescale=1)
item.addAndedCondition(name='TE380', multi='1')
triggerPythonConfig.addLVL1Item(item)
