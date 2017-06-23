#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## from AthenaCommon.AppMgr import ToolSvc
## from AthenaCommon.Include import include
## TRUE = True
## FALSE = False

from TriggerMenu.menu.Lvl1 import Lvl1
from TriggerMenu.menu.Lvl1Flags import Lvl1Flags
from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.Logging import logging

from TriggerMenu.bjet.BjetSliceFlags import BjetSliceFlags
from TriggerMenu.bphysics.BphysicsSliceFlags import BphysicsSliceFlags
from TriggerMenu.menu.CombinedSliceFlags import CombinedSliceFlags
from TriggerMenu.egamma.EgammaSliceFlags import EgammaSliceFlags
from TriggerMenu.jet.JetSliceFlags import JetSliceFlags
from TriggerMenu.met.METSliceFlags import METSliceFlags
from TriggerMenu.minbias.MinBiasSliceFlags import MinBiasSliceFlags
from TriggerMenu.muon.MuonSliceFlags import MuonSliceFlags
from TriggerMenu.tau.TauSliceFlags import TauSliceFlags

l1thresholds_in_1e31 = [
    'MU4', 'MU6', 'MU10', 'MU11', 'MU20', 'MU40',
    'EM3', 'EM7', 'EM13', 'EM13I', 'EM18', 'EM18I', 'EM23I', 'EM100', 
    'HA5', 'HA6', 'HA9I', 'HA11I', 'HA16I', 'HA25', 'HA25I', 'HA40', 
    'J5', 'J10', 'J18', 'J23', 'J35', 'J42', 'J70', 'J100', 
    'JE120', 'JE220', 'JE280', 'JE340',
    'JB10', 'JB18', 'JB26', 'JB65', 'JF10', 'JF18', 'JF26', 'JF65', 
    'TE100', 'TE200', 'TE304', 'TE380', 
    'XE15', 'XE20', 'XE25', 'XE30', 'XE40', 'XE50', 'XE70', 'XE80', 
    ]
l1items_in_1e31 = [
    # EM
    'L1_EM3', 'L1_EM7', 'L1_EM13', 'L1_EM13I', 'L1_EM18',
    'L1_EM18I', 'L1_EM23I', 'L1_EM100', 
    'L1_2EM3', 'L1_2EM7', 'L1_2EM13', 'L1_2EM13I', 'L1_2EM18',
    'L1_2EM18I', 'L1_2EM23I', 'L1_2EM3_EM7', 'L1_3EM7',
    # MUON
    'L1_MU4', 'L1_MU6', 'L1_MU10', 'L1_MU11', 'L1_MU20', 'L1_MU40', 
    'L1_2MU4', 'L1_2MU6', 'L1_2MU10', 'L1_2MU11', 'L1_2MU20', 'L1_3MU6',
    # TAU
    'L1_TAU5', 'L1_TAU6', 'L1_TAU9I', 'L1_TAU11I', 'L1_TAU16I',
    'L1_TAU25', 'L1_TAU25I', 'L1_TAU40', 'L1_2TAU6', 
    'L1_2TAU9I', 'L1_2TAU6_TAU16I', 'L1_2TAU16I', 'L1_2TAU6_TAU25',
    # JET
    'L1_J5', 'L1_J10', 'L1_J18', 'L1_J23', 'L1_J35', 
    'L1_J42', 'L1_J70', 'L1_J100', 
    'L1_3J10', 'L1_4J10', 'L1_3J18', 'L1_4J18', 'L1_4J23',
    # FJET
    'L1_FJ10', 'L1_FJ18', 'L1_FJ26', 'L1_FJ65', 'L1_2FJ10', 
    'L1_2FJ18', 'L1_2FJ26',
    # XE
    'L1_XE15', 'L1_XE20', 'L1_XE25', 'L1_XE30', 'L1_XE40', 'L1_XE50',
    'L1_XE70', 'L1_XE80',
    # TE
    'L1_TE100', 'L1_TE200', 'L1_TE304', 'L1_TE380',
    # JE
    'L1_JE120', 'L1_JE220', 'L1_JE280', 'L1_JE340',
    # EM + X
    'L1_EM13_XE20', 'L1_EM18_XE15', 'L1_EM18_XE30', 
    'L1_2EM7_MU6', 'L1_2EM13_MU6', 'L1_2EM18_MU6', 
    'L1_EM7_MU6', 'L1_EM7_XE30', 'L1_2EM3_TAU6',
    'L1_EM13_MU11', 'L1_EM13I_MU11', 
    # TAU + EM
    'L1_2TAU6_EM7', 'L1_2TAU6_TAU9I_EM7', 'L1_2TAU6_TAU11I_EM7',
    'L1_2TAU9I_EM13I',
    # MU + X
    'L1_MU11_XE15', 'L1_MU20_XE30',
    'L1_MU4_J10', 'L1_MU4_J5', 'L1_MU4_EM3', 'L1_MU6_J5', 
    'L1_MU6_EM3', 'L1_2MU6_EM7',
    # TAU + MU
    'L1_TAU9I_MU6', 'L1_TAU9I_MU10', 'L1_TAU11I_MU6', 'L1_TAU11I_MU10',
    # JET + X
    'L1_J70_XE30', 'L1_2J42_XE30', 'L1_J42_XE30_EM13I', 'L1_J42_XE30_MU11',
    'L1_4J23_EM13I', 'L1_4J23_MU11', 
    # TAU + XE
    'L1_TAU6_XE20', 'L1_TAU9I_XE20', 'L1_TAU9I_XE30', 'L1_TAU9I_XE40',
    'L1_TAU11I_XE30', 'L1_TAU16I_XE20', 'L1_TAU16I_XE30', 'L1_TAU16I_XE40', 
    'L1_TAU25I_XE20','L1_TAU25_XE40',
    # TAU + JET
    'L1_TAU9I_2J5_J35', 'L1_TAU11I_2J5_J18', 'L1_TAU11I_2J5_J35',
    'L1_TAU9I_5J5_4J23',
    # Minimum bias
    'L1_RD0', 'L1_RD1', 
    ]
