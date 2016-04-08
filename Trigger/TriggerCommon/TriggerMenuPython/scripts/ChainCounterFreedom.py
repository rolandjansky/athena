#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
#  Makes are file with summary of counter usage
# Author E. Lipeles
#
import xml.etree.cElementTree as ET
import string
import os

menus = [ 'Physics_pp_v2', 'Physics_pp_v3', 'Physics_pp_v4',
          'MC_pp_v2', 'MC_pp_v3', 'MC_pp_v4',
          'Physics_HI_v1','Physics_HI_v2',
          'MC_HI_v1_pPb_mc_prescale',
          'MC_HI_v2_pPb_mc_prescale',
          'MC_HI_v1',
          'MC_HI_v2'
          ]


menu_xml = {}

version=os.getenv("AtlasVersion")
version='17.1.5.20.3'
xmlpath = os.path.join(os.getenv("AtlasArea"),"InstallArea/XML/TriggerMenuXML")

counters = {}

for m in menus:
    print "Reading Menu",'HLTconfig_'+m+'_'+version+'.xml'
    fullpath = os.path.join(xmlpath,
                            'HLTconfig_'+m+'_'+version+'.xml')
    menu_xml[m] = ET.parse(fullpath).getroot()

    for hlt_item in menu_xml[m].findall('CHAIN_LIST/CHAIN'):
        name = hlt_item.get("chain_name")
        ps   = hlt_item.get("prescale")
        cnt  = string.atoi(hlt_item.get("chain_counter"))
        lvl  = hlt_item.get("level")
        
        counters[cnt] = counters.get(cnt,[])+[(m,name)]

simplefile = open("counteruse.txt","w")

for cnt in range(0,1023):
    simplefile.write("Counter = %d  , " % cnt )
    if not counters.has_key(cnt):        
        simplefile.write("Unused\n")
    else:
        infolist = counters[cnt]
        mconly=True

        for info in infolist:
            if string.count(info[0],'Physics'):
                mconly=False

        if mconly:
            simplefile.write('MC-only\n')        
        else:
            simplefile.write('Physics\n')

        

        for (m,trg) in infolist:
            simplefile.write("      Menu=%s   Trigger=%s  \n" % (m,trg))

    
print "Output is in: ","counteruse.txt"
