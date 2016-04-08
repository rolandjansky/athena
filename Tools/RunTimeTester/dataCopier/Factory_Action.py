# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CopyAction import CopyCastorFile, ListCastorFile
#from myCopyAction import CopyCastorFile, ListCastorFile
from CopyAction   import CopyDQ2Files,ListDQ2Files

class ActionFactory:
    def __init__(self):
        self.dict = {'active' : CopyCastorFile,
                     'passive': ListCastorFile}

    def create(self, mode):
        '''selects the action to be performed
        active  - copy file from castor
        passive - list file on castor
        '''
        if mode in self.dict.keys():
            return self.dict[mode]
        else:
            print 'Unknown mode in DataSetManager cfg, exiting...'
            sys.exit()

class DQ2ActionFactory:

    def __init__(self):
        self.dict = {'active' : CopyDQ2Files,
                     'passive': ListDQ2Files}

    def create(self, mode):
        '''selects the action to be performed
        active  - copy file from castor
        passive - list file on castor
        '''
        if mode in self.dict.keys():
            return self.dict[mode]
        else:
            print 'Unknown mode in DataSetManager cfg, exiting...'
            sys.exit()
