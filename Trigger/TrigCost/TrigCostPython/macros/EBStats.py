#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TrigCostPython import TrigCostAnalysis, TrigCostXML , TrigCostTRP
from optparse import OptionParser
import os, sys, string, math, shutil, re
import ROOT
ROOT.gROOT.SetBatch(True)

#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

usage='<rates xml>'
full_usage='EBStats.py '+usage

p = OptionParser(usage)
    

(opts, args) = p.parse_args()

if len(args) != 1:
    print "Must have exactly one argument"
    print full_usage
    sys.exit(-1)


#-----------------------------------------------------------------------
#  Class for selecting trigger tag
#-----------------------------------------------------------------------
class TrgTag:
    def __init__(self,tag):
        m = re.match("([0-9]*)([A-Z]*)([0-9]*)([A-Z]*)",tag)
        if not m:
            print tag
        if m.group(1)!='':
            self.mult = string.atoi(m.group(1))
        else:
            self.mult = 1
        self.type = m.group(2)
        if m.group(3)!='':
            self.thrs = string.atoi(m.group(3))
        else:
            self.thrs = -1
        self.cuts = m.group(4)
        #self.cuts = "N/A"

    def __str__(self):
        return "Type:"+self.type+" Thrs:"+self.cuts+" Thrs:"+str(self.thrs)+" Mult:"+str(self.mult)



#-----------------------------------------------------------------------
#  EB chains
#-----------------------------------------------------------------------
#eb_low  =  ['L1_EM5','L1_MU0','L1_TAU6','L1_J10','L1_FJ30','L1_TE400','L1_XE10', 'L1_XS15', 'L1_JE60', 'L1_2EM3']
#eb_high =  ['L1_EM10',  'L1_MU6', 'L1_2MU0', 'L1_TAU20',   'L1_J30', 'L1_FJ30', 'L1_TE400', 'L1_XE20', 'L1_JE200',  'L1_2TAU6', 'L1_3J10', 'L1_TAU6_XE10', 'L1_2FJ10', 'L1_2EM5']

#eb_high =  ['L1_EM10',  'L1_MU0',  'L1_TAU20',   'L1_J15', 'L1_FJ30', 'L1_TE180', 'L1_XE20', 'L1_JE60',  'L1_2TAU6', 'L1_3J10', 'L1_TAU6_XE10', 'L1_2FJ10', 'L1_2EM5']


#-----------------------------------------------------------------------
#  Read in XML file
#-----------------------------------------------------------------------
results = TrigCostXML.ReadXmlFile(args[0])

#-----------------------------------------------------------------------
#  Assign based on L1 prescale
#-----------------------------------------------------------------------
ebmenu = {}
ebmenu['eb_physics_noL1PS'] =  []
ebmenu['eb_physics'] =  []
ebmenu['eb_high_physics'] =  []
ebmenu['eb_physics_random'] =  []
ebmenu['eb_physics_empty'] =  []
ebmenu['eb_physics_firstempty'] =  []
ebmenu['eb_physics_unpaired'] =  []
ebmenu['eb_physics_unpaired_noniso'] =  []


