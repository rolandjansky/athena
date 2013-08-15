#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.etree.cElementTree as ET
import sys, commands
import string
from string import *

import processRules
from processRules import *

def readHLTXML(HLT_XML, filter):

    HLT_XML = ET.parse(HLT_XML)
    HLT_INFO = HLT_XML.getroot()
    for e in HLT_INFO.findall('CHAIN_LIST/CHAIN'):
        name          = e.get('chain_name')        
        lower_name    = e.get('lower_chain_name')        
        for g in e.findall('GROUP_LIST/GROUP'):
            group_name = g.get('name')
            if filter in group_name:
                group = lower(group_name[len(filter):])

                if not group in HLT_list.keys():
                    HLT_list[group] = [name]
                    HLT_list[group].append(lower_name)
                else:
                    HLT_list[group].append(name)
                    HLT_list[group].append(lower_name)

    return HLT_list


if __name__ == "__main__":
    from optparse import OptionParser

    p = OptionParser(usage="usage: python getYourSlice.py $ruleBook $Group", version="1.0")
    p.add_option("--XML", type = "str", default="rel", dest = "xml", help = "Get XML from release or specific file (release is default)" )
    p.add_option("--BW"    , action="store_true" , dest="BW"    ,    help="Filter on BW groups [default]")
    p.add_option("--RATE" , action="store_true" , dest="RATE" ,      help="Filter on RATE groups")
    p.add_option("--menu" , type = "str", default="Physics_pp_v2" ,  dest = "trigMenu", help="Specify trigger menu, e.g. Physics_pp_v2")


    (opts, args) = p.parse_args()

    trigMenu         = opts.trigMenu
    xml              = opts.xml


    if len(args) < 2:
        p.print_usage()
        sys.exit()


    rules   = args[0]
    mySlice = lower(args[1])

    if 'xml' in xml:
        HLT_XML = xml
    elif 'rel' in xml:
       print '** Will try to find xmls... '
       nightlyPaths=os.environ['XMLPATH'].split(':')

       for p in nightlyPaths:
         hltpath=p+"/"+processRules.HLTname(trigMenu)
         if os.path.exists(hltpath) is True:
              HLT_XML = hltpath
              print "** Read XML from release",hltpath
              break
         else:
              print "==> Can not find HLT xml " + hltpath
              exit       
        


    if opts.BW and opts.RATE:
        sys.exit("Can only handle one group at a time, BW or RATE")
    elif opts.BW:
        filter = "BW:"
    elif opts.RATE:
        filter = "RATE:"
    else:
        filter = "BW:"
    
    print
    print "Reading XML files:"
    print HLT_XML
    print
    HLT_list = {}
    
    groups =  readHLTXML(HLT_XML, filter)

    print

    if mySlice not in HLT_list.keys():
        sys.exit("Wrong BW/slice name, please pick from %s \n" % HLT_list.keys())

    output_string = 'from RuleClasses import TriggerRule\n'

    newDict = {}
    ruleFile=__import__(rules).rules
    tag     =__import__(rules).tag

    output_string += "tag = " + tag + "\n"

    for chain in HLT_list[mySlice]:
        try:
            newDict[chain] = ruleFile[chain]
        except:
            pass 

    output_string += "rules = "

    for item in str(newDict).split("},"):
        output_string += item.strip("}")+"},\n"

    output_string += "}\n"
    
    output_name = "Rules_for_"+mySlice+".py"
    print "==> Writing rulebook for",mySlice,"group:",output_name+"\n"
    
    output_file = open(output_name,"w")
    
    output_file.writelines(output_string)
    
    output_file.close()
    
