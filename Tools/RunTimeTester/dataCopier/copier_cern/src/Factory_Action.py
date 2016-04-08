# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CopyAction  import CopyCastorFile, ListCastorFile
from CopyAction  import CopyNonDQ2File, CopyNonDQ2FileCERN
from CopyAction  import CopyDQ2Files,ListDQ2Files
import sys

class ActionFactory:
    def __init__(self):
        self.dict = {('active','UCLslc3')   : CopyNonDQ2File,
                     ('active','Lancaster') : CopyNonDQ2File,
                     ('active', 'CERN')     : CopyNonDQ2FileCERN,
                     ('active', 'CERNslc4') : CopyNonDQ2FileCERN, 
                     ('passive', None )     : ListCastorFile}

    def create(self, mode, site):
        '''selects the action to be performed
        active  - copy file from castor
        passive - list file on castor
        '''
        if (mode,site) in self.dict.keys():
            return self.dict[(mode,site)]
        else:
            print 'Unknown mode in DataSetManager cfg, exiting...mode: %s, site: %s' % (mode, site)
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
            print 'Unknown mode in DataSetManager cfg, exiting...mode: %s' % mode
            sys.exit()
