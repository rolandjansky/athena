#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file TableSpecifications.py
@brief Contains TableConstructor classes per each table. They define 
    what should be saved in tables and fill them
'''


from TrigCostAnalysis.TableConstructor import TableConstructorBase, Column


class ROS_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing ROS_HLT table
    '''

    def __init__(self, tableObj):
        super(). __init__(tableObj) 
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
        self.columns['robsUnclassified']    = Column("Unclassified ROBs/Event", "ROB calls which were flagged unclassified.")
        self.columns['robsRetrieved']       = Column("Retrieved ROBs/Event","Total number of fetched ROB calls.")
        self.columns['robsHLTCached']       = Column("Cached HLT ROBs/Event","Total number of HLT cached ROB calls.")
        self.columns['robsDCMCached']       = Column("Cached DCM ROBs/Event","Total number of DCM cached ROB calls.")
        self.columns['robsIgnored']         = Column("Ignored ROBs/Event", "ROB calls which were flagged as ignored.")
        self.columns['robsDisabled']        = Column("Disabled ROBs/Event", "ROB calls which were flagged as disabled.")
        self.columns['robsNotOk']           = Column("Not OK ROBs/Event", "ROB calls in which the is OK bit was false.")


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




class Global_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Global_HLT table
    '''
    def __init__(self, tableObj):
        super(). __init__(tableObj) 
        self.expectedHistograms = ["AlgTime_perEvent", 
                                   "AlgTime_perCall", 
                                   "AlgCalls_perEvent",
                                   "SteeringTime_perEvent",
                                   "LbLength"]


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


    def fillColumns(self, histName):
        lbLength = self.getHistogram("LbLength").GetBinContent(1)
        weightedEvents = self.getHistogram("SteeringTime_perEvent").Integral()
        weightedCalls = self.getXWeightedIntegral("AlgCalls_perEvent", isLog=False)

        self.columns['name'].addValue(histName)
        self.columns['lbLength'].addValue(lbLength)
        self.columns['events'].addValue(self.getHistogram("SteeringTime_perEvent").GetEntries())
        self.columns['eventsWeighted'].addValue(weightedEvents)
        self.columns['callsPerEvent'].addValue(self.getHistogram("AlgCalls_perEvent").GetMean())
        self.columns['eventRate'].addValue(weightedEvents / lbLength)
        self.columns['callRate'].addValue(weightedCalls / lbLength)
        self.columns['steeringTime'].addValue(self.getXWeightedIntegral("SteeringTime_perEvent", isLog=True) * 1e-3)
        self.columns['steeringTimePerEvent'].addValue(self.getHistogram("SteeringTime_perEvent").GetMean())
        self.columns['totalTimeSec'].addValue(self.getXWeightedIntegral("AlgTime_perEvent", isLog=True) * 1e-3)
        self.columns['timePerCall'].addValue(self.getHistogram("AlgTime_perCall").GetMean())
        self.columns['timePerEvent'].addValue(self.getHistogram("AlgTime_perEvent").GetMean())


class Algorithm_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Algorithm_HLT table
    '''
    def __init__(self, tableObj):
        super(). __init__(tableObj) 
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


    def postProcessing(self):

        totalTimeEntries = self.columns["totalTimeSec"].content
        totalTime = sum(totalTimeEntries)

        for entry in totalTimeEntries:
            self.columns["totalTimePerc"].addValue(100 * entry / totalTime)


''' @brief Class representing Algorithm_Class_HLT table

It should have the same behaviour as Algorithm_HLT_TableConstructor
    but with own data (own ROOT directory)
'''
Algorithm_Class_HLT_TableConstructor = Algorithm_HLT_TableConstructor


class Thread_Occupancy_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Thread_Occupancy_HLT table
    '''
    def __init__(self, tableObj):
        super(). __init__(tableObj) 
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


class Chain_HLT_TableConstructor(TableConstructorBase):
    ''' @brief Class representing Chain_HLT table
    '''
    def __init__(self, tableObj):
        super(). __init__(tableObj) 
        self.totalTime = 0 # Total time of algoirthms execution
        self.expectedHistograms = ["Group_perCall", 
                                   "Chain_perEvent", 
                                   "AlgCalls_perEvent",
                                   "Time_perEvent",
                                   "Time_perCall",
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
        self.columns['time'] = Column("Time Per Event [ms]", "Average time for all requests and retrievals per event")
        self.columns['chainRate'] = Column("Execute Rate [Hz]", "Number of chain executions normalised to the wall time for this run range", True)
        self.columns['passFraction'] = Column("Pass Fraction [%]", "What percentage of events pass events are kept")
        self.columns["callsSlow"] = Column("Calls > 1000 ms", "Number of algorithm executions which were particularly slow")
        self.columns['totalTime'] = Column("Total Chain Time [s]", "Total time used by this chain for this run range")
        self.columns['totalTimeFrac'] = Column("Total Chain Time [%]", "Total chain time as a percentage of the total time of all chains in this run range")
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

        passChainEntries = self.columns["passFraction"].content
        totalChains = self.columns["eventsWeighted"].content

        for i in range(0, len(passChainEntries)):
            passFrac = 0 if totalChains[i] == 0 else 100 * passChainEntries[i] / totalChains[i]
            self.columns["passFraction"].content[i] = passFrac
