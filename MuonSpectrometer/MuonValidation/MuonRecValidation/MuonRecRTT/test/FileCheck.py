# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# module: FileCheck.py

import sys, os, logging

from Logger import Logger

class FileCheck:
    
    def __init__(self, argDict={}):
        #
        self.logger    = Logger()
        self.logger.debug('instantiating file checker')
        self.success   = 0
        self.error     = -1
        self.fileName  = argDict.get('fileName')
    
    def run(self):
        #
        exists = True
        exists = exists & os.path.isfile(self.fileName)

        if not exists: self.logger.error('result file %s does not exist '%str(self.fileName))

        if exists: return self.success
        return self.error

