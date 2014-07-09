#!/usr/bin/env python

import os
import sys
from TriggerMenuConfig.TriggerPythonConfig import *

tm = TriggerPythonConfig('hlt.xml', 'LVL1Config-CSC05.xml')

#===========================================================================
# TriggerThresholds
#------------------
# arguments to addLvl1Threshold()
# addLvl1Threshold(name, type, bitnum, range_begin, range_end,
#                  slot, connector)
#
# To Switch off the threshold set following constants for each type of
# threshold
# ClusterOff   (= 255)
# IsolationOff (= 63)
# JetOff       (= 1023)
# EtSumOff     (= 2047)
# EtMissOff    (= 2895)
# JetEtOff     (= 13286)
#
#----- MUON thresholds -----
mu6 = tm.addLvl1Threshold('MU6', 'MUON', 0,'SLOT9','CON0')\
      .addMuonThresholdValue(6, -49, 49, 0, 64)
mu8 = tm.addLvl1Threshold('MU8', 'MUON', 1,'SLOT9','CON0')\
      .addMuonThresholdValue(8, -49, 49, 0, 64)
mu10 = tm.addLvl1Threshold('MU10', 'MUON', 2,'SLOT9','CON0')\
       .addMuonThresholdValue(10, -49, 49, 0, 64)
mu11 = tm.addLvl1Threshold('MU11', 'MUON', 3,'SLOT9','CON0')\
       .addMuonThresholdValue(12, -49, 49, 0, 64)
mu20 = tm.addLvl1Threshold('MU20','MUON', 4,'SLOT9','CON0')\
       .addMuonThresholdValue(20, -49, 49, 0, 64)
mu40 = tm.addLvl1Threshold('MU40','MUON', 5,'SLOT9','CON0')\
       .addMuonThresholdValue(40, -49, 49, 0, 64)
#----- EM thresholds -----
em1 = tm.addLvl1Threshold('EM1', 'EM', 0, 'SLOT7', 'CON1')\
      .addEMThresholdValue(7, -49, 49, 0, 64,\
                           IsolationOff,IsolationOff,IsolationOff)
em11 = tm.addLvl1Threshold('EM11', 'EM', 1, 'SLOT7', 'CON1')\
       .addEMThresholdValue(11, -49, 49, 0, 64, \
                            IsolationOff,IsolationOff,IsolationOff)
em11i = tm.addLvl1Threshold('EM11I', 'EM', 2, 'SLOT7', 'CON1')\
        .addEMThresholdValue(11, -49, 49, 0, 64, 3, 2, 2)
em18 = tm.addLvl1Threshold('EM18', 'EM', 3, 'SLOT7', 'CON1')\
       .addEMThresholdValue(18, -49, 49, 0, 64, \
                            IsolationOff,IsolationOff,IsolationOff)
em18i = tm.addLvl1Threshold('EM18I', 'EM', 4, 'SLOT7', 'CON1')\
        .addEMThresholdValue(18, -49, 49, 0, 64, 3, 2, 2)
em50 = tm.addLvl1Threshold('EM50', 'EM', 5, 'SLOT7', 'CON1')\
       .addEMThresholdValue(50, -49, 49, 0, 64, \
                            IsolationOff,IsolationOff,IsolationOff, 1)
#----- TAU thresholds -----
tau5 = tm.addLvl1Threshold('TAU5', 'TAU', 0, 'SLOT7', 'CON2')\
       .addTauThresholdValue(5, -49, 49, 0, 64, \
                             IsolationOff, IsolationOff, IsolationOff)
tau8 = tm.addLvl1Threshold('TAU8', 'TAU', 1, 'SLOT7', 'CON2')\
       .addTauThresholdValue(8, -49, 49, 0, 64, \
                             IsolationOff, IsolationOff, IsolationOff)
tau8i = tm.addLvl1Threshold('TAU8I', 'TAU', 2, 'SLOT7', 'CON2')\
        .addTauThresholdValue(8, -49, 49, 0, 64, \
                              5, IsolationOff, IsolationOff)
