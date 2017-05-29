#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.etree.cElementTree as ET
import sys

menu_name = []

print
print 'Comparing the two following menus:'

output_file_name = sys.argv[1].strip(".xml") + "_" + sys.argv[2].strip(".xml") + "_RuleBook.txt"
output_xml_file = open(output_file_name,"w")


def read_ps_xml_and_dump_to_ruleBook():
    # read already exiting xml prescales
    LVL1_XML = ET.parse(sys.argv[1])
    LVL1_INFO = LVL1_XML.getroot()
    
    dict_name_ctpid       = {}
    dict_ctpid_prescale   = {}
    dict_L1item_prescale  = {}
    
    # first from LVL1 xml files
    for e in LVL1_INFO.findall('TriggerMenu/TriggerItem'):
        name  = e.get("name")
        ctpid = e.get("ctpid")
        dict_name_ctpid[name] = ctpid
    for e in LVL1_INFO.findall('PrescaleSet/Prescale'):
        ctpid    = e.get("ctpid")
        prescale = e.text.strip()  
        dict_ctpid_prescale[ctpid] = prescale 
    
    for name in dict_name_ctpid.keys():
        ctpid = dict_name_ctpid[name]        
        dict_L1item_prescale[name] = dict_ctpid_prescale[ctpid]
        print dict_ctpid_prescale[ctpid], name, ctpid

    HLT_XML  = ET.parse(sys.argv[2])
    HLT_INFO = HLT_XML.getroot()

    dict_name_ctpid        = {}
    dict_HLTchain_prescale = {}
    
    # second from HLT xml file
    for e in HLT_INFO.findall('CHAIN_LIST/CHAIN'):
        name             = e.get("chain_name")
        prescale         = e.get("prescale")
        pass_through     = e.get("pass_through")
        express_prescale = 0
        for ee in e.findall('STREAMTAG_LIST/STREAMTAG'):
            stream = ee.get("stream")
            if "express" in stream:
                express_prescale = ee.get("prescale")
        dict_HLTchain_prescale[name] = [prescale, express_prescale, pass_through]
        print dict_HLTchain_prescale[name], name
    # return the dictionaries with prescales
    return dict_L1item_prescale, dict_HLTchain_prescale

def read_write_xml(output_xml_file):
    output_file = "### This is an XML list\n"

    # read the prescales from the already existing dictionaries
    dict_L1item_prescale   = read_ps_xml_and_dump_to_ruleBook()[0]
    dict_HLTchain_prescale = read_ps_xml_and_dump_to_ruleBook()[1]

    input_lumi = sys.argv[3]
    
    #read again the xml files. In principle the xml files can be different if we have a new menu. 
    #In that case, we should change these input files in this function to be different than the ones in the previous function. 
    LVL1_XML = ET.parse(sys.argv[1])
    LVL1_INFO = LVL1_XML.getroot()
    for e in LVL1_INFO.findall('TriggerMenu/TriggerItem'):
        name = e.get('name')
        prescale = dict_L1item_prescale[name]
        prescale_factor = '' 
        if float(prescale) == 1 or float(prescale) <= 0 or ('RD' in name):
            prescale_factor = '(0,'+prescale+')'
        else:
            prescale_factor = '('+prescale+',0)'

        print '%50s' % name + " \t; ["+input_lumi+"] \t; ["+prescale_factor+"]\n"  
        output_file += '%50s' % name + " \t; ["+input_lumi+"] \t; ["+prescale_factor+"]\n"
        
    HLT_XML = ET.parse(sys.argv[2])
    HLT_INFO = HLT_XML.getroot()
    for e in HLT_INFO.findall('CHAIN_LIST/CHAIN'):
        name = e.get('chain_name')
        #lower_chain = e.get('lower_chain_name')
        prescale         = dict_HLTchain_prescale[name][0]
        express_prescale = dict_HLTchain_prescale[name][1]
        pass_through     = dict_HLTchain_prescale[name][2]
 
        prescale_factor = '' 
        if      ('RM' in name)           or \
                ('RD' in name)           or \
                ('SCTNoise' in name)     or \
                ('PixelNoise' in name)   or \
                ('cost_monit' in name)   or \
                ('beamspot' in name)     or \
                ('mbSpTrkMh_MB2'in name) or \
                ('mbMbts_2' in name):
            prescale_factor = '(0,'+prescale+')'
            express_prescale_factor = '(0,'+str(express_prescale)+')'
        else:
            if float(prescale) == 1 or float(prescale) <= 0:
                prescale_factor = '(0,'+prescale+')'
            else:
                prescale_factor = '('+prescale+',0)'
            if float(express_prescale) == 1 or float(express_prescale) <= 0:
                express_prescale_factor = '(0,'+str(express_prescale)+')'
            else:
                express_prescale_factor = '('+str(express_prescale)+',0)'                

#        print '%50s' % name + " \t; ["+input_lumi+"] \t; ["+prescale_factor+"]\n"  
        _goes_to_express = False
        for ee in e.findall('STREAMTAG_LIST/STREAMTAG'):
            stream = ee.get("stream")            
            if "express" in stream:
                _goes_to_express = True
            
        if _goes_to_express== True:
            output_file += '%50s' % name + " \t; ["+input_lumi+"] \t; ["+prescale_factor+"]  \t ; ["+express_prescale_factor+"] \t ; ["+pass_through+"]\n"    
        else:
            output_file += '%50s' % name + " \t; ["+input_lumi+"] \t; ["+prescale_factor+"]  \t ; [(0,-999)] \t ; ["+pass_through+"]\n"  
                
    output_xml_file.writelines(output_file)
                

#read_ps_xml_and_dump_to_ruleBook()
read_write_xml(output_xml_file)
