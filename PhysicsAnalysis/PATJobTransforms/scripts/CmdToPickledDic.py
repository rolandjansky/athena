#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print " * * * CmdToPickledDic * * *"
if __name__ == '__main__':
    import sys
    if len(sys.argv) < 2 or sys.argv[1].upper()=="-H":
        print "CmdToPickledDic: Tool to create inputDictionary.pickle to be used with Reco_trf.py"
        print "Parameters are the same as for Reco_trf.py"
        sys.exit(-1)
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine

    kfcu_set=False
    for arg in sys.argv:
        if arg.startswith("keepFullCommandUntouched"):
            kfcu_set=True
            break
    if kfcu_set:
        mySysArgv=sys.argv
    else:
        mySysArgv=sys.argv + ["keepFullCommandUntouched=True",]
        
    #Get the dico and pickle it
    inDic=BuildDicFromCommandLine(mySysArgv)
    #print final command
    print "\n###########"
    print "To run, do:"
    #print "%s --argdict=%s 2>&1 | tee Log.txt"%(mySysArgv[0],'inputDictionary.pickle')
    print "Reco_trf.py --argdict=inputDictionary.pickle 2>&1 | tee Log.txt"
    print "\n###########"
    print "To upload to AMI (manager only), use:  UploadAMITag.py"
    print "\n"
