# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__author__  = 'Javier Montejo'
__version__="$Revision: 1.01 $"
__doc__="Interface to retrieve lists of unprescaled triggers according to types and periods"

import sys, pickle
from TriggerMenu.api.TriggerInfo import TriggerInfo
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

class TriggerAPI:
    centralPickleFile = "/afs/cern.ch/user/j/jmontejo/public/TriggerInfo.pickle" #FIXME
    privatePickleFile = "TriggerInfo.pickle"
    try:
        with open(centralPickleFile, 'r') as f:
            print "Reading cached information"
            dbQueries = pickle.load(f)
    except Exception as e:
        print "Reading cached information failed, please report to jmontejo@cern.ch"
        dbQueries = {}
    try:
        with open(privatePickleFile, 'r') as f:
            print "DEBUG OPEN:",centralPickleFile
            tmpdbQueries = pickle.load(f)
            dbQueries.update(tmpdbQueries)
    except Exception as e:
        pass

    @classmethod
    def getLowestUnprescaled(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", reparse=False):
        ''' Returns a list of the lowest-pt-threshold HLT chains that were always unprescaled in the given period.
            period: see TriggerEnums.TriggerPeriod for all possibilities, recommeded TriggerPeriod.y2017
            triggerType: see TriggerEnums.TriggerType for all possibilities, example TriggerType.el_single
            additionalTriggerType: can request additional types, use TriggerType.ALL to show combined triggers of any kind
            matchPattern: provide additionally a regex-like expression to be applied
        '''
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[period]._getLowestUnprescaled(triggerType, additionalTriggerType, matchPattern)
    
    @classmethod
    def getLowestUnprescaledAnyPeriod(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", reparse=False):
        ''' Returns a list of the lowest-pt-threshold HLT chains that were unprescaled in at least one of 
            the subperiods within the given period. The lowest granularity can be seen in TriggerEnums.TriggerPeriod
            period: see TriggerEnums.TriggerPeriod for all possibilities, recommeded TriggerPeriod.y2017
            triggerType: see TriggerEnums.TriggerType for all possibilities, example TriggerType.el_single
            additionalTriggerType: can request additional types to match, use TriggerType.ALL to show combined triggers of any kind
                                   accepts also a list as input in that case all types have to match
            matchPattern: provide additionally a regex-like expression to be applied
        '''
        lowset = set()
        for i, ibin in enumerate(reversed(bin(period)[2:])): #to binary
            ibin = int(ibin)
            if not ibin: continue
            subperiod = 2**i
            cls._loadTriggerPeriod(subperiod, reparse)
            subperiodset = set( cls.dbQueries[subperiod]._getLowestUnprescaled(triggerType, additionalTriggerType, matchPattern) )
            lowset |= subperiodset
        return list(lowset)
    
    @classmethod
    def getUnprescaled(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", reparse=False):
        ''' Returns a list of always-unprescaled HLT chains, including backup items with higher thresholds.
            period: see TriggerEnums.TriggerPeriod for all possibilities, recommeded TriggerPeriod.y2017
            triggerType: see TriggerEnums.TriggerType for all possibilities, example TriggerType.el_single
            additionalTriggerType: can request additional types, use TriggerType.ALL to show combined triggers of any kind
            matchPattern: provide additionally a regex-like expression to be applied
        '''
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[period]._getUnprescaled(triggerType, additionalTriggerType, matchPattern)
        
    @classmethod
    def getAllHLT(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", reparse=False):
        ''' Returns a map of {HLT chains: average live fraction} for a given period.
            The average live fraction is an approximation weighting the number of lumiblocks by prescale.
            *** Don't use this number in analysis!!! ***
            period: see TriggerEnums.TriggerPeriod for all possibilities, recommeded TriggerPeriod.y2017
            triggerType: see TriggerEnums.TriggerType for all possibilities, example TriggerType.el_single
            additionalTriggerType: can request additional types, use TriggerType.ALL to show combined triggers of any kind
            matchPattern: provide additionally a regex-like expression to be applied
        '''
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[period]._getAllHLT(triggerType, additionalTriggerType, matchPattern)

    @classmethod
    def _loadTriggerPeriod(cls, period, reparse):
        if period not in cls.dbQueries:
            cls.dbQueries[period] = TriggerInfo(period)
            if not period & TriggerPeriod.future or period >= TriggerPeriod.runNumber: 
                #Don't pickle TM information since it can change, very cheap to retrieve anyway
                with open(cls.privatePickleFile, 'w') as f:
                    pickle.dump( cls.dbQueries , f)
        if reparse: cls.dbQueries[period].reparse()

def main():
    ''' Run some tests '''
    for triggerType in TriggerType:
        unprescaled = TriggerAPI.getLowestUnprescaled(332303,triggerType)
        print triggerType
        print sorted(unprescaled)

if __name__ == "__main__":
        sys.exit(main())

