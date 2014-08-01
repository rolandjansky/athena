include.block("TrigMuonHypo/TriggerConfig_Muon_LVL1.py")

if TriggerFlags.doLVL1: 

    #addMuonThreshold: name, value, OPL, confirm
    if TrigT1ConfigVersion.find("900GeV")!=-1:
         thresholdFile.addMuonThreshold(name="MU00",value="4.*GeV",opl="NO",confirm="1")
         thresholdFile.addMuonThreshold(name="MU05",value="5*GeV",opl="NO",confirm="1")
         thresholdFile.addMuonThreshold(name="MU06",value="6*GeV",opl="NO",confirm="1")
 
         #new python configuration
         triggerPythonConfig.addMuonThreshold(name="MU00",value="4*GeV",range_begin='0', range_end='2')
         triggerPythonConfig.addMuonThreshold(name="MU05",value="5*GeV",range_begin='3', range_end='5')
         triggerPythonConfig.addMuonThreshold(name="MU06",value="6*GeV",range_begin='6', range_end='8')
    
    else:
         thresholdFile.addMuonThreshold(name="MU06",value="6*GeV",opl="NO",confirm="1")
         thresholdFile.addMuonThreshold(name="MU08",value="8*GeV",opl="NO",confirm="1")
         thresholdFile.addMuonThreshold(name="MU10",value="10*GeV",opl="NO",confirm="1")
 
         # new python configuration
         triggerPythonConfig.addMuonThreshold(name="MU06",value="6*GeV",range_begin='0', range_end='2')
         triggerPythonConfig.addMuonThreshold(name="MU08",value="8*GeV",range_begin='3', range_end='5')
         triggerPythonConfig.addMuonThreshold(name="MU10",value="10*GeV",range_begin='6', range_end='8')
    
    
    thresholdFile.addMuonThreshold(name="MU11",value="11*GeV",opl="NO",confirm="1")
    thresholdFile.addMuonThreshold(name="MU20",value="20*GeV",opl="NO",confirm="1")
    thresholdFile.addMuonThreshold(name="MU40",value="40*GeV",opl="NO",confirm="1")
    
    #new python configuration
    triggerPythonConfig.addMuonThreshold(name="MU11",value="11*GeV",range_begin='9', range_end='11')
    triggerPythonConfig.addMuonThreshold(name="MU20",value="20*GeV",range_begin='12', range_end='14')
    triggerPythonConfig.addMuonThreshold(name="MU40",value="40*GeV",range_begin='15', range_end='17')


    ##########################################
    # addItem: name,multiplicity,pre-scale

    menuFile.addItem(itemname="L1_MU40",mult="1",prescale="1", thresholdname="MU40")
    menuFile.addOredConditionItems(itemname="L1_MU20",thresholdList=["MU40","MU20"]) 
    menuFile.addOredConditionItems(itemname="L1_MU11",thresholdList=["MU40","MU20","MU11"])
    
    #item = LVL1MenuItem('L1_MU06', prescale='1', ctpid='32')
    #item.addCondition(name='MU06', multi='1')
    #item.addOredCondition(name='MU06', multi='1')
    #item.addOredCondition(name='MU08', multi='1')
    #item.addOredCondition(name='MU10', multi='1')
    #item.addOredCondition(name='MU11', multi='1')
    #item.addOredCondition(name='MU20', multi='1')
    #item.addOredCondition(name='MU40', multi='1')
    #triggerPythonConfig.addLVL1Item(item)
    
    
    #new python configuration
    item = LVL1MenuItem('L1_MU40', prescale='1', ctpid='39')
    item.addCondition(name='MU40', multi='1')
    triggerPythonConfig.addLVL1Item(item)
    
    item = LVL1MenuItem('L1_MU20', prescale='1', ctpid='38')
    item.addCondition(name='MU20', multi='1')  
    #item.addOredCondition(name='MU40', multi='1')
    #item.addOredCondition(name='MU20', multi='1')
    triggerPythonConfig.addLVL1Item(item)
    
    item = LVL1MenuItem('L1_MU11', prescale='1', ctpid='37')
    item.addCondition(name='MU11', multi='1')  
    #item.addOredCondition(name='MU40', multi='1')
    #item.addOredCondition(name='MU20', multi='1')
    #item.addOredCondition(name='MU11', multi='1')
    triggerPythonConfig.addLVL1Item(item)
    

    if TrigT1ConfigVersion.find("900GeV")!=-1:
         menuFile.addOredConditionItems(itemname="L1_MU06",thresholdList=["MU40","MU20","MU11","MU06"])
         menuFile.addOredConditionItems(itemname="L1_MU05",thresholdList=["MU40","MU20","MU11","MU06","MU05"])
         menuFile.addOredConditionItems(itemname="L1_MU00",thresholdList=["MU40","MU20","MU11","MU06","MU05","MU00"])
 
         #new python configuration
         item = LVL1MenuItem('L1_MU06', prescale='1', ctpid='34')
         item.addCondition(name='MU06', multi='1')
         #item.addOredCondition(name='MU40', multi='1')
         #item.addOredCondition(name='MU20', multi='1')
         #item.addOredCondition(name='MU11', multi='1')
         #item.addOredCondition(name='MU06', multi='1')
         triggerPythonConfig.addLVL1Item(item)
 
         item = LVL1MenuItem('L1_MU05', prescale='1', ctpid='33')
         item.addCondition(name='MU05', multi='1')
         #item.addOredCondition(name='MU40', multi='1')
         #item.addOredCondition(name='MU20', multi='1')
         #item.addOredCondition(name='MU11', multi='1')
         #item.addOredCondition(name='MU06', multi='1')
         #item.addOredCondition(name='MU05', multi='1')
         triggerPythonConfig.addLVL1Item(item)
 
         item = LVL1MenuItem('L1_MU00', prescale='1', ctpid='32')
         item.addCondition(name='MU00', multi='1')
         #item.addOredCondition(name='MU40', multi='1')
         #item.addOredCondition(name='MU20', multi='1')
         #item.addOredCondition(name='MU11', multi='1')
         #item.addOredCondition(name='MU06', multi='1')
         #item.addOredCondition(name='MU05', multi='1')
         #item.addOredCondition(name='MU00', multi='1')
         triggerPythonConfig.addLVL1Item(item)
         
    else:
         menuFile.addOredConditionItems(itemname="L1_MU10",thresholdList=["MU40","MU20","MU11","MU10"])
         menuFile.addOredConditionItems(itemname="L1_MU08",thresholdList=["MU40","MU20","MU11","MU10","MU08"])
         menuFile.addOredConditionItems(itemname="L1_MU06",thresholdList=["MU40","MU20","MU11","MU10","MU08","MU06"])
 
         #new python configuration	 
         item = LVL1MenuItem('L1_MU10', prescale='1', ctpid='36')
         item.addCondition(name='MU10', multi='1')
         #item.addOredCondition(name='MU11', multi='1')
         #item.addOredCondition(name='MU20', multi='1')
         #item.addOredCondition(name='MU40', multi='1')
         triggerPythonConfig.addLVL1Item(item)
 
         item = LVL1MenuItem('L1_MU08', prescale='1', ctpid='35')
         item.addCondition(name='MU08', multi='1')
