# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class Bphysics_Lvl1 :
     def __init__(self) :
       pass

     def generateMenu(self, triggerPythonConfig) :

        thr_mu0  = tm.registerLvl1Threshold(name='MU0', type='MUON', mapping=0, slot='SLOT9', connector='CON0')\
                   .addMuonThresholdValue(4, -49, 49, 0, 64)
        thr_mu6  = tm.registerLvl1Threshold(name='MU6', type='MUON', mapping=1, slot='SLOT9', connector='CON0')\
                   .addMuonThresholdValue(6, -49, 49, 0, 64)
 
        thr_em3   = tm.registerLvl1Threshold(name='EM3', type='EM', mapping=0, slot='SLOT7', connector='CON1')\
            .addEMThresholdValue(3, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_j5 = tm.registerLvl1Threshold('J5', 'JET', mapping=0, slot='SLOT8', connector='CON0')\
                 .addJetThresholdValue(5, -49, 49, 0, 64, 4)
        item_em3   = LVL1MenuItem('L1_EM3',   ctpid=0x0).setLogic(thr_em3.condition(1))
        item_mu0  = LVL1MenuItem('L1_MU0',  ctpid=0x20).setLogic(thr_mu0.condition(1))
        item_mu6  = LVL1MenuItem('L1_MU6',  ctpid=0x21).setLogic(thr_mu6.condition(1))
        item_j5   = LVL1MenuItem('L1_J5',   ctpid=0x60, group='1', prescale=1000000).setLogic(thr_j5.condition(1))

        item_mu0_j5    = LVL1MenuItem('L1_MU0_J5', ctpid=0xb2)\
                         .setLogic(thr_mu0.condition(1)&thr_j5.condition(1))
        item_mu0_em3   = LVL1MenuItem('L1_MU0_EM3', ctpid=0xb3)\
                         .setLogic(thr_mu0.condition(1)&thr_em3.condition(1))
        item_mu6_j5    = LVL1MenuItem('L1_MU6_J5', ctpid=0xb4)\
                         .setLogic(thr_mu6.condition(1)&thr_j5.condition(1))
        item_mu6_em3   = LVL1MenuItem('L1_MU6_EM3', ctpid=0xb5)\
                         .setLogic(thr_mu6.condition(1)&thr_em3.condition(1))