sigs_in_1e31 = [
    # Egamma
    "g3_nocut","g10","g15","g15i","g20","g20i",
    "g25","g25i","g105","2g10","2g15","2g20",
    "3g10","2g17i_tight", "g55_tight",
    'em15_passHLT', 'em15i_passHLT',
    '2em15_passHLT', 'em20_passHLT',
    '2em20_passHLT', 'em20i_passHLT',
    'em105_passHLT', 'e5', 'e10_tight',
    'e12', 'e15', 'e15i',
    'e20', 'e20_passL2', 'e20_passEF',
    'e20i', 'e20i_passL2', 'e20i_passEF',
    'e25i', 'e25i_passL2', 'e25i_passEF',
    'e105', '2e5',# 'e5_e7', 'e5_e10',
    '2e10', '2e15', '2e15_passEF',
    '2e15_passL2', '2e20', 'Zee', 'Jpsiee',
    '3e15', '3e15_passL2','3e15_passEF',
    'eNoCut', '2e12_tight',
    'e22i_tight', 'e55_tight',
    # Tau
    'tau10','tau10i','tau15','tau15i','tau20i','tau25i',
    'tau35i','tau45','tau45i','tau60','tau100',
    'tau15i_PT','tauNoCut',
    '2tau15i','2tau25i','2tau35i', 'tau10i_tau45','tau15i_tau45',
    'tau15i_tau35i','2tau25i_PT','tau15i_tau35i_PT',
    # Jet
    "j5","j10","j18","j23","j35","j42","j70",
    "j100","3j10","4j10","3j18","4j18","4j23",
    "fj10","fj18","fj26","fj65","2fj10","2fj18","2fj26",
    "je120","je220","je280","je340",
    # Bjet
    'b10','b18','b23','b35',
    '2b10_3L1Jets', '3b10_3L1Jets', '3b10_4L1Jets',
    '4b10_4L1Jets', '2b18_3L1Jets', '3b18_3L1Jets',
    '3b18_4L1Jets', '4b18_4L1Jets', '3b23_4L1Jets',
    '4b23_4L1Jets',
    # Muon
    'mu4', 'mu6', 'mu6l', 'mu10', 'mu15', 'mu20',
    'mu40', 'stau','2mu6', '2mu20', '3mu6',
    # MET
    "te100", "te200", "te304", "te380",
    "xe15", "xe20", "xe25", "xe30", "xe40",
    "xe50", "xe70", "xe80",
    # Bphysics
    "MU6_DsPhiPi", "MU4_DsPhiPi", "mu6_DsPhiPi", "mu4_DsPhiPi",
    "MU6_DsPhiPi_FS", "MU4_DsPhiPi_FS",  "mu6_DsPhiPi_FS", "mu4_DsPhiPi_FS",
    "MU4_Jpsimumu", "MU6_Jpsimumu","MU4_Bmumu","MU6_Bmumu",
    "mu4_Jpsimumu", "mu6_Jpsimumu","mu4_Bmumu","mu6_Bmumu",
    "2MU4_Bmu4mu4","2MU6_Bmu6mu6","2MU4_Bmu4mu4x","2MU6_Bmu6mu6x",
    "2MU4_Jpsimumu","2MU6_Jpsimumu", "2mu4_Jpsimumu", "2mu6_Jpsimumu",
    # Minbias
    'minbias', 
    # Combined
    'tau15i_e10', 'tau20i_e10', 'tau25i_e10',
    'tau20i_e15i','tau25i_e15i','tau25i_e25i',
    'tau20i_mu6','tau20i_mu10', 'tau25i_mu6','tau25i_mu10',
    'tau15i_xe20', 'tau20i_xe30','L1_TAU9I_EF_xe30a','L1_TAU9I_EF_xe40a',
    'L1_TAU9I_EF_xe30b','L1_TAU9I_EF_xe30',
    'tau25i_xe30', 'tau25i_xe30_PT', 'tau35i_xe20',
    'tau35i_xe30','tau35i_xe40',
    'tau45i_xe20', 'tau45_xe40',
    '2e10_mu6', '2mu6_e10', 'e10_mu6',
    'e10_xe30','e15_xe20','e20_xe15','em20_xe15_PT','e20_xe30',
    'mu15_xe15','mu20_xe30',
    'g20_xe15','e20_g20','2g10_mu6','2mu6_g10','g25_xe30',
    'tau20i_4j23', 'tau20i_4j23_WO', 'tau20i_j70',
    'tau20i_j70_WO', 'tau25i_j70', 'tau25i_b18', 
    '4j23_e15i','4j23_mu15','mu4_j10',
    'j70_xe30','2j42_xe30','j42_xe30_e15i','j42_xe30_mu15'
    ]