tau13 = tm.addLvl1Threshold('TAU13', 'TAU', 3, 'SLOT7', 'CON2')\
        .addTauThresholdValue(13, -49, 49, 0, 64, \
                              IsolationOff, IsolationOff, IsolationOff)
tau13i = tm.addLvl1Threshold('TAU13I', 'TAU', 4, 'SLOT7', 'CON2')\
         .addTauThresholdValue(13, -49, 49, 0, 64, \
                               5, IsolationOff, IsolationOff)
tau17i = tm.addLvl1Threshold('TAU17I', 'TAU', 5, 'SLOT7', 'CON2')\
         .addTauThresholdValue(17, -49, 49, 0, 64, \
                               6, IsolationOff, IsolationOff)
tau22i = tm.addLvl1Threshold('TAU22I', 'TAU', 6, 'SLOT7', 'CON2')\
         .addTauThresholdValue(17, -49, 49, 0, 64,
                               6, IsolationOff, IsolationOff)
tau30i = tm.addLvl1Threshold('TAU30I', 'TAU', 7, 'SLOT7', 'CON2')\
         .addTauThresholdValue(30, -49, 49, 0, 64, \
                               6, IsolationOff, IsolationOff)

#----- JET thresholds -----
jet15 = tm.addLvl1Threshold('JET15', 'JET', 0, 'SLOT8', 'CON0')\
        .addJetThresholdValue(15, -49, 49, 0, 64, 8)
jet20 = tm.addLvl1Threshold('JET20', 'JET', 1, 'SLOT8', 'CON0')\
        .addJetThresholdValue(20, -49, 49, 0, 64, 8)
jet25 = tm.addLvl1Threshold('JET25', 'JET', 2, 'SLOT8', 'CON0')\
        .addJetThresholdValue(25, -49, 49, 0, 64, 8)
jet35 = tm.addLvl1Threshold('JET35', 'JET', 3, 'SLOT8', 'CON0')\
        .addJetThresholdValue(35, -49, 49, 0, 64, 8)
jet55 = tm.addLvl1Threshold('JET55', 'JET', 4, 'SLOT8', 'CON0')\
        .addJetThresholdValue(55, -49, 49, 0, 64, 8)
jet4 = tm.addLvl1Threshold('JET4', 'JET', 5, 'SLOT8', 'CON0')\
       .addJetThresholdValue(4, -49, 49, 0, 64, 4)
#----- FJET thresholds -----
jb10 = tm.addLvl1Threshold('JB10', 'JB', 0,'SLOT8','CON1')\
        .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
        .addJetThresholdValue(10, -49, 0, 0, 64, 8, 2)
jf10 = tm.addLvl1Threshold('JF10','JF',0,'SLOT8','CON1')\
        .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
        .addJetThresholdValue(10, 0, 49, 0, 64, 8, 2)
jb20 = tm.addLvl1Threshold('JB20', 'JB',1,'SLOT8','CON1')\
        .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
        .addJetThresholdValue(20, -49, 0, 0, 64, 8, 2)
jf20 = tm.addLvl1Threshold('JF20','JF',1,'SLOT8','CON1')\
        .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
        .addJetThresholdValue(20, 0, 49, 0, 64, 8, 2)
#----- TE thresholds -----
xe20 = tm.addLvl1Threshold('XE20', 'XE', 0, 'SLOT8', 'CON2')\
       .addEnergyThresholdValue(20, -49, 49, 0, 64)
xe30 = tm.addLvl1Threshold('XE30', 'XE', 1, 'SLOT8', 'CON2')\
       .addEnergyThresholdValue(30, -49, 49, 0, 64)
xe40 = tm.addLvl1Threshold('XE40', 'XE', 2, 'SLOT8', 'CON2')\
       .addEnergyThresholdValue(40, -49, 49, 0, 64)
xe50 = tm.addLvl1Threshold('XE50', 'XE', 3, 'SLOT8', 'CON2')\
       .addEnergyThresholdValue(50, -49, 49, 0, 64)
