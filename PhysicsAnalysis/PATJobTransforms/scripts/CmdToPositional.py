#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print "\n * * * CmdToPositional * * *\n"
def PositionInList(key,poslist):
    ind=-1
    i=0
    for item in poslist:
        if key==item:
            if ind<0:
                ind=i
            else:
                raise RuntimeError("Key '%s' requested twice. Not allowed!"%key)
        i+=1
    if ind<0:
        raise RuntimeError("Key '%s' not found!"%key)
    else:
        print "key '%s' has position %i"%(key,ind)
    return ind

if __name__ == '__main__':
    import sys
    #Remove CmdToPickledDic itself...
    myTrf=sys.argv[1]
    mySysArgv=sys.argv[1:]

    #Get positional values for myTrf
    if myTrf=='Reco_trf.py':
        from PATJobTransforms.Reco_trf import ListOfDefaultPositionalKeys
    elif myTrf=='MergeBS_trf.py':
        from PATJobTransforms.MergeBS_trf import ListOfDefaultPositionalKeys
    elif myTrf=='MergePool_trf.py':
        from PATJobTransforms.MergePool_trf import ListOfDefaultPositionalKeys
    else:
        raise RuntimeError("CmdToPositional not implemented for transform: %s"%myTrf)
    print "Known positional keys for %s: "%myTrf
    print ListOfDefaultPositionalKeys,"\n"

    #Initialize positional value list:
    posList=[]
    for i in xrange(len(ListOfDefaultPositionalKeys)):
        posList+=['NONE']

    #Transform mySysArbs into a dictionnary
    from PATJobTransforms.ArgDicTools import SysArgvToDico,KeyExceptionCatcher
    dic=SysArgvToDico(mySysArgv)
    if not isinstance(dic,dict):
        raise RuntimeError("Unable to interpret given command-line...")
    
    #Now fill relevant values
    print ""
    for key in dic.keys():
        p=PositionInList(key,ListOfDefaultPositionalKeys)
        posList[p]=dic[key]
        if KeyExceptionCatcher(key)=="OK":
            posList[p]=key
    
    #build and print the final command
    cmd=myTrf
    for val in posList:
        cmd+=" "
        cmd+=str(val)
    cmd+=" 2>&1 | tee Log.txt"
    print "\nPositional command:"
    print cmd

