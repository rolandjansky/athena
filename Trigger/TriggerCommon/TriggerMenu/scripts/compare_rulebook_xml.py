#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import commands
import xml.etree.cElementTree as ET


if len(sys.argv)<3:
    print "Need to give RuleBook and rel (e.g. rel_1) in the input"
    sys.exit()
    
file = sys.argv[1]
rel  = sys.argv[2]

L1_XML = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/15.6.X.Y.Z/AtlasCAFHLT/"+rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_pp_v1_*xml")
HLT_XML = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/15.6.X.Y.Z/AtlasCAFHLT/"+rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_pp_v1_*xml")


print
print "Reading XML files:"
print L1_XML
print HLT_XML
print

#-----------------------------------------------------------------------------
# Helper class: stores rules for one trigger
#
class TriggerRule:
    def __init__(self):
    
        self.trigger_name        = ""
        self.has_express         = False
        self.has_pt              = True
        self.lumi_points         = {}
        
    def ReadLine(self, line):
        
        input_list = line.strip(' \n').split(';')

        if len(input_list) < 3:
            print "Skip incorrectly formatted input line:", line
            return False

        self.trigger_name = input_list[0].strip()

        return True


    def GetTriggerName(self):
        return self.trigger_name

#-----------------------------------------------------------------------------
# Read rulebook and fill trigger rules
#
def readRuleBook(file_path):

    file = open(file_path, 'r')

    list_of_triggers=[]

    for line in file: 

        if line.count('#') or not line.count(';'):
            continue

        rule = TriggerRule()
        rule.ReadLine(line)

        list_of_triggers.append(rule.GetTriggerName())    


    return list_of_triggers


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
    

list_in_rulebook = readRuleBook(file)
list_l1  = readL1XML(L1_XML)
list_hlt = readHLTXML(HLT_XML)


print
print "*** Checking L1 Items"
print "*** Missing from the rulebook:"
print "=====>"

for L1_item in list_l1:
    if L1_item not in list_in_rulebook:
        print "++",L1_item

print "<====="
print
print "*** Checking HLT Chains"
print "*** Missing from the rulebook:"
print "=====>"

for HLT_chain in list_hlt:
    if HLT_chain not in list_in_rulebook:
        print "++",HLT_chain

print "<====="

print



print "*** Exist in Rulebook but not in XML:"
print "=====>"
for item_in_rulebook in list_in_rulebook:
    if item_in_rulebook not in list_l1 and item_in_rulebook not in list_hlt:
        print "--",item_in_rulebook
print "<====="

print