xe100 = tm.addLvl1Threshold('XE100', 'XE', 4, 'SLOT8', 'CON2')\
        .addEnergyThresholdValue(100, -49, 49, 0, 64)
xe200 = tm.addLvl1Threshold('XE200', 'XE', 5, 'SLOT8', 'CON2')\
        .addEnergyThresholdValue(200, -49, 49, 0, 64)
#----- XE thresholds -----
xe30 = tm.addLvl1Threshold('XE30', 'XE', 6, 'SLOT8', 'CON2')\
       .addEnergyThresholdValue(30, -49, 49, 0, 64)

#===========================================================================
# TriggerConditions
#------------------
cond_1mu6 = mu6.condition(1)
cond_1mu8 = mu8.condition(1)
cond_1mu10 = mu10.condition(1)
cond_1mu11 = mu11.condition(1)
cond_1mu20 = mu20.condition(1)
cond_1mu40 = mu40.condition(1)
cond_2mu6 = mu6.condition(2)
cond_2mu8 = mu8.condition(2)
cond_2mu10 = mu10.condition(2)
cond_2mu11 = mu11.condition(2)
cond_2mu20 = mu20.condition(2)
cond_2mu40 = mu40.condition(2)

cond_1j15 = jet15.condition(1)
cond_1j20 = jet20.condition(1)
cond_2j20 = jet20.condition(2)
cond_3j20 = jet20.condition(3)
cond_4j20 = jet20.condition(4)
cond_1fj10_left = jb10.condition(1)
cond_1fj10_right = jf10.condition(1)
cond_1bj15 = jet4.condition(1)

cond_1em1 = em1.condition(1)
cond_2em11 = em11.condition(2)
cond_2em11i = em11i.condition(2)
cond_1em18 = em18.condition(1)
cond_1em18i = em18i.condition(1)
cond_1em50 = em50.condition(1)

cond_1tau17i = tau17i.condition(1)
cond_xe30 = xe30.condition()
cond_1jet55 = jet55.condition(5)

cond_xe30 = xe30.condition(1)
cond_xe40 = xe40.condition(1)
cond_xe50 = xe50.condition(1)
cond_xe100 = xe100.condition(1)
cond_xe200 = xe200.condition(1)

#===========================================================================
# TriggerItems
#-------------
# EM Items (bits [0:31])
item_1em1 = LVL1MenuItem('L1_1EM1', 0x0, '1', 100, 'LOW')\
            .setLogic(cond_1em1).setTriggerType(1)
item_2em15 = LVL1MenuItem('L1_2EM15', 0x1, '1', 1, 'HIGH')\
             .setLogic(cond_2em11).setTriggerType(1)
item_2em15i = LVL1MenuItem('L1_2EM15i', 0x2, '1', 1, 'LOW')\
              .setLogic(cond_2em11i).setTriggerType(1)
item_1em25 = LVL1MenuItem('L1_2EM25', 0x3, '1', 1, 'LOW')\
             .setLogic(cond_1em18).setTriggerType(1)
item_1em25i = LVL1MenuItem('L1_2EM25i', 0x4, '1', 1, 'LOW')\
              .setLogic(cond_1em18i).setTriggerType(1)
item_1em60 = LVL1MenuItem('L1_1EM60', 0x5, '1', 1, 'LOW')\
             .setLogic(cond_1em50).setTriggerType(1)

item_xe20 = LVL1MenuItem('L1_XE20', 0x6, '1', 1, 'LOW')\
            .setLogic(xe20.condition(1)).setTriggerType(8)

# Muon Items (bits [32:63])
item_1mu6 = LVL1MenuItem('L1_1MU6', 0x20, '1', 1, 'LOW')\
            .setLogic(cond_1mu6).setTriggerType(2)
item_2mu6 = LVL1MenuItem('L1_2MU6', 0x21, '1', 1, 'LOW')\
            .setLogic(cond_2mu6).setTriggerType(2)
item_1mu8 = LVL1MenuItem('L1_1MU8', 0x22, '1', 1, 'LOW')\
            .setLogic(cond_1mu8).setTriggerType(2)
