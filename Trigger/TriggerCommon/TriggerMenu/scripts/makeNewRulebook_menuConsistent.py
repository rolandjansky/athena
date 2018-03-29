#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import commands
import xml.etree.cElementTree as ET


if len(sys.argv)<3:
    print "Need to give RuleBook and rel (e.g. rel_1) in the input"
    sys.exit()
    
file_name = sys.argv[1]
rel       = sys.argv[2]

L1_XML = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y.Z-VAL/AtlasCAFHLT/"+rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_pp_v2_*xml")
HLT_XML = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y.Z-VAL/AtlasCAFHLT/"+rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_pp_v2_*xml")


file = open(file_name,"r")

print
print "Reading XML files:"
print L1_XML
print HLT_XML
print



def readHLTXML(HLT_XML):

    HLT_list = []
    
    HLT_XML = ET.parse(HLT_XML)
    HLT_INFO = HLT_XML.getroot()
    for e in HLT_INFO.findall('CHAIN_LIST/CHAIN'):
        name    = e.get('chain_name')
        HLT_list.append(name)

    return HLT_list

def readL1XML(L1_XML):

    L1_list = []

    LVL1_XML = ET.parse(L1_XML)
    LVL1_INFO = LVL1_XML.getroot()

    for e in LVL1_INFO.findall('TriggerMenu/TriggerItem'):
        name = e.get('name')
        L1_list.append(name)

    return L1_list
    

list_l1  = readL1XML(L1_XML)
list_hlt = readHLTXML(HLT_XML)



new_RuleBook_content = '### This is an PS list for Physics_pp_v1 menu for P1 \n'
new_RuleBook_content += '### consistent with XML files:\n'
new_RuleBook_content += '### ' + str(L1_XML)  +'\n'
new_RuleBook_content += '### ' + str(HLT_XML) +'\n'
new_RuleBook_content += '\n'
new_RuleBook_content += '\n'


rulebook_items = []
print "*** Copying consistent items between RuleBook and XML:"
print "=====>"
for line in file:
    line_contents = line.split(";")
    item_to_check = line_contents[0].strip()
    rulebook_items.append(item_to_check)
    if item_to_check in list_l1 or item_to_check in list_hlt:
        new_RuleBook_content += line 
    else:
        print "Removing:",line
print "<====="

new_RuleBook_content += '\n### Adding L1 items / HLT chains that were missing from the rulebook\n\n'
print
print "*** Checking L1 Items"
print "*** Adding those missing from the rulebook:"
print "=====>"

for L1_item in list_l1:
    if L1_item not in rulebook_items:
        print "++",L1_item
        new_RuleBook_content += '%50s ' % L1_item+' \t; [1000] \t; [(0,1)] \n '

print "<====="
print
print "*** Checking HLT Chains"
print "*** Adding those missing from the rulebook:"
print "=====>"

for HLT_chain in list_hlt:
    if HLT_chain not in rulebook_items:
        print "++",HLT_chain
        new_RuleBook_content += '%50s' %HLT_chain+' \t; [1000] \t; [(0,1)] \t; [(0,-999)] \t; [(0,0)] \n'

print "<====="

print

print "Writing new rulebook"

new_file=open('newRulebook_'+rel+'.txt',"w")
new_file.writelines(new_RuleBook_content)

print 'Done'
print
