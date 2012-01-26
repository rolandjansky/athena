# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from os import popen,system,makedirs,access,environ
from os.path import exists

def GetFiles(path) :
    if path[-1] != '/':
        path += '/'
    fileNames = popen('ls %s | grep AOD' %(path)).readlines()
    out = []
    
    for key,val in dict( [ (path+f).strip().split(".root") \
                           for f in fileNames ] ).iteritems():
        out.append(key+".root"+val)
        
        
    out.sort()

    return out
