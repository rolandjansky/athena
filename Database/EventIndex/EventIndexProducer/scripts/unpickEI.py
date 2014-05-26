#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import PyUtils.dbsqlite as dbsqlite
from EventIndexProducer.compressB64 import decompressB64
import argparse


def options(argv):

    parser = argparse.ArgumentParser(description='Unpick Event Index File')
    parser.add_argument('-e','--evtnumber', default=0, type=int, help='event number to dump')
    parser.add_argument('-r','--runnumber', default=0, type=int, help='run number to dump')
    parser.add_argument('-m','--evtmax', default=1, type=int, help='Max number of events to dump')
    parser.add_argument('--decodetrig0',action='count', help='decode trigger. only bits fired')
    parser.add_argument('--decodetrig1',action='count', help='decode trigger. Full bit stream')
    parser.add_argument('eifile', help="EventIndex file")

    return parser.parse_args()

def main():
    
    # analyze options
    opt = options(sys.argv)

    fname = opt.eifile
    try:
        db = dbsqlite.open(fname,flags='r')
    except:
        print >> sys.stderr,"Unable to open peeker file %s" % fname
        sys.exit(1)

    try:
        nentries=db['Nentries'] 
    except:
        print >> sys.stderr,"Unable to get nentries from EI file %s" % fname
        sys.exit(1)


    #print "Stored keys: "
    #print "  ",db.keys()

    print ""
    print " version: ", db['Version']
    print " schema: ", db['Schema']
    print " #input files: ", db['Nfiles']
    print "  number of events: ", nentries
    print "  StartProcTime: ", db['StartProcTime']
    print "  EndProcTime: ", db['EndProcTime']
    print "  PandaID: ", db['PandaID']
    print "  PanDA_TaskID: ", db['PanDA_TaskID']
    print "  Includes Provenance: ", db['ProvenanceRef']
    print "  Includes Trigger: ", db['TriggerInfo']

    for i in xrange(db['Nfiles']):
        print ""
        print "File %d"%i
        if 'Nentries_%d'%i in db:
            print "  Events in this file: %s" % db['Nentries_%d'%i]
        print "  StartProcTime: ", db['StartProcTime_%d'%i]
        if 'EndProcTime_%d'%i in db:
            print "  EndProcTime: ", db['EndProcTime_%d'%i]
        else:
            print "  EndProcTime: ", db['EndProcTime']
        if "AMITag_%d"%i in db:
            print "  AMITag: ", db['AMITag_%d'%i]
        if "TrigStream_%d"%i in db:
            print "  TrigStream: ", db['TrigStream_%d'%i]
        if "ProjName_%d"%i in db:
            print "  ProjName: ", db['ProjName_%d'%i]

    print ""
    print "Summary: (info for %d events max)" % opt.evtmax
    print ""

    schema = db['Schema']
    evtnumber_idx = schema.index('EventNumber')
    runnumber_idx = schema.index('RunNumber')
    
    nevt_shown=0
    if opt.evtnumber == 0:
        sw0=True
    else:
        sw0=False
    for i in xrange(nentries):
        evt = db['Entry_%d'%i]
        data = zip(schema,evt)
            
        if opt.evtnumber == evt[evtnumber_idx]:
            if opt.runnumber == 0 or opt.runnumber == evt[runnumber_idx]:
                sw0 = True

        if not sw0:
            continue

        if nevt_shown >= opt.evtmax:
            break

        nevt_shown += 1

        trigL1=None
        trigL2=None
        trigEF=None

        print "%d ============================================================"%nevt_shown
        for k,v in data:
            print "  %-20s: %s" % (k,v)
            if k in "L1PassedTrigMask','L2PassedTrigMask','EFPassedTrigMask":
                ## trigger
                if v is not None and v != '':
                    v = decompressB64(v)
                    if opt.decodetrig1 > 0:
                        print "  %-20s: %s" % (k,v)
                    if k == "L1PassedTrigMask":
                        trigL1=v
                    if k == "L2PassedTrigMask":
                        trigL2=v
                    if k == "EFPassedTrigMask":
                        trigEF=v

        #trigger bits fired

        if trigL1 is not None:
            for i in xrange(8):
                tw = trigL1[32*i:32*(i+1)][::-1]
                v = int(tw,2)
                if v != 0 and opt.decodetrig0 > 0:
                    print "  L1PassedTrigMaskTBP%d = %d (0x%08x)"%(i, v, v)
            for i in xrange(8,16):
                tw = trigL1[32*i:32*(i+1)][::-1]
                v = int(tw,2)
                if v != 0 and opt.decodetrig0 > 0:
                    print "  L1PassedTrigMaskTAP%d = %d (0x%08x)"%(i-8, v, v)
            for i in xrange(16,24):
                tw = trigL1[32*i:32*(i+1)][::-1]
                v = int(tw,2)
                if v != 0 and opt.decodetrig0 > 0:
                    print "  L1PassedTrigMaskTAV%d = %d (0x%08x)"%(i-16, v, v)

        if trigL2 is not None:
            for i in xrange(len(trigL2)/32):
                tw = trigL2[32*i:32*(i+1)][::-1]
                v = int(tw,2)
                if v != 0 and opt.decodetrig0 > 0:
                    print "  L2PassedTrigMask%d = %d (0x%08x)"%(i, v, v)

        if trigEF is not None:
            for i in xrange(len(trigEF)/32):
                tw = trigEF[32*i:32*(i+1)][::-1]
                v = int(tw,2)
                if v != 0 and opt.decodetrig0 > 0:
                    print "  EFPassedTrigMask%d = %d (0x%08x) "%(i, v, v)


    db.close()

if __name__ == '__main__':
    main()

