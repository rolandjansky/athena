#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TrigCostAnalysis.TableConstructorBase import TableConstructorBase, Column

'''
@file Thread_Occupancy_HLT_TableConstructor.py
@brief Contains TableConstructor classes per Thread_Occupancy_HLT table. Defines what
    should be saved in table and fills them
'''


class Thread_Occupancy_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Thread_Occupancy_HLT table
    '''
    def __init__(self, tableObj, underflowThreshold, overflowThreshold):
        super(). __init__(tableObj, underflowThreshold, overflowThreshold) 
        self.expectedHistograms = ["AlgCalls_perEvent", 
                                   "ThisAlgCalls_perEvent", 
                                   "OtherAlgCalls_perEvent",
                                   "AlgSlot_perCall",
                                   "WallTime_perEvent",
                                   "AlgTime_perEvent",
                                   "AlgTime_perEventFractional",
                                   "FrameworkTime_perEvent",
                                   "FrameworkTime_perEventFractional",
                                   "UnmonitoredTime_perEvent",
                                   "UnmonitoredTime_perEventFractional"]


    def defineColumns(self):        
        self.columns["name"]                    = Column("Thread Enumerator", "Counting number assinged to thread")
        self.columns["events"]                  = Column("Raw Active Events", "Raw underlying statistics on number of events processed at least partially on this thread")
        self.columns["eventsWeighted"]          = Column("Active Events", "Total weighted number of events processed at least partially on this thread")
        self.columns["callsPerEvent"]           = Column("Alg Calls/Event", "Mean number of alg calls per processed event on this thread")
        self.columns["thisCallsPerEvent"]       = Column("This Event Algs/Event", "Mean number of alg calls from the monitored event on this thread")
        self.columns["otherCallsPerEvent"]      = Column("Other Event Algs/Event", "Mean number of alg calls from other events on this thread")
        self.columns["wallTimePerEvent"]        = Column("Wall time/Event [ms]", "Mean Total event processing walltime per event")
        self.columns["algTimePerEvent"]         = Column("Alg Time/Event [ms]", "Mean algorithm execution time per event")
        self.columns["frameworkTimePerEvent"]   = Column("Framework Time/Event [ms]", "Mean time spent outside of algorithm execution per event")
        self.columns["unmonitoredTimePerEvent"] = Column("Unmonitored Time/Event [ms]", "Mean time which could not be monitored per event")


    def fillColumns(self, histName):
        self.columns['name'].addValue(histName)
        self.columns["events"].addValue(self.getHistogram("AlgCalls_perEvent").GetEntries())
        self.columns["eventsWeighted"].addValue(self.getHistogram("AlgCalls_perEvent").Integral())
        self.columns["callsPerEvent"].addValue(self.getHistogram("AlgCalls_perEvent").GetMean())
        self.columns["thisCallsPerEvent"].addValue(self.getHistogram("ThisAlgCalls_perEvent").GetMean())
        self.columns["otherCallsPerEvent"].addValue(self.getHistogram("OtherAlgCalls_perEvent").GetMean())
        self.columns["wallTimePerEvent"].addValue(self.getHistogram("WallTime_perEvent").GetMean())
        self.columns["algTimePerEvent"].addValue(self.getHistogram("AlgTime_perEvent").GetMean())
        self.columns["frameworkTimePerEvent"].addValue(self.getHistogram("FrameworkTime_perEvent").GetMean())
        self.columns["unmonitoredTimePerEvent"].addValue(self.getHistogram("UnmonitoredTime_perEvent").GetMean())