#         item.addOredCondition(name='MU08', multi='1')
#         item.addOredCondition(name='MU10', multi='1')
#         item.addOredCondition(name='MU11', multi='1')
#         item.addOredCondition(name='MU20', multi='1')
#         item.addOredCondition(name='MU40', multi='1')
         triggerPythonConfig.addLVL1Item(item)
 
         item = LVL1MenuItem('L1_MU06', prescale='1', ctpid='34')
         item.addCondition(name='MU06', multi='1')
#         item.addOredCondition(name='MU06', multi='1')
#         item.addOredCondition(name='MU08', multi='1')
#         item.addOredCondition(name='MU10', multi='1')
#         item.addOredCondition(name='MU11', multi='1')
#         item.addOredCondition(name='MU20', multi='1')
#         item.addOredCondition(name='MU40', multi='1')
         triggerPythonConfig.addLVL1Item(item)
        
	
    if TrigT1ConfigVersion.find("900GeV")!=-1:
         item = LVL1MenuItem('L1_2MU00', prescale='1', ctpid='40')
         item.addCondition(name='MU00', multi='2')
         triggerPythonConfig.addLVL1Item(item)
 
         item = LVL1MenuItem('L1_2MU05', prescale='1', ctpid='41')
         item.addCondition(name='MU05', multi='2')
         triggerPythonConfig.addLVL1Item(item)
    
         item = LVL1MenuItem('L1_2MU06', prescale='1', ctpid='42')
         item.addCondition(name='MU06', multi='2')
         triggerPythonConfig.addLVL1Item(item)
 
