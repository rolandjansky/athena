#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
import os, sys, string, math, shutil
import xml.dom.minidom

#---------------------------------------------------------------------
# Object to store data
#
class Chain:
    def __init__(self):
        self.name        = ""
        self.stream      = ""
        self.stream_type = ""
        self.has_express = False
        self.lower_name  = ""
        self.lower_list  = []
        self.thr         = ""
        self.counter     = -1
        self.tes         = []
        
    def Print(self):
        print self.name,':',self.stream

class L1Th:
    def __init__(self):
        self.name = ""

    def Print(self):
        print self.name
    
#---------------------------------------------------------------------
def ReadXmlFile(filename):

    chains=[]

    try:
        myf = open(filename)
    except:
        print "Failed to open file:",filename
        return chains

    try:
        dom = xml.dom.minidom.parse(myf)
    except:
        print "Failed to parse file:",filename
        return chains        

    print 'Parsing XML file:',filename

    for xml_menu in dom.getElementsByTagName("HLT_MENU"):
        for xml_chains in xml_menu.getElementsByTagName("CHAIN_LIST"):
            for xml_chain in xml_chains.getElementsByTagName("CHAIN"):

                chain = Chain()
                chain.name       = xml_chain.getAttribute("chain_name")
                chain.lower_name = xml_chain.getAttribute("lower_chain_name")
                chain.lower_name = xml_chain.getAttribute("counter")
                chain.lower_list = chain.lower_name.split(',')

                for xml_streams in xml_chain.getElementsByTagName("STREAMTAG_LIST"):
                    for xml_stream in xml_streams.getElementsByTagName("STREAMTAG"):
                        stream = xml_stream.getAttribute("stream")
                        if stream == "express":
                            chain.has_express = True
                        else:
                            chain.stream      = xml_stream.getAttribute("stream")
                            chain.stream_type = xml_stream.getAttribute("type")

                for xml_sigs in xml_chain.getElementsByTagName("SIGNATURE_LIST"):
                    for xml_sig in xml_sigs.getElementsByTagName("SIGNATURE"):
                        for xml_te in xml_sig.getElementsByTagName("TRIGGERELEMENT"):    
                            te = xml_te.getAttribute("te_name")
                            chain.tes += [te]
                            
                            
                chains += [chain]

    for xml_menu in dom.getElementsByTagName("LVL1Config"):
        for xml_chains in xml_menu.getElementsByTagName("TriggerMenu"):
            for xml_chain in xml_chains.getElementsByTagName("TriggerItem"):

                chain = Chain()
                chain.name       = xml_chain.getAttribute("name")
                chain.counter    = xml_chain.getAttribute("ctpid")
                
                chains += [chain]
                
    return chains

#---------------------------------------------------------------------
def ReadLv1File(filename):

    thrs = []

    try:
        myf = open(filename)
    except:
        print "Failed to open file:",filename
        return thrs

    try:
        dom = xml.dom.minidom.parse(myf)
    except:
        print "Failed to parse file:",filename
        return thrs

    print 'Parsing XML file:',filename

    for xml_menu in dom.getElementsByTagName("LVL1Config"):
        for xml_chains in xml_menu.getElementsByTagName("TriggerThresholdList"):
            for xml_chain in xml_chains.getElementsByTagName("TriggerThreshold"):

                th = L1Th()
                th.name = xml_chain.getAttribute("name")
                
                thrs += [th]
                
    return thrs

#---------------------------------------------------------------------
def Print(objs, options):
    
    for c in objs:
        if options.level == 'L2' and not c.name.count('L2'): continue
        if options.level == 'EF' and not c.name.count('EF'): continue        
        
        if len(options.key) and not c.name.count(options.key):
            continue
        if len(options.ign) and     c.name.count(options.ign):
            continue
        if hasattr(c, 'stream'):
            if len(options.str) and not c.stream.count(options.str):
                continue
            if len(options.stp) and not c.stream_type.count(options.stp):
                continue
        
        c.Print()

        if options.te.count('TE'):
            for t in c.tes:
                print '   ',t

#---------------------------------------------------------------------        
# Read XML files using function above
#
if __name__ == "__main__":
    
    p = OptionParser(usage="usage: <XMK menu file>")
    
    p.add_option("-k",      type="string", default="", dest="key",    help="include chains by matching key")
    p.add_option("-i",      type="string", default="", dest="ign",    help="ignore  chains by matching key")
    p.add_option("-s",      type="string", default="", dest="str",    help="include stream name by matching key")
    p.add_option("-t",      type="string", default="", dest="stp",    help="include stream type by matching key")
    p.add_option("--te",    type="string", default="", dest="te",     help="print trigger elements")
    p.add_option("--level", type="string", default="", dest="level",  help="level")    

    (options, args) = p.parse_args()

    if len(args) != 1:
        p.error("need path to input file")    
    import operator

    chains = ReadXmlFile(args[0]);
    thresh = ReadLv1File(args[0]);    
    
    chains.sort(key=lambda x: x.name, reverse=False)
    thresh.sort(key=lambda x: x.name, reverse=False)    

    Print(chains, options)
    Print(thresh, options)
