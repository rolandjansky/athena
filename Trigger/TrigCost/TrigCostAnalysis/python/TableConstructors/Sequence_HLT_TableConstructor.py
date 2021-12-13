#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TrigCostAnalysis.TableConstructorBase import TableConstructorBase, Column

'''
@file Sequence_HLT_TableConstructor.py
@brief Contains TableConstructor classes per Sequence_HLT table. Defines what
    should be saved in table and fills them
'''

# TODO Uncomment values after fixing Time_perCall histogram
class Sequence_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Sequence_HLT table
    '''
    def __init__(self, tableObj, underflowThreshold, overflowThreshold):
        super(). __init__(tableObj, underflowThreshold, overflowThreshold) 
        self.expectedHistograms = ["Sequence_perEvent",
                                   "AlgCalls_perEvent",
                                   "Time_perCall", 
                                   "Time_perEvent",
                                   "Request_perEvent",
                                   "NetworkRequest_perEvent",
                                   "CachedROBSize_perEvent",
                                   "NetworkROBSize_perEvent",
                                   "RequestTime_perEvent"]


    def defineColumns(self):
        self.columns["name"]                    = Column("Name", "Sequence name")
        self.columns["events"]                  = Column("Raw Active Events", "Raw underlying statistics on how many events in which this sequence was executed.")
        self.columns["eventsWeighted"]          = Column("Active Events", "How many events in which this sequence was executed.")
        self.columns["callsPerEvent"]           = Column("Calls/Event", "Total number of calls made to this sequence.")
        self.columns["callsSlow"]               = Column("Calls > 1000 ms", "Number of sequence executions which were particularly slow.")
        self.columns["eventRate"]               = Column("Event Rate [Hz]", "Rate in this run range of events with at least one execution of this sequence.", True)
        self.columns["callRate"]                = Column("Call Rate [Hz]", "Rate in this run range of calls to this sequence.", True)
        self.columns["totalTimeSec"]            = Column("Sequence Total Time [s]", "Total time for this sequence")
        self.columns["totalTimePerc"]           = Column("Sequence Total Time [%]", "Total time for this sequence as a percentage of all sequence executions in this run range")
        self.columns["timePerCall"]             = Column("Sequence Total Time/Call [ms]", "Average execution time per sequence call in this run range.")
        self.columns["timePerEvent"]            = Column("Sequence Total Time/Event [ms]", "Mean weighted alg time. Normalised to all events with one or more alg calls")
        self.columns["algsPerEvent"]            = Column("Run Algs/Event", "Total number of algorithms executed by this sequence.")
        self.columns["requestTimePerEvent"]     = Column("ROS Data Request Time/Event [ms]", "Average time waiting for ROS data per event for  events with at least one execution in this run range")
        self.columns["dataRate"]                = Column("Data Request Rate [Hz]", "Rate of calls to the ROS from algorithms in this sequence in this run range", True)
        self.columns["retrievedDataRate"]       = Column("Retrieved ROB Rate [Hz]", "Rate of ROB retrievals from algorithms in this sequence in this run range", True)
        self.columns["cachedDataSizeRate"]      = Column("Cached ROB Rate [kB/s]", "Average size of cached ROB data fetches for algorithms in this sequence in this run range", True)
        self.columns["retrievedDataSizeRate"]   = Column("Retrieved ROB Rate [kB/s]", "Average size of retrieved ROB data fetches for algorithms in this sequence in this run range")
    
    
    def fillColumns(self, histName):
        weightedEvents = self.getHistogram("Sequence_perEvent").Integral()
        weightedCalls = self.getXWeightedIntegral("Sequence_perEvent", isLog=False)
        slowCalls = self.getHistogram("Time_perCall").Integral(self.getHistogram("Time_perCall").FindBin(1000.), self.getHistogram("Time_perCall").GetNbinsX())

        self.columns["name"].addValue(histName)
        self.columns["events"].addValue(self.getHistogram("Sequence_perEvent").GetEntries())
        self.columns["eventsWeighted"].addValue(weightedEvents)
        self.columns["callsPerEvent"].addValue(self.getHistogram("Sequence_perEvent").GetMean())
        self.columns["callsSlow"].addValue(slowCalls)
        self.columns["eventRate"].addValue(weightedEvents)
        self.columns["callRate"].addValue(weightedCalls)
        self.columns["totalTimeSec"].addValue(self.getXWeightedIntegral("Time_perCall", isLog=True) * 1e-3)
        #self.columns["totalTimePerc"] in post processing
        self.columns["timePerCall"].addValue((self.getHistogram("Time_perCall").GetMean()))
        self.columns["timePerEvent"].addValue(self.getHistogram("Time_perEvent").GetMean())
        self.columns["algsPerEvent"].addValue(self.getHistogram("AlgCalls_perEvent").GetMean())
        self.columns["requestTimePerEvent"].addValue(self.getHistogram("RequestTime_perEvent").GetMean())
        self.columns["dataRate"].addValue(self.getXWeightedIntegral("Request_perEvent", isLog=False))
        self.columns["retrievedDataRate"].addValue(self.getXWeightedIntegral("NetworkRequest_perEvent", isLog=False))
        self.columns["cachedDataSizeRate"].addValue(self.getXWeightedIntegral("CachedROBSize_perEvent", isLog=False))
        self.columns["retrievedDataSizeRate"].addValue(self.getXWeightedIntegral("NetworkROBSize_perEvent", isLog=False))


    def postProcessing(self):
        totalTimeEntries = self.columns["totalTimeSec"].content
        totalTime = sum(totalTimeEntries)

        for entry in totalTimeEntries:
            self.columns["totalTimePerc"].addValue(100 * entry / totalTime)