item_1mu10 = LVL1MenuItem('L1_1MU10', 0x23, '1', 1, 'LOW')\
             .setLogic(cond_1mu10).setTriggerType(2)
item_1mu11 = LVL1MenuItem('L1_1MU11', 0x24, '1', 1, 'LOW')\
             .setLogic(cond_1mu11).setTriggerType(2)
item_1mu20 = LVL1MenuItem('L1_1MU20', 0x25, '1', 1, 'LOW')\
             .setLogic(cond_1mu20).setTriggerType(2)
item_1mu40 = LVL1MenuItem('L1_1MU40', 0x26, '1', 1, 'LOW')\
             .setLogic(cond_1mu40).setTriggerType(2)
#item_1mu6 = LVL1MenuItem('L1_1MU6', 0x20, '1', 1, 'LOW')\
#            .setLogic(cond_1mu6|cond_1mu8|cond_1mu10|cond_1mu11|cond_1mu20|
#                      cond_1mu40)

## item_1mu6 = LVL1MenuItem('L1_1MU6', 0x20, '1', 1, 'LOW')\
##             .addOredCondition('L1_1MU6', 1, 'MU6')

## item_2mu6 = LVL1MenuItem('L1_2MU6', 0x21, '1', 1, 'LOW')\
##             .setLogic( (cond_2mu6|cond_2mu8|cond_2mu10|cond_2mu11|
##                         cond_2mu20|cond_2mu40) |
##                        (cond_1mu6 & (cond_1mu8|cond_1mu10|cond_1mu11|
##                                      cond_1mu20|cond_1mu40)) |
##                        (cond_1mu8 & (cond_1mu10|cond_1mu11|cond_1mu20|
##                                      cond_1mu40)) |
##                        (cond_1mu10 & (cond_1mu11|cond_1mu20|cond_1mu40)) |
##                        (cond_1mu11 & (cond_1mu20|cond_1mu40)) |
##                        (cond_1mu20 & cond_1mu40))
## item_1mu8 = LVL1MenuItem('L1_1MU8', 0x22, '1', 1, 'LOW')\
##             .setLogic(cond_1mu8|cond_1mu10|cond_1mu11|cond_1mu20|cond_1mu40)
## item_1mu10 = LVL1MenuItem('L1_1MU10', 0x23, '1', 1, 'LOW')\
##              .setLogic(cond_1mu10|cond_1mu11|cond_1mu20|cond_1mu40)
## item_1mu11 = LVL1MenuItem('L1_1MU11', 0x24, '1', 1, 'LOW')\
##              .setLogic(cond_1mu11|cond_1mu20|cond_1mu40)
## item_1mu20 = LVL1MenuItem('L1_1MU20', 0x25, '1', 1, 'LOW')\
##              .setLogic(cond_1mu20|cond_1mu40)
## item_1mu40 = LVL1MenuItem('L1_1MU40', 0x26, '1', 1, 'LOW').setLogic(cond_1mu40)

# Tau Items (bits [64:95])
item_1tau5 = LVL1MenuItem('L1_1TAU5', 0x40, '1', 1, 'LOW')\
             .setLogic(tau5.condition(1)).setTriggerType(4)
item_1tau10 = LVL1MenuItem('L1_1TAU10', 0x41, '1', 1, 'LOW')\
              .setLogic(tau8.condition(1)).setTriggerType(4)
item_1tau10i = LVL1MenuItem('L1_1TAU10i', 0x42, '1', 1, 'LOW')\
               .setLogic(tau8i.condition(1)).setTriggerType(4)
item_1tau15 = LVL1MenuItem('L1_1TAU15', 0x43, '1', 1, 'LOW')\
              .setLogic(tau13.condition(1)).setTriggerType(4)
item_1tau15i = LVL1MenuItem('L1_1TAU15i', 0x44, '1', 1, 'LOW')\
               .setLogic(tau13i.condition(1)).setTriggerType(4)
item_1tau20i = LVL1MenuItem('L1_1TAU20i', 0x45, '1', 1, 'LOW')\
               .setLogic(tau17i.condition(1)).setTriggerType(4)
