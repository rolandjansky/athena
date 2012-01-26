include.block('TrigL2CosmicMuonHypo/TriggerConfig_Cosmic_LVL1.py')
from __main__ import doM4L1Menu

if TriggerFlags.doLVL1 or TriggerFlags.doLVL2 or TriggerFlags.doEF:
    # LVL1 menu
    if not doM4L1Menu:
        thr_mu4 = triggerPythonConfig.addLvl1Threshold('MU4','MUON',0,'SLOT9','CON0')\
                  .addMuonThresholdValue(4, -49, 49, 0, 64)
        thr_mu5 = triggerPythonConfig.addLvl1Threshold('MU5', 'MUON', 1, 'SLOT9', 'CON0')\
                  .addMuonThresholdValue(5, -49, 49, 0, 64)
        thr_mu6 = triggerPythonConfig.addLvl1Threshold('MU6', 'MUON', 2, 'SLOT9', 'CON0')\
                  .addMuonThresholdValue(6, -49, 49, 0, 64)
        item_mu4 = LVL1MenuItem('L1_MU4', ctpid=32).setLogic(thr_mu4.condition(1))
        item_mu5 = LVL1MenuItem('L1_MU5', ctpid=33).setLogic(thr_mu5.condition(1))
        item_mu6 = LVL1MenuItem('L1_MU6', ctpid=34).setLogic(thr_mu6.condition(1))
        triggerPythonConfig.addLVL1Item(item_mu4)
        triggerPythonConfig.addLVL1Item(item_mu5)
        triggerPythonConfig.addLVL1Item(item_mu6)
    else:
        thr_mu1 = triggerPythonConfig.addLvl1Threshold('MU1','MUON',0,'SLOT9','CON0')\
                  .addMuonThresholdValue(4, -49, 49, 0, 64)
        thr_mu2 = triggerPythonConfig.addLvl1Threshold('MU2', 'MUON', 1, 'SLOT9', 'CON0')\
                  .addMuonThresholdValue(5, -49, 49, 0, 64)
        thr_mu3 = triggerPythonConfig.addLvl1Threshold('MU3', 'MUON', 2, 'SLOT9', 'CON0')\
                  .addMuonThresholdValue(6, -49, 49, 0, 64)
        thr_mu4 = triggerPythonConfig.addLvl1Threshold('MU4','MUON',3,'SLOT9','CON0')\
                  .addMuonThresholdValue(4, -49, 49, 0, 64)
        thr_mu5 = triggerPythonConfig.addLvl1Threshold('MU5', 'MUON', 4, 'SLOT9', 'CON0')\
                  .addMuonThresholdValue(5, -49, 49, 0, 64)
        thr_mu6 = triggerPythonConfig.addLvl1Threshold('MU6', 'MUON', 5, 'SLOT9', 'CON0')\
                  .addMuonThresholdValue(6, -49, 49, 0, 64)
        item_mu1 = LVL1MenuItem('1MU1', ctpid=0).setLogic(thr_mu1.condition(1))
        item_mu2 = LVL1MenuItem('1MU2', ctpid=1).setLogic(thr_mu2.condition(1))
        item_mu3 = LVL1MenuItem('1MU3', ctpid=2).setLogic(thr_mu3.condition(1))
        item_mu4 = LVL1MenuItem('1MU4', ctpid=3).setLogic(thr_mu4.condition(1))
        item_mu5 = LVL1MenuItem('1MU5', ctpid=4).setLogic(thr_mu5.condition(1))
        item_mu6 = LVL1MenuItem('1MU6', ctpid=5).setLogic(thr_mu6.condition(1))
        triggerPythonConfig.addLVL1Item(item_mu1)
        triggerPythonConfig.addLVL1Item(item_mu2)
        triggerPythonConfig.addLVL1Item(item_mu3)
        triggerPythonConfig.addLVL1Item(item_mu4)
        triggerPythonConfig.addLVL1Item(item_mu5)
        triggerPythonConfig.addLVL1Item(item_mu6)
