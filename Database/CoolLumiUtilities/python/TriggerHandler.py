#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TriggerHandler
#
# Eric Torrence - October 2011
#
# Contents:
# TriggerL1Data - L1 data object
# TriggerHandler - utility tool to find trigger-based information from COOL
#

import sys
import time
import calendar

from PyCool import cool
from CoolLumiUtilities.CoolDataReader import CoolDataReader

#
# Data object to hold L1 trigger count information
#
class TriggerL1Data:

    def __init__(self):

        self.clear()
        
    def clear(self):
        # Dictionaries keyed by L1 trigger name
        self.TBP = dict()
        self.TAP = dict()
        self.TAV = dict()

        self.startTime = 0.
        self.endtime = 0.
        self.runlb = 0

        # Duration of this lumi block (in seconds) 
        self.dtime = 0.

# Main LumiHandler class for retrieving luminosity data from DB

class TriggerHandler:

    def __init__(self):

        # Database parameters
        self.menuReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LVL1/Menu')
        self.countsReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LVL1COUNTERS')
        self.lbtimeReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBTIME')
        self.lblbReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLB')

        self.verbose = False
        #self.verbose = True

        # Dict of all TrigL1Data objects for the given time interval (keyed by RunLB IOV)
        self.trigL1Dict = dict()

        self.allL1Triggers = False
        
        # List of all trigger items to read
        self.trigList = ['L1_MBTS_2', 'L1_EM30']

        # Dictionary of trigger channel number keyed by trigger name
        self.trigChan = dict()
        self.chanTrig = dict() # reverse order

        # Store the lumi block times
        self.lblbDict = dict()
        
    # Clear all data
    def clear(self):

        # Clear trigger dict
        self.trigL1Dict.clear()

    # Find trigger information in iovrange by time
    def loadData(self, startIOV, endIOV):

        self.clear()

        # Runlist holds specific runs in this time range
        self.runlist = []
        
        if self.verbose: print 'Searching for trigger information for max IOVRange', timeString(startIOV), timeString(endIOV)
        
        # Load the run based information as we fundamentally need to do this by run number
        
        # Prepare the lbtime reader
        self.lbtimeReader.setIOVRange(startIOV, endIOV)
        self.lbtimeReader.readData()

        for obj in self.lbtimeReader.data:

            runnum = int(obj.payload()['Run'])

            if not runnum in self.runlist:
                self.runlist.append(runnum)
                
        # Loop over each run, getting the trigger counts/Lumi
        # Must do this by run, as trigger menu can change
        # Here we are storing this in a separate list
        for runnum in self.runlist:
            self.loadDataByRun(runnum, clear=False)


    # Find trigger information by run
    def loadDataByRun(self, runnum, clear=True):

        if self.verbose:
            print 'TriggerHandler.loadDataByRun(%d) called' % runnum
            
        if clear:
            self.clear()

        # Figure out the channel mappings for the L1 trigger items
        self.loadTrigChannels(runnum)

        # Get the LB durations
        self.loadLBLBData(runnum)
        
        # Third, get the trigger counts
        self.loadTrigCounts(runnum)

    # Read LBLB data
    def loadLBLBData(self, runnum):

        if self.verbose:
            print 'TriggerHandler.loadLBLBData(%d) called' % runnum

        self.lblbDict.clear()
        self.lblbReader.setIOVRangeFromRun(runnum)
        self.lblbReader.readData()

        for obj in self.lblbReader.data:
            self.lblbDict[obj.since()] = (obj.payload()['StartTime'], obj.payload()['EndTime'])
            
    # Read trigger channel mappings
    # Fills self.trigChan based on values in self.trigList
    def loadTrigChannels(self, runnum):    

        if self.verbose:
            print 'TriggerHandler.loadTrigChannels(%d) called' % runnum

        # Trigger channels keyed by name            
        self.trigChan = dict()

        # Trigger name keyed by channel
        self.chanTrig = dict()

        for trig in self.trigList:
            self.trigChan[trig] = -1

        self.menuReader.setIOVRangeFromRun(runnum)
        self.menuReader.readData()

        for obj in self.menuReader.data:
            
            if self.verbose or True: print int(obj.channelId()), obj.payload()['ItemName']

            trigName = obj.payload()['ItemName']
            trigChan = int(obj.channelId())

            if self.allL1Triggers or self.trigList.count(trigName) > 0:
                self.trigChan[trigName] = trigChan
                self.chanTrig[trigChan] = trigName
                
        for trig in self.trigList:
            if self.trigChan[trig] == -1:
                print "Couldn't find", trig, "in run", str(runnum)

        if self.verbose:
            for (trig, chan) in self.trigChan.iteritems():         
                print 'Found', trig, 'in channel', chan

    # Load all trigger counts for the given run
    # Fills counts for all triggers with channels found in self.trigChan
    def loadTrigCounts(self, runnum):

        if self.verbose:
            print 'TriggerHandler.loadTrigCounts(%d) called' % runnum

        self.countsReader.setIOVRangeFromRun(runnum)

        # Build channel list
        chanList = self.trigChan.values()
        chanList.sort()

        nMaxChan   = 50
        nChanBlock = 0
        chanBlock  = []
        nChannels  = 0

        # Skip any trigger we didn't find
        tmpList = []
        for chan in chanList:
            if chan < 0: continue
            tmpList.append( chan )
        chanList = tmpList

        if self.verbose:
            print 'breaking up', len(chanList), 'into', nMaxChan, 'for run', runnum 

        # There is a 50 item limit somehow hardcoded into browseObjects.
        # Use this code from Elliot to get around the limitation.
        
        # Break up list of indices into blocks:
        for x in range(0, len(chanList), nMaxChan):
            top = min([x+nMaxChan, len(chanList)])

            if self.verbose:
                print 'Initializing block [%d] from %d to %d' % (nChanBlock, x, top)
                
            chanBlock.append( chanList[x:top] )
            nChanBlock += 1

        for x in range(nChanBlock):
            if self.verbose: print 'Channel Selector', chanBlock[x]
            self.countsReader.setChannel(chanBlock[x])
            self.countsReader.readData()

            for obj in self.countsReader.data:

                since = obj.since()
                until = obj.until()
                if self.verbose:
                    print runLBString(since), runLBString(until), obj.channelId(), obj.payload()['BeforePrescale'], obj.payload()['AfterPrescale'], obj.payload()['L1Accept']

                # use the string as the dictionary key
                ss = since
                chan = int(obj.channelId())
                trig = self.chanTrig.get(chan, "")
                if len(trig) == 0:
                    print 'TriggerHandler.loadTrigCounts(%d) - found unknown channel %d in %s!' % (runnum, chan, runLBString(ss))
                    continue
                
                if ss not in self.trigL1Dict:
                    self.trigL1Dict[ss] = TriggerL1Data()
                    self.trigL1Dict[ss].runlb = obj.since()
                    self.trigL1Dict[ss].startTime = self.lblbDict.get(ss, (0., 0.))[0]
                    self.trigL1Dict[ss].endTime = self.lblbDict.get(ss, (0., 0.))[1]
                    self.trigL1Dict[ss].dtime = (self.trigL1Dict[ss].endTime - self.trigL1Dict[ss].startTime)/1.E9

                self.trigL1Dict[ss].TBP[trig] = obj.payload()['BeforePrescale']
                self.trigL1Dict[ss].TAP[trig] = obj.payload()['AfterPrescale']
                self.trigL1Dict[ss].TAV[trig] = obj.payload()['L1Accept']
                