#         menuFile.startItem(itemname="L1_2MU00") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU00", mult="2")         
#         menuFile.addTriggerConditionInItem(thresholdName="MU05", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40", mult="2") 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU00",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU05",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU05",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.endOredConditionsInItem() 
#         menuFile.endItem() 

#         menuFile.startItem(itemname="L1_2MU05") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU05", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40", mult="2") 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU05",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.endOredConditionsInItem() 
#         menuFile.endItem() 

#         menuFile.startItem(itemname="L1_2MU06") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40", mult="2") 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.endOredConditionsInItem() 
#         menuFile.endItem() 
    else:
         item = LVL1MenuItem('L1_2MU06', prescale='1', ctpid='42')
         item.addCondition(name='MU06', multi='2')
         triggerPythonConfig.addLVL1Item(item)
 
         item = LVL1MenuItem('L1_2MU08', prescale='1', ctpid='43')
         item.addCondition(name='MU08', multi='2')
         triggerPythonConfig.addLVL1Item(item)
    
         item = LVL1MenuItem('L1_2MU10', prescale='1', ctpid='44')
         item.addCondition(name='MU10', multi='2')
         triggerPythonConfig.addLVL1Item(item)         

#         menuFile.startItem(itemname="L1_2MU06") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06", mult="2")         
#         menuFile.addTriggerConditionInItem(thresholdName="MU08", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40", mult="2") 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU06",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU08",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU08",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.endOredConditionsInItem() 
#         menuFile.endItem() 

#         menuFile.startItem(itemname="L1_2MU08") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU08", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40", mult="2") 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU08",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.endOredConditionsInItem() 
#         menuFile.endItem() 

#         menuFile.startItem(itemname="L1_2MU10") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20", mult="2") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40", mult="2") 

         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU10",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#         menuFile.startOredConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endOredConditionsInItem() 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.startAndedConditionsInItem() 
#         menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#         menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#         menuFile.endAndedConditionsInItem() 

#         menuFile.endOredConditionsInItem() 
#         menuFile.endItem() 
    item = LVL1MenuItem('L1_2MU11', prescale='1', ctpid='45')
    item.addCondition(name='MU11', multi='2')
    triggerPythonConfig.addLVL1Item(item)
 
    item = LVL1MenuItem('L1_2MU20', prescale='1', ctpid='46')
    item.addCondition(name='MU20', multi='2')
    triggerPythonConfig.addLVL1Item(item)
    
    item = LVL1MenuItem('L1_2MU40', prescale='1', ctpid='47')
    item.addCondition(name='MU40', multi='2')
    triggerPythonConfig.addLVL1Item(item)  


#    menuFile.startItem(itemname="L1_2MU11") 
#    menuFile.startOredConditionsInItem() 
#    menuFile.addTriggerConditionInItem(thresholdName="MU11", mult="2") 
#    menuFile.addTriggerConditionInItem(thresholdName="MU20", mult="2") 
#    menuFile.addTriggerConditionInItem(thresholdName="MU40", mult="2") 

#    menuFile.startAndedConditionsInItem() 
#    menuFile.addTriggerConditionInItem(thresholdName="MU11",mult="1") 
#    menuFile.startOredConditionsInItem() 
#    menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#    menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#    menuFile.endOredConditionsInItem() 
#    menuFile.endAndedConditionsInItem() 
    
#    menuFile.startAndedConditionsInItem() 
#    menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#    menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#    menuFile.endAndedConditionsInItem() 

#    menuFile.endOredConditionsInItem() 
#    menuFile.endItem() 

#    menuFile.startItem(itemname="L1_2MU20") 
#    menuFile.startOredConditionsInItem() 
#    menuFile.addTriggerConditionInItem(thresholdName="MU20", mult="2") 
#    menuFile.addTriggerConditionInItem(thresholdName="MU40", mult="2") 

#    menuFile.startAndedConditionsInItem() 
#    menuFile.addTriggerConditionInItem(thresholdName="MU20",mult="1") 
#    menuFile.addTriggerConditionInItem(thresholdName="MU40",mult="1") 
#    menuFile.endAndedConditionsInItem() 

#    menuFile.endOredConditionsInItem() 
#    menuFile.endItem() 

#    menuFile.addItem(itemname="L1_2MU40",mult="2",prescale="1", thresholdname="MU40")


triggerPythonConfig.setMuctpiInfo(low_pt=1, high_pt=1, max_cand=13)
