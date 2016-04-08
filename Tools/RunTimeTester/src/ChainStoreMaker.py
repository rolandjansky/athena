# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from createDirectoryDictionary import createDirectoryDictionary

import os

class ChainStoreMaker:
    def __init__(self, argBag, enableMake):
        
        the_path         = createDirectoryDictionary(argBag, withoutReleaseName=True)['packages']
        self.csPath      = os.path.join(argBag.base, the_path, argBag.package)
    
        self.enableMake  = enableMake

    def chainStorePath(self, chainName):
        path = os.path.join(self.csPath, chainName)
        if self.enableMake:
            if not os.path.exists(path):
                os.makedirs(path)
        return path
        

