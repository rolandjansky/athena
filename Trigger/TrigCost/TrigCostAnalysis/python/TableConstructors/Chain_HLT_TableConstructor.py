#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TrigCostAnalysis.TableConstructorBase import TableConstructorBase, Column

'''
@file Chain_HLT_TableConstructor.py
@brief Contains TableConstructor classes per Chain_HLT table. Defines what
    should be saved in table and fills them
'''


class Chain_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Chain_HLT table
    '''
    def __init__(self, tableObj, underflowThreshold, overflowThreshold):
        super(). __init__(tableObj, underflowThreshold, overflowThreshold) 
        self.totalTime = 0 # Total time of algoirthms execution
        self.expectedHistograms = ["Group_perCall", 
                                   "Chain_perEvent", 
                                   "AlgCalls_perEvent",
                                   "Time_perEvent",
                                   "Time_perCall",
                                   "UniqueTime_perCall",
                                   "ChainPassed_perEvent",
                                   "Request_perEvent",
                                   "NetworkRequest_perEvent",
                                   "CachedROBSize_perEvent",
                                   "NetworkROBSize_perEvent"]

    def defineColumns(self):
        self.columns['name'] = Column("Name", "Chain name")
        self.columns['group'] = Column("Group", "Bandwidth group this chain is associated to")
        self.columns['events'] = Column("Raw Active Events", "Raw underlying statistics on the number of events in which this ROS was accessed")
        self.columns['eventsWeighted'] = Column("Active Events", "How many events in which this sequence was executed")
        self.columns['time'] = Column("Time Per Event [ms]", "Average execution time of this chain per event")
        self.columns['chainRate'] = Column("Execute Rate [Hz]", "Number of chain executions normalised to the wall time for this run range", True)
        self.columns['passFraction'] = Column("Pass Fraction [%]", "What percentage of events pass events are kept")
        self.columns["callsSlow"] = Column("Calls > 1000 ms", "Number of algorithm executions which were particularly slow")
        self.columns['totalTime'] = Column("Total Chain Time [s]", "Total time used by this chain for this run range")
        self.columns['totalTimeFrac'] = Column("Total Chain Time [%]", "Total chain time as a percentage of the total time of all chains in this run range")
        self.columns["totalUniqTime"] = Column("Total Unique Time [s]", "Total time used by algorithms for this chain for this run range")
        self.columns['totalUniqTimeFrac'] = Column("Total Unique Time [%]", "Total unique chain time as a percentage of the total time of all chains in this run range")
        self.columns['algPerEvent'] = Column("Run Algs/Event", "Total number of algorithms executed by this chain")
        self.columns["dataRate"] = Column("Data Request Rate [Hz]", "Rate of calls to the ROS from this algorithm in this run range", True)
        self.columns["retrievedDataRate"] = Column("Retrieved ROB Rate [Hz]", "Rate of ROB retrievals from this algorithm in this run range", True)
        self.columns["cachedDataSizeRate"] = Column("Cached ROB Rate [kB/s]", "Average size of cached ROB data fetches for this algorithm in this run range", True)
        self.columns["retrievedDataSizeRate"]   = Column("Retrieved ROB Rate [kB/s]", "Average size of retrieved ROB data fetches for this algorithm in this run range") 


    def fillColumns(self, histName):
        slowCalls = self.getHistogram("Time_perCall").Integral(self.getHistogram("Time_perCall").FindBin(1000.), self.getHistogram("Time_perCall").GetNbinsX())

        self.columns["name"].addValue(histName)
        self.columns["events"].addValue(self.getHistogram("Chain_perEvent").GetEntries())
        self.columns["eventsWeighted"].addValue(self.getHistogram("Chain_perEvent").Integral())
        self.columns["time"].addValue(self.getHistogram("Time_perEvent").GetMean())
        self.columns["chainRate"].addValue(self.getHistogram("Chain_perEvent").Integral())
        self.columns['passFraction'].addValue(self.getHistogram("ChainPassed_perEvent").Integral())
        self.columns["callsSlow"].addValue(slowCalls)
        self.columns['totalTime'].addValue(self.getXWeightedIntegral("Time_perCall", isLog=True) * 1e-3)
        #self.columns['totalTimeFrac'] in postprocessing
        self.columns['totalUniqTime'].addValue(self.getXWeightedIntegral("UniqueTime_perCall", isLog=True) * 1e-3)
        #self.columns['totalUniqueTimeFrac'] in postprocessing
        self.columns["algPerEvent"].addValue(self.getHistogram("AlgCalls_perEvent").GetMean())
        self.columns["dataRate"].addValue(self.getXWeightedIntegral("Request_perEvent", isLog=False))
        self.columns["retrievedDataRate"].addValue(self.getXWeightedIntegral("NetworkRequest_perEvent", isLog=False))
        self.columns["cachedDataSizeRate"].addValue(self.getXWeightedIntegral("CachedROBSize_perEvent", isLog=False))
        self.columns["retrievedDataSizeRate"].addValue(self.getXWeightedIntegral("NetworkROBSize_perEvent", isLog=False))

        groups = ""
        for i in range (1, self.getHistogram("Group_perCall").GetNbinsX()):
            label = self.getHistogram("Group_perCall").GetXaxis().GetBinLabel(i)
            if not label: break

            groups += label + ", "

        self.columns['group'].addValue(groups)

    def postProcessing(self):
        totalTimeEntries = self.columns["totalTime"].content

        for entry in totalTimeEntries:
            self.columns["totalTimeFrac"].addValue(100 * entry / self.totalTime)

        totalUniqTimeEntries = self.columns["totalUniqTime"].content

        for entry in totalUniqTimeEntries:
            self.columns["totalUniqTimeFrac"].addValue(100 * entry / self.totalTime)

        passChainEntries = self.columns["passFraction"].content
        totalChains = self.columns["eventsWeighted"].content

        for i in range(0, len(passChainEntries)):
            passFrac = 0 if totalChains[i] == 0 else 100 * passChainEntries[i] / totalChains[i]
            self.columns["passFraction"].content[i] = passFrac