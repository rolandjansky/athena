#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: David Cote (DESY), September 2008
# Usage:
#   -determines the transform behavior dynamically, according to args values
#       |-->note: this was the idea, but it was never not really done this way in the end... (David, August 2010)

from PATJobTransforms.Configuration import ConfigDic
from PATJobTransforms.ArgDicTools import KeyExceptionCatcher
    
def AutoConfigureFromDic(trf,zeDic):
    #configure the transform from zeDic, but also modify zeDic as needed
    origDic=zeDic.copy()

    #loop over origDic, since zeDic may be modified by the functions returned by ConfigDic
    for key in origDic.keys():
        if ConfigDic.has_key(key):        
            ConfigDic[key](trf,zeDic)
        elif KeyExceptionCatcher(key) is "OK":
            print "No ConfigDic entry for key '%s'."
            print "This allowed for special options like: --ignoreunknown, --athenaopts, -h, etc."
        else:
            raise RuntimeError("key %s is not defined in ConfigDic"%key)

    return zeDic

