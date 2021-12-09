#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TrigCostAnalysis.TableConstructorBase import TableConstructorBase, Column
from AthenaCommon.Logging import logging
log = logging.getLogger('Algorithm_HLT')

'''
@file Algorithm_HLT_TableConstructor.py
@brief Contains TableConstructor classes per Algorithm_HLT table. Defines what
    should be saved in table and fills them
'''


class Algorithm_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Algorithm_HLT table
    '''
    def __init__(self, tableObj, underflowThreshold, overflowThreshold):
        super(). __init__(tableObj, underflowThreshold, overflowThreshold) 
        # Dump summary of mean time execution for all the algorithms to the log
        self.dumpSummary = False
        self.expectedHistograms = ["Time_perCall", 
                                   "FirstTime_perEvent", 
                                   "Time_perEvent",
                                   "Time_perEventFractional",
                                   "AlgCalls_perEvent",
                                   "RoIID_perCall",
                                   "InEventView_perCall",
                                   "Request_perEvent",
                                   "NetworkRequest_perEvent",
                                   "CachedROBSize_perEvent",
                                   "NetworkROBSize_perEvent",
                                   "RequestTime_perEvent"]


    def defineColumns(self):
        self.columns["name"]                    = Column("Name", "Algorithms name")
        self.columns["events"]                  = Column("Raw Active Events", "Raw underlying statistics on number of events processed with the alg active")
        self.columns["eventsWeighted"]          = Column("Active Events", "Total weighted number of events with the alg active")
        self.columns["callsPerEvent"]           = Column("Calls/Event", "Mean number of alg calls in events with one or more calls")
        self.columns["callsSlow"]               = Column("Calls > 1000 ms", "Weighted number of events in which this alg was exceptionally slow")
        self.columns["eventRate"]               = Column("Event Rate [Hz]", "Walltime normalised rate of events with one or more executions of the alg", True)
        self.columns["callRate"]                = Column("Call Rate [Hz]", "Walltime normalised rate of calls in events with one or more executions of the alg", True)
        self.columns["totalTimeSec"]            = Column("Alg Total Time [s]", "Total weighted integrated walltime of the alg")
        self.columns["totalTimePerc"]           = Column("Alg Total Time [%]", "Total weighted integrated walltime of the alg as a percentage of all algs")
        self.columns["timePerCall"]             = Column("Alg Total Time/Call [ms]", "Mean weighted alg time. Normalised to all alg calls")
        self.columns["timePerEvent"]            = Column("Alg Total Time/Event [ms]", "Mean weighted alg time. Normalised to all events with one or more alg calls")
        self.columns["requestTimePerEvent"]     = Column("ROS Data Request Time/Event [ms]", "Average time waiting for ROS data per event for  events with at least one execution in this run range")
        self.columns["dataRate"]                = Column("Data Request Rate [Hz]", "Rate of calls to the ROS from this algorithm in this run range", True)
        self.columns["retrievedDataRate"]       = Column("Retrieved ROB Rate [Hz]", "Rate of ROB retrievals from this algorithm in this run range", True)
        self.columns["cachedDataSizeRate"]      = Column("Cached ROB Rate [kB/s]", "Average size of cached ROB data fetches for this algorithm in this run range", True)
        self.columns["retrievedDataSizeRate"]   = Column("Retrieved ROB Rate [kB/s]", "Average size of retrieved ROB data fetches for this algorithm in this run range")
    
    
    def fillColumns(self, histName):
        weightedEvents = self.getHistogram("AlgCalls_perEvent").Integral()
        weightedCalls = self.getXWeightedIntegral("AlgCalls_perEvent", isLog=False)
        slowCalls = self.getHistogram("Time_perCall").Integral(self.getHistogram("Time_perCall").FindBin(1000.), self.getHistogram("Time_perCall").GetNbinsX())

        self.columns["name"].addValue(histName)
        self.columns["events"].addValue(self.getHistogram("AlgCalls_perEvent").GetEntries())
        self.columns["eventsWeighted"].addValue(weightedEvents)
        self.columns["callsPerEvent"].addValue(self.getHistogram("AlgCalls_perEvent").GetMean())
        self.columns["callsSlow"].addValue(slowCalls)
        self.columns["eventRate"].addValue(weightedEvents)
        self.columns["callRate"].addValue(weightedCalls)
        self.columns["totalTimeSec"].addValue(self.getXWeightedIntegral("Time_perCall", isLog=True) * 1e-3)
        #self.columns["totalTimePerc"] in post processing
        self.columns["timePerCall"].addValue(self.getHistogram("Time_perCall").GetMean())
        self.columns["timePerEvent"].addValue(self.getHistogram("Time_perEvent").GetMean())
        self.columns["requestTimePerEvent"].addValue(self.getHistogram("RequestTime_perEvent").GetMean())
        self.columns["dataRate"].addValue(self.getXWeightedIntegral("Request_perEvent", isLog=False))
        self.columns["retrievedDataRate"].addValue(self.getXWeightedIntegral("NetworkRequest_perEvent", isLog=False))
        self.columns["cachedDataSizeRate"].addValue(self.getXWeightedIntegral("CachedROBSize_perEvent", isLog=False))
        self.columns["retrievedDataSizeRate"].addValue(self.getXWeightedIntegral("NetworkROBSize_perEvent", isLog=False))

        if self.dumpSummary:
            log.info("Algorithm: {0:300} Mean Time per call [ms]: {1:10.4} Mean Time per event [ms]: {2:10.3}".format(histName, self.getHistogram("Time_perCall").GetMean(), self.getHistogram("Time_perEvent").GetMean()))


    def postProcessing(self):

        totalTimeEntries = self.columns["totalTimeSec"].content
        totalTime = sum(totalTimeEntries)

        for entry in totalTimeEntries:
            self.columns["totalTimePerc"].addValue(100 * entry / totalTime)