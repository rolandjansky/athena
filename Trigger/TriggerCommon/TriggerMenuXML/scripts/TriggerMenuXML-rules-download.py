#!/usr/bin/env python2.6

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os
import sys
import commands
import shlex

#this is needed to get xml files from the release
nightlyPaths=os.environ['XMLPATH'].split(':')
for p in nightlyPaths:
    if os.path.exists(p+"/TriggerMenuXML/") is True:
        TriggerMenuXMLPath=p+"/TriggerMenuXML/"
        print "TriggerMenuXML directory is found in "+TriggerMenuXMLPath
        success = True
        break
    else:
        success = False

release  = sys.argv[2]
if release is '0':
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    release=TriggerFlags.menuVersion()

#useDB = sys.argv[3]
if sys.argv[3] is "True":
    useDB=True
else:
    useDB=False
    
print 'DB status: ', useDB

menu_tag = 'Physics_pp_v2'
rate_file='TriggerCosts_1e33.xml'

lvl1_file=TriggerMenuXMLPath+'LVL1config_'+menu_tag+'_'+release+'.xml'
hlt_file=TriggerMenuXMLPath+'HLTconfig_'+menu_tag+'_'+release+'.xml'

print 'LVL1 file:', lvl1_file
print 'HLT file:', hlt_file

if useDB:
    command="RuleClasses --run_type=%s --download='Physics_pp_v2_RULES.py' --active" %(menu_tag)
    RuleClasses.main(shlex.split(command), [])
    rule_file='Physics_pp_v2_RULES'
else:
    rule_file='TrigMenuRulebook.Physics_pp_v2_rules'

print 'Rule book: ', rule_file

lumi_list  = []
lumi_list  = sys.argv[1].split(",")

for lumi_point in lumi_list:
    print "*** Running:", lumi_point
    
    os.system("pwd")
    # example of usage a new rate rule book
    command2="processRules.py --rulebook=%s --target_lumi=%s --rates-xml=%s --lvl1-xml=%s --hlt-xml=%s --use_lowest_rule -v1" %(rule_file, lumi_point, rate_file, lvl1_file, hlt_file)
    thisargs = sys.argv
    sys.argv = shlex.split(command2)
    import TrigMenuRulebook.processRules
    status = TrigMenuRulebook.processRules.main()
    sys.argv = thisargs
    print "Converting xml files"
    command3="python cnvXML.py \
        --lv1xml=%s\
        --hltxml=%s \
        --ps_name=%s'_'%s'e30_'%s \
        --ps_xml='prescales'%s'.xml'" %(lvl1_file, hlt_file, menu_tag, lumi_point, release, lumi_point)
    os.system(command3)

#copy output files to ../data
os.system("mv RuleBook*PS*xml ../data/")
