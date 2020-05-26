#!/usr/bin/env python 
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import sys,os
import pickle 

def usage():
    print ("CARunner, to execute a pickled ComponentAccumulator")
    print ("Usage:")
    print ("CARunner.py <picklefile> [nEvents]")
    sys.exit(-1)



if __name__=="__main__":

    if not (len(sys.argv)==2 or len(sys.argv)==3):
        usage()

    inputName=sys.argv[1]
    if not os.access(inputName,os.R_OK):
        print("ERROR, can't read file",inputName)
        usage()
    
    nEvt=10
    if len(sys.argv)==3:
        try:
            nEvt=int(sys.argv[2])
        except ValueError:
            print("Failed to interpret nEvent, got",sys.arv[2])
            usage()

    inFile=open(inputName, 'rb')
    
    acc=pickle.load(inFile)

    if acc._isMergable: #Not a top-level accumulator
        from AthenaConfiguration.MainServicesConfig import MainServicesCfg
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        acc1=MainServicesCfg(ConfigFlags)
        acc1.merge(acc)
        acc1.run(nEvt)
    else:
        acc.run(nEvt)
