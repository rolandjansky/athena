#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.etree.cElementTree as ET
import sys
from ROOT import TH1F,TFile

output_file = TFile("L1Summary.root","recreate")
allItems = TH1F("AllItems", "AllItems", 512, 0, 512)

EM  = TH1F("EM",  "EM",  20,0,20)
MU  = TH1F("MU",  "MU",  20,0,20)
Jet = TH1F("Jet", "Jet", 20,0,20)
FJ  = TH1F("FJ",  "FJ",  20,0,20)
Tau = TH1F("Tau", "Tau", 20,0,20)
Mon = TH1F("Mon", "Mon", 20,0,20)
XE  = TH1F("XE",  "XE",  20,0,20)
JE  = TH1F("JE",  "JE",  20,0,20)
TE  = TH1F("TE",  "TE",  20,0,20)
XS  = TH1F("XS",  "XS",  20,0,20)

EM_X  = TH1F("EM_X",  "EM_X",  20,0,20)
MU_X  = TH1F("MU_X",  "MU_X",  20,0,20)
Jet_X = TH1F("Jet_X", "Jet_X", 20,0,20)
Tau_X = TH1F("Tau_X", "Tau_X", 20,0,20)
XE_X  = TH1F("XE_X",  "XE_X",  20,0,20)
XS_X  = TH1F("XS_X",  "XS_X",  20,0,20)

EM_Multi  = TH1F("EM_Multi_Etc",  "EM_Multi_Etc",  20,0,20)
MU_Multi  = TH1F("MU_Multi_Etc",  "MU_Multi_Etc",  20,0,20)
Jet_Multi = TH1F("Jet_Multi_Etc", "Jet_Multi_Etc", 20,0,20)
FJ_Multi  = TH1F("FJ_Multi_Etc",  "FJ_Multi_Etc",  20,0,20)
Tau_Multi = TH1F("Tau_Multi_Etc", "Tau_Multi_Etc", 20,0,20)


class Ddict(dict):
    def __init__(self, default=None):
        self.default = default

    def __getitem__(self, key):
        if not self.has_key(key):
            self[key] = self.default()
        return dict.__getitem__(self, key)

info_xml = Ddict(dict)
info_xml_thr = Ddict(dict)
menu_name = []

print
print 'Comparing the two following menus:'

number_of_items = 0

for i in range(1,3):
    LVL1_XML = ET.parse(sys.argv[i])
    LVL1_INFO = LVL1_XML.getroot()
    for e in LVL1_INFO.findall('TriggerMenu/TriggerItem'):
        number_of_items+=1
        name = e.get('name')
        ctpid = e.get('ctpid')
        trigger_type = e.get('trigger_type')
        info_xml[i][ctpid]={name:trigger_type}
    for e in LVL1_INFO.findall('TriggerThresholdList/TriggerThreshold'):
        bitnum = e.get('bitnum')
        id = e.get('id')
        name = e.get('name')
        mapping = e.get('mapping')
        info_xml_thr[i][name]={id:{bitnum:mapping}}
    menu_name.append(LVL1_INFO.get('name'))
    print 'Menu (%1s) %15s, #Items %3s ' % (i,menu_name[i-1],str(number_of_items)) 
    number_of_items=0



print
print "**************** LVL1 ITEMS ******************* "
print "*********************************************** "

print '%35s == %5s == %35s %25s %10s' % (menu_name[0]+' (1)', 'CTPID', menu_name[1]+' (2)', 'tag','DIFFs')
print