for ch in results.GetL1Chains():

    if string.count(ch.GetName(),'CALREQ'):
        continue

    if string.count(ch.GetName(),'BGRP0'):
        continue

    if string.count(ch.GetName(),'MBTS'):
        continue

    if string.count(ch.GetName(),'ZDC'):
        continue

    if string.count(ch.GetName(),'BCM'):        
        continue

    if string.count(ch.GetName(),'LUCID'):        
        continue

    if string.count(ch.GetName(),'ALFA'):        
        continue

    if string.count(ch.GetName(),'L1_BGRP'):
        continue

    if string.count(ch.GetName(),'L1_LB_Num'):
        continue

    if string.count(ch.GetName(),'L1_TRT'):
        continue

    if string.count(ch.GetName(),'L1_L1A'):
        continue

    if string.count(ch.GetName(),'RD'):
        continue

    if string.count(ch.GetName(),'L1_EMPTY'):
        continue

    if string.count(ch.GetName(),'L1_ZB'):
        continue

    ebitem = None

    ps = ch.GetPrescale()
    rate = ch.GetPrescale()*ch.GetTAVRate()

    bgrp='filled'
    # explicit rules
    if ch.GetName()=='L1_2TAU11I':
        ebitem = 'eb_high_physics'

    if ch.GetName()=='L1_2FJ30':
        ebitem = 'eb_physics_noL1PS'
        
    # generic rule
    elif string.count(ch.GetName(),'FIRSTEMPTY'):
        bgrp='firstempty'
        if rate < 2e6:
            ebitem = 'eb_physics_firstempty'
    elif string.count(ch.GetName(),'EMPTY'):
        bgrp='empty'
        if rate < 2e6:
            ebitem = 'eb_physics_empty'
    elif string.count(ch.GetName(),'UNPAIRED_ISO'):
        bgrp='unpaired_iso'
        if rate < 2e6:
            ebitem = 'eb_physics_unpaired'
    elif string.count(ch.GetName(),'UNPAIRED_NONISO'):
        bgrp='unpaired_noniso'
        if rate < 2e6:
            ebitem = 'eb_physics_unpaired_noniso'
    else:
        if ps == -1:            
            print "Unused trigger",ch.GetName(),rate
            continue

        if ps == 1:
            ebitem = 'eb_physics_noL1PS'
        elif ps < 200 and rate < 3e4:
            ebitem = 'eb_high_physics'
        elif ps < 2e4 and rate < 2e5:
            ebitem = 'eb_physics'
        else:
            ebitem = 'eb_physics_random'

    print "%10s %20s %20s %10.0f" % (bgrp,ebitem,ch.GetName(),rate)

    if not ebitem:
        print "Unassigned = ",ch.GetName()
        continue
            
    ebmenu[ebitem].append(ch.GetName())

# special fixes for ps-disabling
ebmenu['eb_high_physics'].append('L1_2MU4_MU6')
ebmenu['eb_high_physics'].append('L1_2MU4_BARREL')
ebmenu['eb_high_physics'].append('L1_2MU4_2EM3')

print
for (ebitem,menu) in ebmenu.items():
    if ebitem!='eb_physics' and ebitem!='eb_high_physics':
        print ebitem+"_seed ='",string.join(menu,","),"'"
    else:
        print ebitem+" = ",
        print [x.encode('ascii') for x in menu]

    print 
    

    
#print "noL1PS:"
#noL1PSmap = {}
#for ch in eb_noL1PS:
#    #print ch.GetName(),
#    tags = [TrgTag(tag) for tag in string.split(ch.GetName()[3:],"_")]
#    classification = [(tag.type,tag.cuts,tag.mult) for tag in tags]
#    noL1PSmap.setdefault(tuple(classification),[]).append((tags,ch.GetName()))
#
#for (classification,chains) in noL1PSmap.items():
#    print classification
#    for ch in chains:        
#        (tags,chname) = ch    
#        print "     ",chname
#
#

print 

# EBConfig from Monica
#eboldconfig = {}
#
#eboldconfig['eb_physics_empty'] = string.split('L1_EM6_EMPTY,L1_MU4_EMPTY,L1_MU11_EMPTY,L1_2MU4_EMPTY,L1_TAU8_EMPTY,L1_J10_EMPTY,L1_FJ30_EMPTY',',')
#
#eboldconfig['eb_physics_firstempty'] = string.split('L1_EM3_FIRSTEMPTY,L1_MU4_FIRSTEMPTY,L1_2MU4_FIRSTEMPTY,L1_MU6_FIRSTEMPTY,L1_TAU8_FIRSTEMPTY,L1_J10_FIRSTEMPTY,L1_J30_FIRSTEMPTY',',')
#
#eboldconfig['eb_physics_noL1PS'] = string.split('L1_EM18VH,L1_EM30,L1_2EM10VH,L1_2EM12_EM16V,L1_3EM6_EM14VH,L1_MU15,L1_2MU6,L1_2MU4_BARRELONLY,L1_2EM6_MU6,L1_EM10VH_MU6,L1_EM6_2MU6,L1_TAU40,L1_2TAU20,L1_2TAU11I_TAU15,L1_2TAU11I_EM14VH,L1_2TAU11_TAU20_EM14VH,L1_TAU8_MU10,L1_TAU15I_XE40,L1_TAU20_XE40,L1_J75,L1_4J15,L1_J50_XE40,L1_J30_XE50,L1_3J15_J50,L1_MU10_J20,L1_MU4_J20_XE35,L1_FJ75,L1_2FJ30,L1_XE60,L1_TE800,L1_2MU4_XE40,L1_XE40_BGRP7',',')
#
#eboldconfig['eb_physics_unpaired'] = string.split('L1_EM3_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_MU10_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_MU4_J15_UNPAIRED_ISO,L1_TAU8_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J30_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO',',')
#
#eboldconfig['eb_physics_unpaired_noniso'] = string.split('L1_EM3_UNPAIRED_NONISO,L1_MU4_UNPAIRED_NONISO,L1_2MU6_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_NONISO,L1_J10_UNPAIRED_NONISO,L1_J30_UNPAIRED_NONISO',',')
#
#eboldconfig['eb_physics'] = ['L1_EM6','L1_MU4','L1_TAU8','L1_J10','L1_FJ50','L1_TE500','L1_XE20','L1_XS25','L1_2EM3']
#
#eboldconfig['eb_high_physics'] = ['L1_EM10VH','L1_MU6','L1_2MU4','L1_TAU15','L1_J30','L1_TE700','L1_XE40','L1_XS30','L1_2TAU11','L1_3J10','L1_2FJ30','L1_3EM6']


