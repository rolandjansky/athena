#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TrigCostAnalysis.TableConstructorBase import TableConstructorBase, Column

'''
@file ROS_HLT_TableConstructor.py
@brief Contains TableConstructor classes per ROS_HLT table. Defines what
    should be saved in table and fills them
'''


class ROS_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing ROS_HLT table
    '''

    def __init__(self, tableObj, underflowThreshold, overflowThreshold):
        super(). __init__(tableObj, underflowThreshold, overflowThreshold) 
        self.expectedHistograms = ["Request_perEvent", 
                                   "NetworkRequest_perEvent", 
                                   "CachedROBSize_perEvent",
                                   "NetworkROBSize_perEvent",
                                   "Time_perEvent",
                                   "ROBStatus_perCall"]


    def defineColumns(self):
        self.columns['name']                = Column("Name", "ROS name")
        self.columns['events']              = Column("Raw Active Events", "Raw underlying statistics on the number of events in which this ROS was accessed.")
        self.columns['eventsWeighted']      = Column("Active Events", "How many events in which this sequence was executed.")
        self.columns['requestRate']         = Column("Data Requests Rate [Hz]", "Rate of ROS access requests each may contain many ROB reads.", True)
        self.columns['networkRequestRate']  = Column("Retrieved Data Requests Rate [Hz]", "Rate of ROS access requests that fetch data from the ROBs.", True)
        self.columns['retrievedSizeRate']   = Column("Retrieved ROB Data Rate [kB/s]", "Amount of data fetched from the ROBs in kB/s.", True)
        self.columns['cachedSizeRate']      = Column("Cached ROB Data Rate [kB/s]", "Amount of cached data requested from the ROBs in kB/s.", True)
        self.columns['time']                = Column("Time Per Event [ms]", "Average time for all requests and retrievals per event.")
        self.columns['robsUnclassified']    = Column("Unclassified ROBs Rate [Hz]", "Rate of ROB calls which were flagged unclassified.", True)
        self.columns['robsRetrieved']       = Column("Retrieved ROBs Rate [Hz]","Total rate of fetched ROB calls.", True)
        self.columns['robsHLTCached']       = Column("Cached HLT ROBs Rate [Hz]","Total rate of HLT cached ROB calls.", True)
        self.columns['robsDCMCached']       = Column("Cached DCM ROBs Rate [Hz]","Total rate of DCM cached ROB calls.", True)
        self.columns['robsIgnored']         = Column("Ignored ROBs Rate [Hz]", "Rate of ROB calls which were flagged as ignored.", True)
        self.columns['robsDisabled']        = Column("Disabled ROBs Rate [Hz]", "Rate of ROB calls which were flagged as disabled.", True)
        self.columns['robsNotOk']           = Column("Not OK ROBs Rate [Hz]", "Rate of ROB calls in which the is OK bit was false.", True)


    def fillColumns(self, histName):
        self.columns['name'].addValue(histName)
        self.columns['events'].addValue(self.getHistogram("Request_perEvent").GetEntries())
        self.columns['eventsWeighted'].addValue(self.getHistogram("Request_perEvent").Integral())
        self.columns['requestRate'].addValue(self.getXWeightedIntegral("Request_perEvent", isLog=False))
        self.columns['networkRequestRate'].addValue(self.getXWeightedIntegral("NetworkRequest_perEvent", isLog=False))
        self.columns['retrievedSizeRate'].addValue(self.getXWeightedIntegral("NetworkROBSize_perEvent", isLog=False))
        self.columns['cachedSizeRate'].addValue(self.getXWeightedIntegral("CachedROBSize_perEvent", isLog=False))
        self.columns['time'].addValue(self.getHistogram("Time_perEvent").GetMean())
        self.columns['robsUnclassified'].addValue(self.getHistogram("ROBStatus_perCall").GetBinContent(1))
        self.columns['robsRetrieved'].addValue(self.getHistogram("ROBStatus_perCall").GetBinContent(2))
        self.columns['robsHLTCached'].addValue(self.getHistogram("ROBStatus_perCall").GetBinContent(3))
        self.columns['robsDCMCached'].addValue(self.getHistogram("ROBStatus_perCall").GetBinContent(4))
        self.columns['robsIgnored'].addValue(self.getHistogram("ROBStatus_perCall").GetBinContent(5))
        self.columns['robsDisabled'].addValue(self.getHistogram("ROBStatus_perCall").GetBinContent(6))
        self.columns['robsNotOk'].addValue(self.getHistogram("ROBStatus_perCall").GetBinContent(7))