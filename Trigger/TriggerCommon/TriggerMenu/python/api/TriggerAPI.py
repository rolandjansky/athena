# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__author__  = 'Javier Montejo'
__version__="$Revision: 1.01 $"
__doc__="Interface to retrieve lists of unprescaled triggers according to types and periods"

import sys, pickle, os.path
from TriggerMenu.api.TriggerInfo import TriggerInfo
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
from PathResolver import PathResolver
from AthenaCommon.Logging import logging

class TriggerAPI:
    log = logging.getLogger( 'TriggerMenu.api.TriggerAPI.py' )
    centralPickleFile = PathResolver.FindCalibFile("TriggerMenu/TriggerInfo_20181112.pickle")
    if not centralPickleFile: 
        log.warning("Couldn't find primary pickle file, try backup")
        centralPickleFile = PathResolver.FindCalibFile("TriggerMenu/TriggerInfo_20180925.pickle")
    if centralPickleFile: 
        log.info("Found pickle file:"+centralPickleFile)
        centralPickleFile = os.path.realpath(centralPickleFile)
    else: log.error("Couldn't find backup pickle file")
    privatePickleFile = "TriggerInfo.pickle"
    dbQueries = {}
    privatedbQueries = {}
    customGRL = None
    release   = None
    pickleread = False

    @classmethod
    def init(cls):
        if cls.pickleread: return
        cls.pickleread = True
        if cls.centralPickleFile:
            try:
                with open(cls.centralPickleFile, 'r') as f:
                    cls.log.info("Reading cached information from: "+cls.centralPickleFile)
                    cls.dbQueries = pickle.load(f)
            except (pickle.PickleError, ValueError):
                cls.log.info("Reading cached information failed")
                cls.dbQueries = {}
        else:
            cls.dbQueries = {}
        try:
            with open(cls.privatePickleFile, 'r') as f:
                cls.privatedbQueries = pickle.load(f)
                cls.dbQueries.update(cls.privatedbQueries)
        except (pickle.PickleError, ValueError):
            cls.log.error("Error unpickling the private file")
        except IOError:
            pass

    @classmethod
    def setRelease(cls, release):
        import re
        if release and re.match('21\.1(\.[0-9]+)+$',release):
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
            period: see TriggerEnums.TriggerPeriod for all possibilities, recommeded TriggerPeriod.y2017
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
                cls.privatedbQueries[(period,cls.customGRL)] = cls.dbQueries[(period,cls.customGRL)]
                if not period & TriggerPeriod.future or TriggerPeriod.isRunNumber(period): 
                    #Don't pickle TM information since it can change, very cheap to retrieve anyway
                    with open(cls.privatePickleFile, 'w') as f:
                        pickle.dump( cls.privatedbQueries , f)
            else:
                basePeriods = [tp for tp in TriggerPeriod.basePeriods() if tp & period]
                for bp in basePeriods:
                    cls._loadTriggerPeriod(bp,reparse)
                cls.dbQueries[(period,cls.customGRL)] = TriggerInfo.merge([cls.dbQueries[(bp,cls.customGRL)] for bp in basePeriods])
                cls.privatedbQueries[(period,cls.customGRL)] = cls.dbQueries[(period,cls.customGRL)]
        if reparse: cls.dbQueries[(period,cls.customGRL)].reparse()

    @classmethod
    def dumpFullPickle(cls):
        for period,grl in cls.dbQueries.keys():
            if TriggerPeriod.isRunNumber(period) or (isinstance(period,TriggerPeriod) and period.isBasePeriod()): continue
            del cls.dbQueries[(period,grl)]
        with open(cls.privatePickleFile, 'w') as f:
            pickle.dump( cls.dbQueries , f)
        print sorted(cls.dbQueries.keys())

def main(dumpFullPickle=False):
    ''' Run some tests or dump the full pickle for CalibPath '''

    if dumpFullPickle:
        for triggerPeriod in TriggerPeriod:
            unprescaled = TriggerAPI.getLowestUnprescaled(triggerPeriod,TriggerType.mu_single)
            print triggerPeriod
            print sorted(unprescaled)
        #Cache also one run for the example script
        unprescaled = TriggerAPI.getLowestUnprescaled(337833,TriggerType.mu_single)
        print 337833
        print sorted(unprescaled)
        TriggerAPI.dumpFullPickle()
    else:
        try: period = int(sys.argv[1])
        except: period = TriggerPeriod.y2018
        for triggerType in TriggerType:
            unprescaled = TriggerAPI.getLowestUnprescaled(period,triggerType)
            print triggerType
            print sorted(unprescaled)

if __name__ == "__main__":
        dumpFullPickle = ("dumpFullPickle" in sys.argv)
        sys.exit(main(dumpFullPickle))

