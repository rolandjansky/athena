#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# L1CT crew, Johan.Lundberg@cern.ch, CERN, 2009

import sys,getopt,os

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                   this printout"
    print " "
    print " Make and draw CTPMON cool histograms "
    print " "
    print " Mandatory: "
    print "  --inputfile | -i <filename>      A .py.obj with parsed cool data"    
    print " "
    print " Switches: "
    print " "
    print "  --outbase | -o <outname>    filename base for output files"     
    print "  --nodraw <options (read busy_cool_reader.py for info.)> "

try:
    longopts=['inputfile=','i=','help','text','nodrawHist'
              'outbase=','nodraw=']
    opts,args=getopt.getopt(sys.argv[1:],'i:o:',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

fname=""
for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o in ('--inputfile','-i'):
        fname=a
if not fname:
    usage()
    sys.exit(2)

## read file

import busy_py
busy_py.plotfile(fname,opts)
