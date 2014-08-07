#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import pickle,sys

def GetCommand(sysArgv):
    fname=sys.argv[1]
    f = open(fname, 'r')
    dic = pickle.load(f)
    f.close()

    print "original config:"
    print dic,"\n"

    dic['postExec']='PoolSvc.MaxFilesOpen = 32'

    print "updated config:"
    print dic

    outName="updatedConfig.pickle"
    f = open(outName, 'w')
    pickle.dump(dic, f)
    f.close()


if __name__ == '__main__':
    GetCommand(sys.argv)

        
