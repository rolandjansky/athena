# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, string
# -------------------------------------------------------------------------

def adjustPathEnvVar(siteSpecificsObj):        
    envPath   = os.environ['PATH']
    
    sitePaths = siteSpecificsObj.addToPath()
    toRemove  = []
    tokens    = []
    if sitePaths:
        tokens = sitePaths.split(':')
        [toRemove.append(t) for t in tokens if envPath.find(t)>=0]
        
        [tokens.remove(t) for t in toRemove]
        
        if tokens:
            tokens.append(envPath)
            os.environ['PATH'] = string.join(tokens, ':')
            
