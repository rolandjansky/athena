#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.etree.cElementTree as ET
import sys
from ROOT import TH1F,TFile

output_file = TFile("L1Summary.root","recreate")
allItems = TH1F("AllItems", "AllItems", 256, 0, 256)

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
print 'Printing contents of the L1 Menu:'

number_of_items = 0

for i in range(1,2):
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
    print 'Menu %15s, #Items %3s ' % (menu_name[i-1],str(number_of_items)) 
    number_of_items=0



print
print "**************** LVL1 ITEMS ******************* "
print "*********************************************** "

print '%35s == %5s ' % (menu_name[0], 'CTPID')
print



for i_ in range(256):
    i=str(i_)

    tag = []
    for xml_i in range(1,2):

        #        print info_xml[xml_i][i].keys()
        if i not in info_xml[xml_i].keys():
            info_xml[xml_i][i]={"***None***":"***None***"}
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
    print '%35s == %5s -- %25s' % (info_xml[1][i].keys()[0],i, print_tag)



output_file.Write()
output_file.Close()
print
print
print "**************** THRESHOLDS ******************* "
print "*********************************************** "

        

print '%22s == %10s ' % (menu_name[0], 'THR')
print '%22s == %10s ' % ('{id:{bitnum:mapping}}', 'THR')
print 

for thr in info_xml_thr[1]:
    print '%22s == %10s' % (info_xml_thr[1][thr],thr)
    

print