def timeVal(val):
    "Convert argument to time in seconds, treating as a literal or date"
    try:
        a=int(val)
        return a
    except ValueError:
        try:
            ts=time.strptime(val,'%Y-%m-%d:%H:%M:%S/%Z')
            return int(calendar.timegm(ts))*1000000000L
        
        # Try again with UTC attached
        except ValueError:
            try:
                ts=time.strptime(val+'/UTC', '%Y-%m-%d:%H:%M:%S/%Z')
                return int(calendar.timegm(ts))*1000000000L
            
            except ValueError,e:
                print "ERROR in time specification:",val,"- use e.g. 2007-05-25:14:01:00/UTC"
                sys.exit(-1)
                
def timeString(iovkey):
    "Convert the IOVtime (63 bit) to a string representing timestamp"
    if (iovkey==cool.ValidityKeyMin):
        return "ValidityKeyMin"
    elif (iovkey==cool.ValidityKeyMax):
        return "ValidityKeyMax"
    else:
        # Round to avoid bias if microseconds exist
        stime=int(round(iovkey/1000000000L))
        return time.strftime('%Y-%m-%d:%H:%M:%S/UTC', time.gmtime(stime))
    
def runLBString(iovkey):
    "Convert the IOVtime (63 bit) to a string representing timestamp"
    if (iovkey==cool.ValidityKeyMin):
        return "ValidityKeyMin"
    elif (iovkey==cool.ValidityKeyMax):
        return "ValidityKeyMax"
    else:
        # Round to avoid bias if microseconds exist
        run = iovkey >> 32
        lb = iovkey & 0xFFFFFFFF
        return "%d/%d" % (run, lb)
                                    
                
# Executed from the command line, for testing only
if __name__ == '__main__':
    th = TriggerHandler()
    th.verbose = True
    th.allL1Triggers = True    
    tstart = '2011-10-16:06:00:00'
    tend   = '2011-10-17:04:00:00'

    th.loadData(timeVal(tstart), timeVal(tend))
