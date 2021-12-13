#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TrigCostAnalysis.TableConstructorBase import TableConstructorBase, Column

'''
@file Global_HLT_TableConstructor.py
@brief Contains TableConstructor classes per Global_HLT table. Defines what
    should be saved in table and fills them
'''


class Global_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Global_HLT table
    '''
    def __init__(self, tableObj, underflowThreshold, overflowThreshold):
        super(). __init__(tableObj, underflowThreshold, overflowThreshold) 
        self.expectedHistograms = ["AlgTime_perEvent", 
                                   "AlgTime_perCall", 
                                   "AlgCalls_perEvent",
                                   "SteeringTime_perEvent",
                                   "LbLength"]

        self.lbLength = 0


    def defineColumns(self):
        self.columns['name']                 = Column("Name", "Algorithms name")
        self.columns['lbLength']             = Column("Lumi Block Length [s]", "Length of this luminosity block or blocks")
        self.columns['events']               = Column("Raw Active Events", "Raw underlying statistics on number of events processed")
        self.columns['eventsWeighted']       = Column("Active Events", "Total weighted number of events processed")
        self.columns['callsPerEvent']        = Column("Alg Calls/Event", "Mean number of alg calls")
        self.columns['eventRate']            = Column("Event Rate [Hz]", "LB normalised rate of events in the HLT")
        self.columns['callRate']             = Column("Alg Call Rate [Hz]", "LB normalised rate of alg calls in events in the HLT")
        self.columns['steeringTime']         = Column("Total Steering Time [s]", "Total weighted integrated steering time")
        self.columns['steeringTimePerEvent'] = Column("Steering Time/Event [ms]", "Mean integrated steering time per event")
        self.columns['totalTimeSec']         = Column("Alg Total Time [s]", "Total weighted integrated walltime of all algs")
        self.columns['timePerCall']          = Column("Alg Time/Call [ms]", "Mean weighted alg time normalised to all alg calls")
        self.columns['timePerEvent']         = Column("Alg Time/Event [ms]", "Mean weighted alg time normalised to all events")
        self.columns['nPU']                  = Column("Number of PU", "Estimated number of needed CPU with 100 kHz input rate based on algorithm time per event")
        self.columns['nPUErr']               = Column("Error on number of CPUs", "Error on number of CPUs based on Algorithm tome per event error")


    def fillColumns(self, histName):
        rateDenominator = self.lbLength if histName == "All" else self.getHistogram("LbLength").GetBinContent(1)
        weightedEvents = self.getHistogram("SteeringTime_perEvent").Integral()
        weightedCalls = self.getXWeightedIntegral("AlgCalls_perEvent", isLog=False)

        self.columns['name'].addValue(histName)
        self.columns['lbLength'].addValue(self.getHistogram("LbLength").GetBinContent(1))
        self.columns['events'].addValue(self.getHistogram("SteeringTime_perEvent").GetEntries())
        self.columns['eventsWeighted'].addValue(weightedEvents)
        self.columns['callsPerEvent'].addValue(self.getHistogram("AlgCalls_perEvent").GetMean())
        self.columns['eventRate'].addValue(weightedEvents / rateDenominator)
        self.columns['callRate'].addValue(weightedCalls / rateDenominator)
        self.columns['steeringTime'].addValue(self.getXWeightedIntegral("SteeringTime_perEvent", isLog=True) * 1e-3)
        self.columns['steeringTimePerEvent'].addValue(self.getHistogram("SteeringTime_perEvent").GetMean())
        self.columns['totalTimeSec'].addValue(self.getXWeightedIntegral("AlgTime_perEvent", isLog=True) * 1e-3)
        self.columns['timePerCall'].addValue(self.getHistogram("AlgTime_perCall").GetMean())
        self.columns['timePerEvent'].addValue(self.getHistogram("AlgTime_perEvent").GetMean())
        self.columns['nPU'].addValue(self.getHistogram("AlgTime_perEvent").GetMean() * 100) # 100 kHz L1 input rate
        self.columns['nPUErr'].addValue(self.getHistogram("AlgTime_perEvent").GetMeanError() * 100)