item_1tau25i = LVL1MenuItem('L1_1TAU25i', 0x46, '1', 1, 'LOW')\
               .setLogic(tau22i.condition(1)).setTriggerType(4)
item_1tau35i = LVL1MenuItem('L1_1TAU35i', 0x47, '1', 1, 'LOW')\
               .setLogic(tau30i.condition(1)).setTriggerType(4)


# Jet items (bits [96:127])
item_1j35 = LVL1MenuItem('L1_1J35', 0x60, '1', 1, 'LOW')\
            .setLogic(cond_1j15).setTriggerType(0x10)
item_1j45 = LVL1MenuItem('L1_1J45', 0x61, '1', 1, 'LOW')\
            .setLogic(cond_1j20).setTriggerType(0x10)
item_2j45 = LVL1MenuItem('L1_2J45', 0x62, '1', 1, 'LOW')\
            .setLogic(cond_2j20).setTriggerType(0x10)
item_3j45 = LVL1MenuItem('L1_3J45', 0x63, '1', 1, 'LOW')\
            .setLogic(cond_3j20).setTriggerType(0x10)
item_4j45 = LVL1MenuItem('L1_4J45', 0x64, '1', 1, 'LOW')\
            .setLogic(cond_4j20).setTriggerType(0x10)
item_1fj30 = LVL1MenuItem('L1_1fj30', 0x65, '1', 1, 'LOW')\
             .setLogic(cond_1fj10_right & cond_1fj10_left).setTriggerType(0x10)
item_1bj15 = LVL1MenuItem('L1_1bj15', 0x66, '1', 1, 'LOW')\
             .setLogic(cond_1bj15).setTriggerType(0x10)



# Etmiss items (bits [128:159])
item_xe30 = LVL1MenuItem('L1_XE30', 0x80, '1', 1, 'LOW')\
            .setLogic(cond_xe30).setTriggerType(0x20)
item_xe40 = LVL1MenuItem('L1_XE40', 0x81, '1', 1, 'LOW')\
            .setLogic(cond_xe40).setTriggerType(0x20)
item_xe50 = LVL1MenuItem('L1_XE50', 0x82, '1', 1, 'LOW')\
            .setLogic(cond_xe50).setTriggerType(0x20)
item_xe100 = LVL1MenuItem('L1_XE100', 0x83, '1', 1, 'LOW')\
             .setLogic(cond_xe100).setTriggerType(0x20)
item_xe200 = LVL1MenuItem('L1_XE200', 0x84, '1', 1, 'LOW')\
             .setLogic(cond_xe200).setTriggerType(0x20)

item_1tau17i_xe30 = LVL1MenuItem('L1_1TAU17I_XE30', 0x8f, '1', 1, 'LOW')\
                    .setLogic(cond_1tau17i&cond_xe30&Not(cond_1jet55))\
                    .setTriggerType(0x24)


item_bgrp0 = LVL1MenuItem('L1_BGRP0', 0xa0, '1', 1, 'LOW').setLogic(bgrp0)
item_bgrp1 = LVL1MenuItem('L1_BGRP1', 0xa1, '1', 1, 'LOW').setLogic(bgrp1)
item_bgrp2 = LVL1MenuItem('L1_BGRP2', 0xa2, '1', 1, 'LOW').setLogic(bgrp2)
item_bgrp3 = LVL1MenuItem('L1_BGRP3', 0xa3, '1', 1, 'LOW').setLogic(bgrp3)
item_bgrp4 = LVL1MenuItem('L1_BGRP4', 0xa4, '1', 1, 'LOW').setLogic(bgrp4)
item_bgrp5 = LVL1MenuItem('L1_BGRP5', 0xa5, '1', 1, 'LOW').setLogic(bgrp5)
item_bgrp6 = LVL1MenuItem('L1_BGRP6', 0xa6, '1', 1, 'LOW').setLogic(bgrp6)
item_bgrp7 = LVL1MenuItem('L1_BGRP7', 0xa7, '1', 1, 'LOW').setLogic(bgrp7)
item_mu10rndm0 = LVL1MenuItem('L1_MU6RNDM0', 0xb0, '1', 1, 'LOW')\
                 .setLogic(cond_1mu6&rndm0)