for i_ in range(512):
    i=str(i_)
    if not info_xml[1].has_key(i):
        info_xml[1][i]={'**NONE**':'**NONE'}
    if not info_xml[2].has_key(i):
        info_xml[2][i]={'**NONE**':'**NONE'}    
    dif_string = ''

    for name in info_xml[2][i]:
        if not info_xml[2][i]==info_xml[1][i] :
            dif_string=' *** DIFF *** '
            if info_xml[2][i].keys()==info_xml[1][i].keys():
                dif_string=' *** DIFF in TT *** : '+info_xml[1][i][name]+' vs. '+info_xml[2][i][name] 

    tag = []
    for xml_i in range(1,3):

        item_name = info_xml[xml_i][i].keys()[0].strip('EMPTY')
        
        try_tag = 'MBTS'
        if 'MBTS' in  item_name:
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'NIM'        
        if 'NIM' in  item_name:
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'ZB'        
        if 'ZB' in  item_name:
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Random'        
        if 'RD' in  item_name:
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Calib'        
        if 'CALREQ' in  item_name:
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Lumi,Mon'        
        if any(x in item_name for x in ['ZDC','LUC','BCM','LZ','BPTX','BGRP','TR','Mon']):
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Jets'
        if 'J' in item_name  and not any(x in item_name for x in ['JE','EM','MU','TAU','FJ','XE','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'EM'
        if 'EM' in item_name  and not any(x in item_name for x in ['JE','J','MU','TAU','FJ','XE','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'XS'
        if 'XS' in item_name :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'EM+X'
        if 'EM' in item_name  and any(x in item_name for x in ['JE','J','MU','TAU','FJ','XE','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'JE'
        if 'JE' in item_name  and not any(x in item_name for x in ['XE','EM','MU','TAU','FJ','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'FJ'
        if 'FJ' in item_name  and not any(x in item_name for x in ['XE','EM','MU','TAU','JE','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'XE'
        if 'XE' in item_name  and not any(x in item_name for x in ['JE','J','EM','MU','TAU','FJ','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'XE+X'
        if 'XE' in item_name  and any(x in item_name for x in ['JE','J','EM','TAU','FJ','MU','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Jets+X'
        if 'J' in item_name  and any(x in item_name for x in ['JE','XE','EM','TAU','MU','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'TE'
        if 'TE' in item_name  and not any(x in item_name for x in ['JE','J','EM','MU','TAU','FJ','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Muons'
        if 'MU' in item_name  and not any(x in item_name for x in ['JE','J','EM','TAU','FJ','XE','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Muons+X'
        if 'MU' in item_name  and any(x in item_name for x in ['JE','J','EM','TAU','FJ','XE','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Taus'
        if 'TAU' in item_name  and not any(x in item_name for x in ['JE','J','EM','MU','FJ','XE','XS']) :
            if try_tag not in tag:
                tag.append(try_tag)

        try_tag = 'Taus+X'
        if 'TAU' in item_name  and any(x in item_name for x in ['JE','J','EM','MU','FJ','XE','XS']) : 
            if try_tag not in tag:
                tag.append(try_tag)

##        try_tag = 'Jets+XE'
##        if all(y in item_name for y in ['J','XE']) and not any(x in item_name for x in ['JE','EM','MU','TAU','FJ']):
##            if try_tag not in tag:
##                tag.append(try_tag)
##
##        try_tag = 'Jets+EM'
##        if all(y in item_name for y in ['J','EM']) and not any(x in item_name for x in ['JE','XE','MU','TAU','FJ']):
##            if try_tag not in tag:
##                tag.append(try_tag)
##            
##        try_tag = 'Jets+MU'
##        if all(y in item_name for y in ['J','MU']) and not any(x in item_name for x in ['JE','EM','XE','TAU','FJ']): 
##            if try_tag not in tag:
##                tag.append(try_tag)
##            
##        try_tag = 'Taus'
##        if 'TAU' in item_name :
##            if try_tag not in tag:
##                tag.append(try_tag)
##
            
    print_tag = ' '.join(i for i in tag)
#    print '%35s -- %25s' % (info_xml[2][i].keys()[0], print_tag)
#print '%35s == %5s == %35s -- %25s %10s' % (info_xml[1][i].keys()[0],i,info_xml[2][i].keys()[0], print_tag, dif_string)

    new_tag = []
    
    allItems.Fill(info_xml[2][i].keys()[0],0)

    if "EM" in print_tag and not 'X' in print_tag and '_EM' in info_xml[2][i].keys()[0] and len(info_xml[2][i].keys()[0])<10:
        EM.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("SingleEM")
    elif "EM" in print_tag and not '+' in print_tag :
        EM_Multi.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("MultiEM")
    if "Muons" in print_tag and not 'X' in print_tag and '_MU' in info_xml[2][i].keys()[0] and len(info_xml[2][i].keys()[0])<10:
        MU.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("SingleMU")
    elif "Muons" in print_tag and not '+' in print_tag :
        MU_Multi.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("MultiMU")
    if "Tau" in print_tag and not 'X' in print_tag and '_TAU' in info_xml[2][i].keys()[0] and len(info_xml[2][i].keys()[0])<10:
        Tau.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("SingleTau")
    elif "Tau" in print_tag and not '+' in print_tag :
        Tau_Multi.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("MultiTau")
    if "Jet" in print_tag and not 'X' in print_tag and '_J' in info_xml[2][i].keys()[0] and len(info_xml[2][i].keys()[0])<10:
        Jet.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("SingleJet")
    elif "Jet" in print_tag and not '+' in print_tag :
        Jet_Multi.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("MultiJet")
    if "FJ" in print_tag and not 'X' in print_tag and '_FJ' in info_xml[2][i].keys()[0] and len(info_xml[2][i].keys()[0])<10:
        FJ.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("SingleFJ")
    elif "FJ" in print_tag and not '+' in print_tag :
        FJ_Multi.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("MultiFJ")
    if "XE" in print_tag and len(info_xml[2][i].keys()[0])<10:
        XE.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("OnlyXE")
    if "TE" in print_tag and len(info_xml[2][i].keys()[0])<10:
        TE.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("OnlyTE")
    if "JE" in print_tag and len(info_xml[2][i].keys()[0])<10:
        JE.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("singleJE")
    if "XS" in print_tag and len(info_xml[2][i].keys()[0])<10:
        XS.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("OnlyXS")


    if "EM" in print_tag and '+' in print_tag:
        EM_X.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("X_EM")
    if "Muons" in print_tag and '+' in print_tag:
        MU_X.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("X_MU")
    if "Tau" in print_tag and '+' in print_tag:
        Tau_X.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("X_TAU")
    if "Jet" in print_tag and '+' in print_tag :
        Jet_X.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("X_Jet")
    if "XE" in print_tag and '+' in print_tag:
        XE_X.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("X_XE")
    if "XS" in print_tag and '+' in print_tag:
        XS_X.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("X_XS")

    elif any(x in print_tag for x in ['Mon','MBTS','ZB','Random']):
        Mon.Fill(info_xml[2][i].keys()[0],0)
        new_tag.append("Mon")


    print_tag_new = ' '.join(i for i in new_tag)
#    print '%35s -- %25s' % (info_xml[2][i].keys()[0], print_tag_new)
    print '%35s == %5s == %35s -- %25s %10s' % (info_xml[1][i].keys()[0],i,info_xml[2][i].keys()[0], print_tag_new, dif_string)



output_file.Write()
output_file.Close()
print
print
print "**************** THRESHOLDS ******************* "
print "*********************************************** "

for i in range(1,3):
    for thr_ in info_xml_thr[i]:
        thr = str(thr_)
        if not info_xml_thr[2].has_key(thr):
            info_xml_thr[2][thr]={-1:{-1:-1}}
            
        if not info_xml_thr[1].has_key(thr):
            info_xml_thr[1][thr]={-1:{-1:-1}}
    

print '%22s == %10s == %22s ' % (menu_name[0]+' (1)', 'THR', menu_name[1]+' (2)')
print '%22s == %10s == %22s ' % ('{id:{bitnum:mapping}}', 'THR', '{id:{bitnum:mapping}}')
print 

for thr in info_xml_thr[1]:
    dif_string = ''    
    if info_xml_thr[2][thr]=={-1:{-1:-1}} or info_xml_thr[1][thr]=={-1:{-1:-1}}:
        dif_string=' *** DIFF *** '
    if info_xml_thr[2][thr]=={-1:{-1:-1}}:
        print '%22s == %10s == %22s %6s' % (info_xml_thr[1][thr],thr,'-- ABSENT --',dif_string)
    elif info_xml_thr[1][thr]=={-1:{-1:-1}}:
        print '%22s == %10s == %22s %6s' % ('-- ABSENT --',thr,info_xml_thr[2][thr],dif_string)
    else:
        print '%22s == %10s == %22s %6s' % (info_xml_thr[1][thr],thr,info_xml_thr[2][thr],dif_string)


print
