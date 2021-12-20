#!/usr/bin/env python 
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys,os
import pickle
import getopt
from AthenaCommon.Debugging import DbgStage

def usage():
    print ("""CARunner, to execute a pickled ComponentAccumulator
Usage:
CARunner.py [options] <picklefile> [nEvents]

Accepted command line options:
 -d, --debug <stage>                  ...  attach debugger (gdb) before run,
                                            w/ <stage>: conf, init, exec, fini
                                           -d is a shorthand for --debug init and takes no argument
""")
    
    sys.exit(-1)



if __name__=="__main__":

    opts = 'd'
    longopts = ['debug=']

    try:
        optlist, args = getopt.getopt (sys.argv[1:], opts, longopts)
    except getopt.error:
        print (sys.exc_info()[1])
        usage()

    if not (len(args)==1 or len(args)==2):
        usage()

        

    inputName=args[0]
    if not os.access(inputName,os.R_OK):
        print("ERROR, can't read file",inputName)
        usage()
    
    nEvt=10
    if len(args)==2:
        try:
            nEvt=int(args[1])
        except ValueError:
            print("Failed to interpret nEvent, got",args[1])
            usage()

    inFile=open(inputName, 'rb')
    
    acc=pickle.load(inFile)

    if acc._isMergable: #Not a top-level accumulator
        from AthenaConfiguration.MainServicesConfig import MainServicesCfg
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        acc1=MainServicesCfg(ConfigFlags)
        acc1.merge(acc)
    else:
        acc1 = acc

    for opt, arg in optlist:
        if opt in ('-d', '--debug'):
            if not arg:
                arg = 'init'
            elif arg not in DbgStage.allowed_values:
                usage()
            acc1.setDebugStage (arg)

    acc1.run(nEvt)
