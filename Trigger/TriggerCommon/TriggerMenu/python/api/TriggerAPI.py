# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__author__  = 'Javier Montejo'
__version__="$Revision: 1.00 $"
__doc__="Interface to retrieve lists of unprescaled triggers according to types and periods"

import sys, pickle
from TriggerMenu.api.TriggerInfo import TriggerInfo
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

class TriggerAPI:
    pickleFile = "TriggerInfo.pickle"
    try:
        with open(pickleFile, 'r') as f:
            dbQueries = pickle.load(f)
    except Exception as e:
        dbQueries = {}
    
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
            #print TriggerPeriod(subperiod), subperiodset
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
                with open(cls.pickleFile, 'w') as f:
                    pickle.dump( cls.dbQueries , f)
        if reparse: cls.dbQueries[period].reparse()

def main():
    ''' Run some tests '''
    from TriggerEnums import TriggerPeriod, TriggerType
    for triggerType in TriggerType:
        unprescaled = TriggerAPI.getLowestUnprescaled(TriggerPeriod.y2017periodAll,triggerType)
        #unprescaled = TriggerAPI.getLowestUnprescaled(332303,triggerType)
        print triggerType
        print sorted(unprescaled)
    print TriggerAPI.getLowestUnprescaled(337833,TriggerType.mu_bphys,matchPattern="bJpsi")
    print TriggerAPI.getLowestUnprescaled(337833,TriggerType.xe,matchPattern="pufit")

if __name__ == "__main__":
        sys.exit(main())