tm.addLVL1Item(item_bgrp0)
tm.addLVL1Item(item_bgrp1)
tm.addLVL1Item(item_bgrp2)
tm.addLVL1Item(item_bgrp3)
tm.addLVL1Item(item_bgrp4)
tm.addLVL1Item(item_bgrp5)
tm.addLVL1Item(item_bgrp6)
tm.addLVL1Item(item_bgrp7)
tm.addLVL1Item(item_mu10rndm0)

#---------------------------------------------------------------------------
# Setting items to the menu
tm.addLVL1Item(item_1em1)
tm.addLVL1Item(item_2em15)
tm.addLVL1Item(item_2em15i)
tm.addLVL1Item(item_1em25)
tm.addLVL1Item(item_1em25i)
tm.addLVL1Item(item_1em60)
tm.addLVL1Item(item_xe20)
#
tm.addLVL1Item(item_1mu6)
tm.addLVL1Item(item_2mu6)
tm.addLVL1Item(item_1mu8)
tm.addLVL1Item(item_1mu10)
tm.addLVL1Item(item_1mu11)
tm.addLVL1Item(item_1mu20)
tm.addLVL1Item(item_1mu40)
#
tm.addLVL1Item(item_1tau5)
tm.addLVL1Item(item_1tau10)
tm.addLVL1Item(item_1tau10i)
tm.addLVL1Item(item_1tau15)
tm.addLVL1Item(item_1tau15i)
tm.addLVL1Item(item_1tau20i)
tm.addLVL1Item(item_1tau25i)
tm.addLVL1Item(item_1tau35i)
#
tm.addLVL1Item(item_1tau17i_xe30)

#===========================================================================
# Other CTP configuration
#------------------------
tm.CTPInfo().setRandom('rand1', rate1=1000, rate2=100000, seed1=1, seed2=1)
tm.CTPInfo().setPrescaledClock('psc1', clock1=10, clock2=100)
tm.CTPInfo().setDeadtime(name='dt1', simple=0,
                          complex1_rate=10, complex1_level=0,
                          complex2_rate=20, complex2_level=1)
bgs = tm.CTPInfo().setBunchGroupSet('bg_set1')\
      .addBunchGroup('bg_p1', 1, [1, 2])\
      .addBunchGroup('bg_p2', 2, [1001, 2002, 3003])\
      .addBunchGroup('bg_empty', 2, [101, 202, 302])
#tm.CTPInfo().setTriggerType(0, 0x1234)
#tm.CTPInfo().setTriggerType(1, 0x0)
#tm.CTPInfo().setTriggerType(2, 0x0)
#tm.CTPInfo().setTriggerType(3, 0x0)
#tm.CTPInfo().setTriggerType(4, 0x0)
#tm.CTPInfo().setTriggerType(5, 0x0)
#tm.CTPInfo().setTriggerType(6, 0x124<<128)
#tm.CTPInfo().setTriggerType(7, 0x0)
#===========================================================================
# Muctpi
#-------
# tm.setMuctpiInfo(low_pt=2, high_pt=6, max_cand=13)

#===========================================================================
# CaloInfo
#---------
tm.Lvl1CaloInfo().setName('standard')
tm.Lvl1CaloInfo().setGlobalScale(1)
#--- ALGORITHM FOR CALCULATING JET WEIGHTS IS NOT WORKING YET!!!!!
#jet_weights = tm.Lvl1CaloInfo().calcJetWeights(tm.Lvl1Thresholds(), \
#                                               Lvl1CaloInfo.AverageOfThr)
#tm.Lvl1CaloInfo().setJetWeights(jet_weights)
tm.Lvl1CaloInfo().setJetWeights([4, 15, 20, 25, 35, 55, 0, 0, 10, 20, 30, 40])

tm.writeConfigFiles()
