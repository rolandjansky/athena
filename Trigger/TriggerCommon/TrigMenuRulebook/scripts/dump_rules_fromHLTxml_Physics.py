#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# This code reads a HLT xml file and create the rules with PS = -1 and rerun = 0
# The PS=1 primaries can be enabled from list_of_primaries
# You can specify special chains list_of_specials_cosmic
#
## run it that way:
## python dump_rules_fromHLTxml.py HLTMenu_125.xml Physics_pp_v4

import xml.etree.cElementTree as ET
import sys
from sys import argv
import string

if len(argv) < 3:
    print "Error, you need to provide an input hlt xml"
    print "--hltxmlname : fileName for the hlt xml"
    print "--tag        : tag"
    sys.exit()

inputhltfile = sys.argv[1] 
tag = sys.argv[2]

#
# All triggers will be set to PS=-1, rerun = 0 besides the special ones defined below
#

list_of_l1 = {
#    'L1_RD0_FILLED' : { 'PS' : 1000000 },
#    'L1_RD0_EMPTY'  : { 'PS' : 1000000 },
#    'L1_RD1_EMPTY'  : { 'PS' : 1000000 },
}

#
# primary triggers, PS = 1
#
list_of_primaries = [
#        'HLT_3mu6_msonly',
#        'HLT_e15_etcut',
]

rulebook_filename = "%s_rules.py" %(tag)
rulebookfile = open(rulebook_filename, "w")
print "Writing out %s " % rulebook_filename

rulebookfile.writelines("from RuleClasses import TriggerRule\n\n")
rulebookfile.writelines("tag = %s%s%s\n\n" % ("'", tag, "'" ))
rulebookfile.writelines("EBrun=False\n\n")
rulebookfile.writelines("rules = {\n")

print 'Writing rulesbook file %s from HLT xml file  %s with tag %s' % (rulebook_filename, inputhltfile, tag)
HLT_XML = ET.parse(inputhltfile)
HLT_INFO = HLT_XML.getroot()

list_of_chains = []
list_of_streams = []

number_of_items = 0
for e in HLT_INFO.findall('CHAIN_LIST/CHAIN'):
    number_of_items+=1
    name    = e.get('chain_name')
    list_of_chains.append(name)
    stream = list(e.iter("STREAMTAG"))
    for i in stream:
       stream_name = i.get('stream')
    list_of_streams.append(stream_name)

# list = zip(list_of_streams, list_of_chains)
list = zip(list_of_chains, list_of_streams)
list.sort()


#
# L1 rule first
#
for chain in list_of_l1:
    ps = list_of_l1_cosmic[chain]['PS']
    rerun = 0.
    chain_name = "'%s'" % chain
    rulebookfile.writelines("%60s : { 10000 : TriggerRule(PS=%.1f, comment = 'L1', rerun = %.1f) }, \n" % (chain_name, ps, rerun))

# for stream, chain in the hlt list: 
for chain, stream in list: 
    ps = -1.
    ps_es = 0.
    rerun = 0.
    if chain in list_of_primaries:
        ps = 1
    chain_name = "'%s'" % chain
    if ps_es > 0. :
        rulebookfile.writelines("%60s : { 100 : TriggerRule(PS=%.1f, comment = '%s', rerun = %.1f, ESValue = %.1f) }, \n" % (chain_name, ps, stream, rerun, ps_es))
    else :
        rulebookfile.writelines("%60s : { 100 : TriggerRule(PS=%.1f, comment = '%s', rerun = %.1f) }, \n" % (chain_name, ps, stream, rerun))

rulebookfile.writelines("        }\n")
