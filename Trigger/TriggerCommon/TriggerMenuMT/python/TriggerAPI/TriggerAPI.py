# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__author__  = 'Javier Montejo'
__version__="$Revision: 2.0 $"
__doc__="Interface to retrieve lists of unprescaled triggers according to types and periods"

import sys
from TriggerMenuMT.TriggerAPI.TriggerInfo import TriggerInfo
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
from TriggerMenuMT.TriggerAPI import SerializeAPI
from AthenaCommon.Logging import logging

class TriggerAPI:
    log = logging.getLogger( 'TriggerMenuMT.TriggerAPI' )
    dbQueries = {}
    customGRL = None
    release   = None
    cacheread = False

    @classmethod
    def init(cls):
        if cls.cacheread: return
        cls.cacheread = True
        cls.dbQueries = SerializeAPI.load()
        todel = [(p,grl) for p,grl in cls.dbQueries if p  & TriggerPeriod.future]
        for key in todel:
            del cls.dbQueries[key]

    @classmethod
    def setRelease(cls, release):
        import re
        if release and re.match(r'21\.1(\.[0-9]+)+$',release):
            cls.release = release
        elif release=="current": #Don't allow the release to be automatically overwritten
            cls.release = release
        else:
            cls.log.warning("Release doesn't seem to be a well-formed 21.1 release, ignoring: "+release)

    @classmethod
    def setCustomGRL(cls, grl):
        if TriggerInfo.testCustomGRL(grl):
            cls.customGRL = grl
        else:
            cls.log.warning("Couldn't set GRL: "+grl)
            cls.log.warning("Will use default GRL")
            cls.customGRL = None

    @classmethod
    def getLowestUnprescaled(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", livefraction=1.0, reparse=False):
        ''' Returns a list of the lowest-pt-threshold HLT chains that were always unprescaled in the given period.
            period: see TriggerEnums.TriggerPeriod for all possibilities, recommeded TriggerPeriod.y2018
            triggerType: see TriggerEnums.TriggerType for all possibilities, example TriggerType.el_single
            additionalTriggerType: can request additional types to match, use TriggerType.ALL to show combined triggers of any kind
                                   accepts also a list as input in that case all types have to match
            matchPattern: provide additionally a regex-like expression to be applied
            livefraction: accept items that are not unprescaled but have a live fraction above this threshold, example 0.95
                          The live fraction is only an approximation, weighting the number of lumiblocks by prescale.
        '''
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[(period,cls.customGRL)]._getLowestUnprescaled(triggerType, additionalTriggerType, matchPattern, livefraction)
    
    @classmethod
    def getLowestUnprescaledAnyPeriod(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", livefraction=1.0, reparse=False):
        ''' Returns a list of the lowest-pt-threshold HLT chains that were unprescaled in at least one of 
            the subperiods within the given period. The lowest granularity can be seen in TriggerEnums.TriggerPeriod
            See getLowestUnprescaled for a detailed description of the options
        '''
        lowset = set()
        for i, ibin in enumerate(reversed(bin(period)[2:])): #to binary
            ibin = int(ibin)
            if not ibin: continue
            subperiod = 2**i
            cls._loadTriggerPeriod(subperiod, reparse)
            subperiodset = set( cls.dbQueries[(subperiod,cls.customGRL)]._getLowestUnprescaled(triggerType, additionalTriggerType, matchPattern, livefraction) )
            lowset |= subperiodset
        return list(lowset)
    
    @classmethod
    def getUnprescaled(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", livefraction=1.0, reparse=False):
        ''' Returns a list of always-unprescaled HLT chains, including backup items with higher thresholds.
            See getLowestUnprescaled for a detailed description of the options
        '''
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[(period,cls.customGRL)]._getUnprescaled(triggerType, additionalTriggerType, matchPattern, livefraction)
    
    @classmethod
    def getUnprescaledAnyPeriod(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", livefraction=1.0, reparse=False):
        ''' Returns a list of HLT chains that were unprescaled in at least one of 
            the subperiods within the given period. The lowest granularity can be seen in TriggerEnums.TriggerPeriod
            See getLowestUnprescaled for a detailed description of the options
        '''
        lowset = set()
        for i, ibin in enumerate(reversed(bin(period)[2:])): #to binary
            ibin = int(ibin)
            if not ibin: continue
            subperiod = 2**i
            cls._loadTriggerPeriod(subperiod, reparse)
            subperiodset = set( cls.dbQueries[(subperiod,cls.customGRL)]._getUnprescaled(triggerType, additionalTriggerType, matchPattern, livefraction) )
            lowset |= subperiodset
        return list(lowset)
        
    @classmethod
    def getInactive(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", livefraction=1e-99, reparse=False):
        ''' Returns a list of HLT chains that were fully inactive, excluding disabled chains in rerun.
            See getLowestUnprescaled for a detailed description of the options
        '''
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[(period,cls.customGRL)]._getInactive(triggerType, additionalTriggerType, matchPattern, livefraction)
    
    @classmethod
    def getActive(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", livefraction=1e-99, reparse=False):
        ''' Returns a list of HLT chains that were active at some point, including disabled chains in rerun.
            See getLowestUnprescaled for a detailed description of the options
        '''
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[(period,cls.customGRL)]._getActive(triggerType, additionalTriggerType, matchPattern, livefraction)
    
    @classmethod
    def getAllHLT(cls, period, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="",  livefraction=0, reparse=False):
        ''' Returns a map of {HLT chains: average live fraction} for a given period.
            The average live fraction is an approximation weighting the number of lumiblocks by prescale.
            *** Don't use this number in analysis!!! ***
            See getLowestUnprescaled for a detailed description of the options
        '''
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[(period,cls.customGRL)]._getAllHLT(triggerType, additionalTriggerType, matchPattern, livefraction)
        
    @classmethod
    def checkPeriodConsistency(cls, period=TriggerPeriod.future, triggerType=TriggerType.ALL, additionalTriggerType=TriggerType.UNDEFINED, matchPattern="", reparse=False):
        ''' Returns a list of triggers that are tighter than the lowest unprescaled but are not flagged as primary
            This only makes sense for future periods, the past is already consistent :)
            See getLowestUnprescaled for a detailed description of the options
        '''
        period &= TriggerPeriod.future #Only meaningful for future periods
        cls._loadTriggerPeriod(period,reparse)
        return cls.dbQueries[(period,cls.customGRL)]._checkPeriodConsistency(triggerType, additionalTriggerType, matchPattern)
        
    @classmethod
    def _loadTriggerPeriod(cls, period, reparse):
        if not period & TriggerPeriod.future: cls.init()
        if (period,cls.customGRL) not in cls.dbQueries:
            if TriggerPeriod.isRunNumber(period) or (isinstance(period,TriggerPeriod) and period.isBasePeriod()):
                cls.dbQueries[(period,cls.customGRL)] = TriggerInfo(period,cls.customGRL,cls.release)
            else:
                basePeriods = [tp for tp in TriggerPeriod.basePeriods() if tp & period]
                for bp in basePeriods:
                    cls._loadTriggerPeriod(bp,reparse)
                cls.dbQueries[(period,cls.customGRL)] = TriggerInfo.merge([cls.dbQueries[(bp,cls.customGRL)] for bp in basePeriods])
        if reparse: cls.dbQueries[(period,cls.customGRL)].reparse()

    @classmethod
    def dumpAPI(cls, full=False):
        if not full: #store only what is not in the central pickle/json
            for period,grl in list(cls.dbQueries.keys()):
                if TriggerPeriod.isRunNumber(period) or (isinstance(period,TriggerPeriod) and period.isBasePeriod()): continue
                del cls.dbQueries[(period,grl)]
        SerializeAPI.dump(cls.dbQueries)
        cls.log.info(sorted(cls.dbQueries.keys()))

def main(dumpFullAPI=False):
    ''' Run some tests or dump the full pickle/json for CalibPath '''
    log = logging.getLogger( 'TriggerMenuMT.TriggerAPI.main' )

    if dumpFullAPI:
        for triggerPeriod in TriggerPeriod:
            unprescaled = TriggerAPI.getLowestUnprescaled(triggerPeriod,TriggerType.mu_single)
            log.info(triggerPeriod)
            log.info(sorted(unprescaled))
        #Cache also one run for the example script
        unprescaled = TriggerAPI.getLowestUnprescaled(337833,TriggerType.mu_single)
        log.info(337833)
        log.info(sorted(unprescaled))
        TriggerAPI.dumpAPI(full=True)
    else:
        try: period = int(sys.argv[1])
        except Exception: period = TriggerPeriod.y2018
        for triggerType in TriggerType:
            unprescaled = TriggerAPI.getLowestUnprescaled(period,triggerType)
            log.info(triggerType)
            log.info(sorted(unprescaled))

if __name__ == "__main__":
        dumpFullAPI = ("dumpFullAPI" in sys.argv)
        sys.exit(main(dumpFullAPI))