print 'N signatures in 1E31: ', len(sigs_in_1e31)

thrs_final = []
items_final = []
sigs_final = []

lumifile='Lumi1E31_test.py'
fout = open(lumifile, 'w')
offset = '    '

def print2(m, offset=''):
    # print '%s%s' % (offset, m)
    fout.write('%s%s\n' % (offset, m))

def printSignatures(slice):
    print2('TriggerFlags.%s.signatures = [' % slice, offset)
    names = []
    sigs = []
    exec 'sigs = %sFlags.signatures.allowedValues.allowedList' % slice
    exec 'names = %sFlags.signatures.StoredValue' % slice
    for name in names:
        if name not in sigs:
            print "Signature '%s' in StoredValue not in AllowedList: " % name
            sigs.append(name)
    str = ''
    for i, s in enumerate(sigs):
        if s not in sigs_in_1e31:
            print "Skipping signature: '%s' (not in previous 1e31 menu)" % s
            continue
        sigs_final.append(s)
        str += "'%s', " % s
        if i!=0 and ( (i+1)%4)==0:
            print2('%s' % str, offset*2)
            str = ''
    if len(str)>0: print2('%s' % str, offset*2)
    print2(']', offset*2)

def printL1Thresholds():
    print2('Lvl1Flags.thresholds = [', offset)
    thrs = []
    exec 'thrs = Lvl1Flags.thresholds.allowedValues.allowedList'
    str = ''
    for i, thr in enumerate(thrs):
        if thr not in l1thresholds_in_1e31:
            print "Skipping threshold: '%s' (not in previous 1e31 menu)" % thr
            continue
        thrs_final.append(thr)
        str += "'%s', " % thr
        if i!=0 and ( (i+1)%4)==0:
            print2('%s' % str, offset*2)
            str = ''
    if len(str)>0: print2('%s' % str, offset*2)
    print2(']', offset*2)
    