# EBConfig Sept 2012
eboldconfig = {}
eboldconfig['eb_physics_empty'] = string.split('L1_MU4_J15_EMPTY,L1_MU11_EMPTY,L1_J30_EMPTY,L1_MU4_EMPTY,L1_FJ30_EMPTY,L1_TAU8_EMPTY,L1_J10_EMPTY,L1_MU10_EMPTY,L1_EM3_EMPTY,L1_EM6_EMPTY,L1_2MU4_EMPTY',',')
eboldconfig['eb_physics_firstempty'] = string.split('L1_MU20_FIRSTEMPTY,L1_EM3_FIRSTEMPTY,L1_J30_FIRSTEMPTY,L1_TAU8_FIRSTEMPTY,L1_FJ30_FIRSTEMPTY,L1_2MU4_FIRSTEMPTY,L1_MU6_FIRSTEMPTY,L1_MU4_FIRSTEMPTY,L1_MU10_FIRSTEMPTY,L1_J10_FIRSTEMPTY',',')
eboldconfig['eb_physics_noL1PS'] = string.split('L1_2MU6,L1_MU6_2J20,L1_TAU15_XE40,L1_2MU10,L1_3EM6_EM14VH,L1_TAU20_XE35,L1_2FJ30,L1_2MU4_BARRELONLY,L1_XE70,L1_2TAU11I_TAU15,L1_XE40_BGRP7,L1_MU15,L1_2TAU20,L1_2EM12_EM16V,L1_XE60,L1_2MU4_MU6_BARREL,L1_TAU15_XE35,L1_2TAU11I_EM14VH,L1_XE50,L1_J50_XE35,L1_XE35_BGRP7,L1_2EM6_MU6,L1_2EM10VH,L1_2MU4_MU6,L1_XE40,L1_J350,L1_TAU15I_XE35,L1_TE700,L1_EM6_2MU6,L1_FJ75,L1_TAU20_XE40,L1_TAU8_MU10,L1_MU4_J20_XE35,L1_EM10VH_MU6,L1_J30_XE40,L1_2MU4_XE40,L1_2MU4_BARREL,L1_TE800,L1_3MU4,L1_3MU6,L1_TAU11_MU10,L1_EM18VH,L1_2TAU11_TAU20_EM14VH,L1_3J15_J50,L1_EM6_XS55,L1_MU10_J20,L1_EM30,L1_J50_XE40,L1_J30_XE50,L1_TAU15I_XE40,L1_XE50_BGRP7,L1_J75,L1_TAU40,L1_MU20,L1_EM16VH_MU4,L1_4J15',',')
eboldconfig['eb_physics_unpaired'] = string.split('L1_MU4_J15_UNPAIRED_ISO,L1_MU10_UNPAIRED_ISO,L1_J30_UNPAIRED_ISO,L1_2MU6_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_TAU8_UNPAIRED_ISO,L1_EM3_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO',',')
eboldconfig['eb_physics_unpaired_noniso'] = string.split('L1_MU4_UNPAIRED_NONISO,L1_J30_UNPAIRED_NONISO,L1_EM3_UNPAIRED_NONISO,L1_2MU6_UNPAIRED_NONISO,L1_J10_UNPAIRED_NONISO,L1_TAU8_UNPAIRED_NONISO',',')
eboldconfig['eb_physics']  = ['L1_2EM3_EM12', 'L1_XS45', 'L1_J50', 'L1_TAU15I', 'L1_MU4_J15', 'L1_FJ30', 'L1_MU10', 'L1_J30', 'L1_EM10VH', 'L1_EM16VH', 'L1_XE30', 'L1_XE20', 'L1_3J15', 'L1_TAU20', 'L1_MU6', 'L1_EM12', 'L1_TE300']
eboldconfig['eb_high_physics']  = ['L1_2MU4', 'L1_MU4_J30', 'L1_XS60', 'L1_TE500', 'L1_FJ50', 'L1_EM16V_XE20', 'L1_MU4_J50', 'L1_2EM6_EM16VH', 'L1_XS50']

