#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# set_ibldist.py - tool to generate COOL sqlite DB fragment for IBL distortion
# Richard Hawkings, 21/5/15
# execute without arguments for help

## Matthias:
# Use the script:
# ./set_ibldist.py  CONDBR2 IndetIBLDist dummy.txt 0 100
# The last 2 numbers are the LB range, this needs to be enabled below; 
# Otherwise the IOV will be set form firt to last run for the full LB range

import os,sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib

class setIBLDist(AtlCoolLib.coolTool):
    def setup(self,args):
        "Setup routine called from initialisation"
        # set value of non-optional parameters
        self.tag=str(args[0])
        self.txtfile=str(args[1])
        #self.runmin=281411            # Set here the RunNr. min
        #self.runmax=281411            # Set here the RunNr. max
        #self.lumimin=int(args[2])     # Set here the LB min
        #self.lumimax=int(args[3])     # Set here the LB max

    def usage(self):
        "Define the additional syntax for options"
        self._usage1()
        print "COOL_tag txtfile"
        self._usage2()


    def execute(self):
        "Execute the database creation/update"
        # setup specification 
        spec=cool.RecordSpecification()
        spec.extend("stave",cool.StorageType.Int32)
        spec.extend("eta",cool.StorageType.Int32)
        spec.extend("mag",cool.StorageType.Float)
        spec.extend("base",cool.StorageType.Float)
        spec.extend("free",cool.StorageType.Float)

        # create folder if it does not exist
        folder='/Indet/IBLDist'
        cfolder=AtlCoolLib.ensureFolder(self.db,folder,spec,AtlCoolLib.athenaDesc(self.runLumi,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)
        if (cfolder is None): sys.exit(1)

        # read data from the text file
        print "Reading input data from %s" % self.txtfile
        bowdata=[]
        staves=[]
        for iline in open(self.txtfile).readlines():
            # split line into space-separated tokens, ignoring newline
            tokens=iline[:-1].split()
            # skip lines starting with #
            if tokens[0]=='#': continue
            # expect format '<stave> <eta> <mag> <base> <free>'
            if len(tokens)!=5:
                print "Skipping malformed line with %i tokens: " % len(tokens), iline 
            else:
                stave=int(tokens[0])
                eta=int(tokens[1])
                mag=float(tokens[2])
                base=float(tokens[3])
                free=float(tokens[4])
                if stave not in staves:
                    bowdata+=[(stave,eta,mag,base,free)]
                    staves+=[stave]
                else:
                    print "ERROR: read multiple lines for stave %i" % stave
        print "Ready to add data for %i staves" % len(bowdata)
        
        # now print data and write to COOL folder
        print "Channel Stave  eta    mag   base   free"
        for idata in bowdata:
            (stave,eta,mag,base,free)=idata
            # determine channel number from stave and eta
            # have 14 staves 0-13, and eta -10 to +9
            channel=100*(1+stave)+eta
            print "%7i" % channel, "%3i %6i %6.3f %6.3f %6.3f" % idata
            payload=cool.Record(spec)
            payload['stave']=stave
            payload['eta']=eta
            payload['mag']=mag #*.001
            payload['base']=base  #*.001
            payload['free']=free
            try:
                cfolder.storeObject(self.since,self.until,payload,channel,self.tag)
            except Exception,e:
                print e
                print "Store to COOL failed for channel %i" % channel
                sys.exit(-1)


smytool=setIBLDist('set_ibldist.py',False,5,5,[])