def printL1Items():
    print2('Lvl1Flags.items = [', offset)
    items = []
    exec 'items = Lvl1Flags.items.allowedValues.allowedList'
    str = ''
    for i, item in enumerate(items):
        if item not in l1items_in_1e31:
            print "Skipping item: '%s' (not in previous 1e31 menu)" % item
            continue
        items_final.append(item)
        str += "'%s', " % item
        if i!=0 and ( (i+1)%4)==0:
            print2('%s' % str, offset*2)
            str = ''
    if len(str)>0: print2('%s' % str, offset*2)
    print2(']', offset*2)

def maxStringLength(words):
    max_length = 0
    for item_name in Lvl1Flags.items():
        if max_length < len(item_name): max_length = len(item_name)
    return max_length

    

log = logging.getLogger('dumpPrescales.py')

TriggerFlags.triggerMenuSetup = 'lumi0.01'

tm = TriggerPythonConfig('hlt.xml', 'lvl1.xml')

Lvl1.registerItems(tm)
# lumi(tm)
Lvl1.generateMenu(tm, l1items_in_1e31, l1thresholds_in_1e31)

sigs = [
    # '2e12_tight','2g17i_tight', 'g55_tight'
    ]
slice_flags = [
    MuonSliceFlags, 
    EgammaSliceFlags,
    TauSliceFlags,
    JetSliceFlags,
    BjetSliceFlags, 
    BphysicsSliceFlags, 
    METSliceFlags, 
    CombinedSliceFlags, 
    MinBiasSliceFlags, 
    ]
for sflags in slice_flags:
    for name in sflags.signatures.allowedValues.allowedList:
        if name not in sigs: sigs.append(name)
    for name in sflags.signatures.StoredValue:
        if name not in sigs: sigs.append(name)

# print2('class TriggerMenu_lumi1E31:')
print2('def setupLumi1E31():')
fout.write("""
    from TriggerMenu.menu.Lvl1 import Lvl1
    from TriggerMenu.menu.Lvl1Flags import Lvl1Flags
    from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from AthenaCommon.Logging import logging

    import TriggerMenu.MinBiasSliceFlags
\n""")

print2('TriggerFlags.Slices_all_setOff()', offset)
printL1Thresholds()
printL1Items()
printSignatures('MuonSlice')
printSignatures('EgammaSlice')
printSignatures('TauSlice')
printSignatures('JetSlice')
printSignatures('BjetSlice')
printSignatures('BphysicsSlice')
printSignatures('METSlice')
printSignatures('CombinedSlice')
printSignatures('MinBiasSlice')

print2('class Prescales:')
max_name_length = maxStringLength(Lvl1Flags.items())+1
format = '#   %%-%ds  | Prescale' % max_name_length
print2(format % ('Item name'), offset)
print2('#----------------------------------------------------------', offset)
print2('L1Prescales = {', offset)
for item_name in Lvl1Flags.items():
    item = tm.getLvl1Item(item_name)
    if item==None:
        print '# Warning! No item registered: ', item_name
        continue
    else:
        format="    '%%s'%%%ds: %%8d, " % (max_name_length-len(item_name))
        print2(format % (item.name, ' ', int(item.prescale)), offset)
print2('}', offset)

# Dump HLT menu
max_name_length = maxStringLength(sigs)+1
format = '#   %%-%ds  | [ L2prescale, EFprescale]' % max_name_length
print2(format % ('Signature name'), offset)
print2('#----------------------------------------------------------', offset)
print2('HLTPrescales = {', offset)
for sig_name in sigs:
    l2_ps, ef_ps = 1, 1
    format="    '%%s'%%%ds: [%%8d, %%8d], " % (max_name_length-len(sig_name))
    print2(format % (sig_name, ' ', int(l2_ps), int(ef_ps)), offset)
print2('}\n', offset)

print 'Number of L1 items: %s' % len(items_final)
print 'Number of HLT chains: %s' % len(sigs_final)

fout.close()
