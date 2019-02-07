# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## This script checks the justification database for chains of a specific tag (e.g. "primary_at_1e32"). It makes a list of chains of this specific tag that are also in the menu of a specific release (e.g. "rel_0"). It picks from a reference RuleBook the lines corresponding to that list, and if the RuleBook does not contain any of these items, it writes this item with default value "(0,1)" for lumi 1000.
## Tomasz'json is being used.
##e.g. (from within the scripts directory) python makeRuleBook_for_justificationTag.py primary_at_1e32 rel_0 ../rules/Physics_pp_rulebook_PT.txt


import json
import urllib2
import sys, commands

import xml.etree.cElementTree as ET

tag  = sys.argv[1]
rel  = sys.argv[2]
rulebook = sys.argv[3]

#L1_XML = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/15.6.X.Y.Z/AtlasCAFHLT/"+rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_pp_v1_*xml")
HLT_XML = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/15.6.X.Y.Z/AtlasCAFHLT/"+rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_pp_v1_*xml")

print "HLT XML:"
print HLT_XML
print

def read_menu_xml(xml):
    
    HLT_list = {}
    
    HLT_XML = ET.parse(xml)
    HLT_INFO = HLT_XML.getroot()
    for e in HLT_INFO.findall('CHAIN_LIST/CHAIN'):
        name        = e.get('chain_name')
        lower_chain = e.get('lower_chain_name') 
        HLT_list[name]=lower_chain


    menu_dict = {}
    for i in HLT_list.keys():
        j=HLT_list[i]
        if i.startswith('EF_'):
            if (j):
                menu_dict[i.lstrip('EF_')]=[i,j,HLT_list[j]]
            else:
                menu_dict[i.lstrip('EF_')]=[i,'','']
        if i.startswith('L2_'):
            if i not in HLT_list.values():
                menu_dict[i.lstrip('L2_')]=['',i,j]
                
    return menu_dict



menu_dictionary=read_menu_xml(HLT_XML)


justurl = urllib2.urlopen('http://atlas-trigconf.cern.ch/just/get/')

justdata = json.load(justurl)

tagged_objects = []
for i in justdata.keys():
    if tag in justdata[i]['tags'].strip(','):
        if i[3:] in menu_dictionary.keys():
            chain = i[3:]
            tagged_objects.append(menu_dictionary[chain][0])
            tagged_objects.append(menu_dictionary[chain][1])
            tagged_objects.append(menu_dictionary[chain][2])
            

new_rulebook=''
tagged_objects_in_rulebook=[]
rb = open(rulebook,"r")
print 'Copying lines to new rulebook...'
for line in rb:
    line_content=line.split(';')
    
    if line_content[0].strip() in tagged_objects:
        print 'Copying line',line
        tagged_objects_in_rulebook.append(line_content[0].strip())
        new_rulebook+= '\t'+line_content[0].strip()+ ' \t ; ' + line_content[1].strip() + ' \t ; ' + line_content[2].strip() + ' \n '  
print
print
print 'Adding new objects with default values to rulebook...'
for i in tagged_objects:
    if i not in tagged_objects_in_rulebook:
        new_line = '\t ' + i +' \t; [1000] \t; [0,1] \n'
        print 'Adding line', new_line
        new_rulebook+=new_line

print
print
print 'Writing new rulebook...'
#print new_rulebook

new_file=open('newRulebook_'+tag+'_'+rel+'.txt',"w")
new_file.writelines(new_rulebook)

print 'Done'
print
