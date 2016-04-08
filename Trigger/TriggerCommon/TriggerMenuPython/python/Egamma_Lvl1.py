# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenuPython.TriggerPythonConfig import *

class Egamma_Lvl1:
    def __init__(self):
        pass
    def generateMenu(self, triggerPythonConfig):
        
        # EM01
        thr = triggerPythonConfig.addLvl1Threshold(name='EM01', type='EM', mapping=0, slot='SLOT7', connector='CON1')\
              .addEMThresholdValue(7, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        item = LVL1MenuItem('EM01',ctpid=1,group='1',prescale=1,priority='LOW')
        item.setLogic(Logic(thr.lvl1Condition(1)))
        triggerPythonConfig.addLVL1Item(item)
        
        # 2EM15

        thr = triggerPythonConfig.addLvl1Threshold(name='EM11', type='EM', mapping=1, slot='SLOT7', connector='CON1')\
              .addEMThresholdValue(11, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        item = LVL1MenuItem('L1_2EM15',ctpid=2,group='1',prescale=1,priority='LOW')
        item.setLogic(Logic(thr.lvl1Condition(2)))
        triggerPythonConfig.addLVL1Item(item)
        
        
        # 2EM15I
        
        thr = triggerPythonConfig.addLvl1Threshold(name='EM11I', type='EM', mapping=2, slot='SLOT7', connector='CON1')\
              .addEMThresholdValue(11, -49, 49, 0, 64, 3, 2, 2)
        item = LVL1MenuItem('L1_2EM15I', ctpid=3, group='1', prescale=1, priority='LOW')
        item.setLogic(Logic(thr.lvl1Condition(2)))
        triggerPythonConfig.addLVL1Item(item)
        
        
        # EM25
        
        thr = triggerPythonConfig.addLvl1Threshold(name='EM18', type='EM', mapping=3, slot='SLOT7', connector='CON1')\
              .addEMThresholdValue(18, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        item = LVL1MenuItem("L1_EM25", ctpid=4, group='1', prescale=1, priority='LOW')
        item.setLogic(Logic(thr.lvl1Condition(1)))
        triggerPythonConfig.addLVL1Item(item)
        
        
        # EM25I
        
        thr = triggerPythonConfig.addLvl1Threshold('EM18I', 'EM', mapping=4, slot='SLOT7', connector='CON1')\
              .addEMThresholdValue(18, -49, 49, 0, 64, 3, 2, 2)
        item = LVL1MenuItem("L1_EM25I", ctpid=5, group='1', prescale=1, priority='LOW')
        item.setLogic(Logic(thr.lvl1Condition(1)))
        triggerPythonConfig.addLVL1Item(item)
        
        
        # EM60
        
        thr = triggerPythonConfig.addLvl1Threshold('EM50', 'EM', mapping=5, slot='SLOT7', connector='CON1')\
              .addEMThresholdValue(50, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        item = LVL1MenuItem("L1_EM60", ctpid=6, group='1', prescale=1, priority='LOW')
        item.setLogic(Logic(thr.lvl1Condition(1)))
        triggerPythonConfig.addLVL1Item(item)
