#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import sys,os
from PyUtils import RootUtils

#return values
#Positive number: Number of events
#-1.. access problem
#-2.. corruption found

def checkPFCorruption(filename,verbose=False):
    if not os.access(filename,os.R_OK):
        print ("ERROR can't access file",filename)
        return -1

    ROOT = RootUtils.import_root()  # noqa: F841
    from ROOT import TFile,TTree
    
    try:
        f=TFile.Open(filename)
    except Exception:
        print ("Can't open file",filename)
        return -1

    n=None

    keys=f.GetListOfKeys()
    for k in keys:
        try:
            tn=k.GetName()
            t=f.Get(tn)
            if not isinstance(t,TTree): return
        except Exception:
            print ("Can't get tree %s from file %s",tn,fn)
            f.Close()
            return -1

        if (verbose): print ("Working on tree",tn)
        n=t.GetEntriesFast()
        for i in range(n):
            s=t.GetEntry(i)
            if s<=0:
                print ("Tree %s: Found corruption in event %i" % (i,n))
                f.Close()
                return -2
            else:
                if verbose and i>0 and i%100==0:
                    print ("Checking event",i)
        print ("Tree %s: %i event(s) ok" % (tn,n))

        pass #end of loop over trees

    f.Close()
    print ("ROOT file",filename,"looks ok")
    if n is None:
        print ("Failed to determine number of events in file %s. No tree named 'CollectionTree'" % filename)
        return 0
    return n


if __name__ == "__main__":
    if len(sys.argv)!=2:
        print ("Usage: ",sys.argv[0]," <file>")
    else:
        fn=sys.argv[1]
        checkPFCorruption(fn)#,True)