for menu in eboldconfig.keys():

    print "Comparing ",menu
    for item in eboldconfig[menu]:
        if item not in ebmenu[menu]:
            print "In old config, but not new",item

    for item in ebmenu[menu]:
        if item not in eboldconfig[menu]:
            print "In new config, but not old",item



#-----------------------------------------------------------------------
#  Figure out what "seed" trigger has
#-----------------------------------------------------------------------
eb_source_map = {}
eb_source_map['eb_random'] = []
eb_source_map['eb_low']    = []
eb_source_map['eb_high']   = []

for ch in results.GetEFChains():

    l2_chname = ch.GetLowerChainSingleSeed()
    if l2_chname=='':
        continue
    l2_ch = results.GetChain(l2_chname)
    
    l1_chname = l2_ch.GetLowerChainSingleSeed()
    if l1_chname=='':
        continue

    if string.count(l1_chname,'EMPTY') or \
       string.count(l1_chname,'UNPAIRED'):
        continue
       
    for (menu,items) in ebmenu.items():
        if l1_chname in items:
            eb_source_map.setdefault(menu,[]).append((ch,l1_chname))

    
    #sample='eb_random'    
    #for s in string.split(l1_chname,'_'):
    #    tag=TrgTag(s)
    #    if tag.type in eb_high_map.keys():
    #        if string.atoi(tag.thrs) >= string.atoi(eb_high_map[tag.type].thrs):
    #            sample='eb_high'
    #    if (not sample=='eb_high') and (tag.type in eb_low_map.keys()):
    #        if string.atoi(tag.thrs) >= string.atoi(eb_low_map[tag.type].thrs):
    #            sample='eb_low'
    #


    
    #print "%-40s %-30s %-10s " % (ch.GetName(),l1_chname,sample)
    #eb_source_map[sample].append((ch,l1_chname))

    #if l1_chname in eb_high:
    #    eb_high_seed.append((ch,l1_chname,l1_seed))


for sample in ['eb_physics_noL1PS','eb_physics','eb_high_physics',"eb_physics_random"]:
    print
    print "------------------------------------------------------------"
    print sample
    print "------------------------------------------------------------"
    print "          %-40s &            %-30s &          %7s  \\\\" % ('Name','L1','Prod PS')
    
    for (ch,l1name) in eb_source_map[sample]:

        # product PS for L2 and EF
        prodps=ch.GetPrescale()
        efps=ch.GetPrescale()
        try:            
            if ch.GetLevel()=="L2" or ch.GetLevel()=="EF":
                lower_chain = results.GetChain(ch.GetLowerChain())
                prodps *= lower_chain.GetPrescale()
                l2ps = lower_chain.GetPrescale()
                if ch.GetLevel()=="EF":
                    lower_lower_chain = results.GetChain(lower_chain.GetLowerChain())
                    prodps *= lower_lower_chain.GetPrescale()
                    l1ps = lower_lower_chain.GetPrescale()
        except Exception,e:
            print e
            prodps=0.0

        if prodps==0:
            continue
        format="          %-40s &            %-30s &          %8.1d \\\\"
        if prodps > 1e6:
            format="          %-40s &            %-30s &          %7.1e  \\\\"
        if (sample=='eb_random' or sample=='eb_low') and prodps==1:
            format="\darkred{%-40s} &  \darkred{%-30s} & \darkred{%8.1d} \\\\"
        
        print format % (string.replace(ch.GetName(),"_","\\_"),
                        string.replace(l1name,"_","\\_"),
                        prodps)




#-----------------------------------------------------------------------
#  Input rates
#-----------------------------------------------------------------------
#for (sample,trgs) in [('eb_low',eb_low),('eb_high',eb_high)]:
#    print
#    print "------------------------------------------------------------"
#    print sample
#    print "------------------------------------------------------------"    
#    for trg in trgs:
#        print "          %-40s &          %8.1d \\\\" % (trg,results.GetChain(trg).GetRate()*results.GetChain(trg).GetPrescale())



