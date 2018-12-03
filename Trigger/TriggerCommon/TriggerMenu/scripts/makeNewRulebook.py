#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# usage python makeNewRulebook.py ../rules/InitialBeam_HI_rulebook.txt rel_1 Physics_HI_v

import sys
import commands
import xml.etree.cElementTree as ET


if len(sys.argv)<4:
    print "Need to give RuleBook and rel (e.g. rel_1) in the input and a menu name"
    sys.exit()
    
file_name = sys.argv[1]
rel       = sys.argv[2]
menu_name       = sys.argv[3]


L1_XML = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/17.1.X.Y.Z-VAL/AtlasCAFHLT/"+rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_"+menu_name+"_*xml") 
HLT_XML = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/17.1.X.Y.Z-VAL/AtlasCAFHLT/"+rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_"+menu_name+"_*xml")



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



new_RuleBook_content = '### This is a PS list for a menu for P1 \n'
new_RuleBook_content += '### consistent with XML files:\n'
new_RuleBook_content += '### ' + str(L1_XML)  +'\n'
new_RuleBook_content += '### ' + str(HLT_XML) +'\n'
new_RuleBook_content += '\n'
new_RuleBook_content += '\n'


rulebook_items = []
#print "*** Copying consistent items between RuleBook and XML:"
#print "=====>"
#for line in file:
#    line_contents = line.split(";")
#    item_to_check = line_contents[0].strip()
#    rulebook_items.append(item_to_check)
#    if item_to_check in list_l1 or item_to_check in list_hlt:
#        new_RuleBook_content += line 
#    else:
#        print "Removing:",line
#print "<====="

new_RuleBook_content += 'from RuleClasses import TriggerRule \n\n\n'
new_RuleBook_content += 'tag = \'%s\' \n\n\n' %menu_name
new_RuleBook_content += 'rules = { \n\n\n'
print
print "*** Checking L1 Items"
print "*** Adding those missing from the rulebook:"
print "=====>"

for L1_item in list_l1:
    if L1_item not in rulebook_items:
        print "++",L1_item
        new_RuleBook_content += '\t \'%s\' : \t { 100 : TriggerRule( PS=1, comment=\'primary trigger\',)}, \n ' % L1_item

print "<====="
print
print "*** Checking HLT Chains"
print "*** Adding those missing from the rulebook:"
print "=====>"

for HLT_chain in list_hlt:
    if HLT_chain not in rulebook_items:
        print "++",HLT_chain
        new_RuleBook_content += '\t \'%s\' : \t { 100 : TriggerRule( PS=1, comment=\'primary trigger\',)}, \n ' % HLT_chain
        
print "<====="

print

new_RuleBook_content += '} \n\n\n'
print "Writing new rulebook"

new_file=open(menu_name+'_'+rel+'_rules.py',"w")
new_file.writelines(new_RuleBook_content)

print 'Done'
print
