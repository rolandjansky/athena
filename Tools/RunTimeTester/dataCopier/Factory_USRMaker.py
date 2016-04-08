# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from UserStuffRetrieverMaker import USRProjectMaker
from UserStuffRetrieverMaker import USRMonolithicMaker


class USRMakerFactory:
    def __init__(self):
        self.dict = {'project'   : USRProjectMaker,
                     'monolithic': USRMonolithicMaker}

    def create(self, releaseType):
        '''selects the action to be performed
        active  - copy file from castor
        passive - list file on castor
        '''
        if releaseType in self.dict.keys():
            return self.dict[releaseType]
        else:
            print 'Unknown releaseType in RTT topLevelConfig cfg, exiting...'
            sys.exit()
