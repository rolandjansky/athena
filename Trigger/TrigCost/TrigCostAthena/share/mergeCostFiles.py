#!/usr/bin/env python

# @file: 
# @purpose: Merge L2 and EF TrigCost and TrigRate files
# @author: Stephen Sekula <sekula@cern.ch>

import os
import sys
from datetime import datetime
from operator import itemgetter, attrgetter
import ROOT, PyCintex
from optparse import OptionParser
#from ROOT import *

# Create a class that can hold identifiers for a uniqye
# event in a TTree holding TrigMonEvent objects.
# Allow this object to know whether it is present
# in the L2 file, the EF file, or both.
#-------------------------------------------------------------------------------
def parse_options() :
    parser = OptionParser(description=('Submit trigger jobs to a '
	                                             'batch system'      ))

    parser.add_option('--inputFileL2', dest='inputFileNameL2', default=None, help='The L2 Cost file')
    parser.add_option('--inputFileEF', dest='inputFileNameEF', default=None, help='The EF Cost file')
    parser.add_option('--outputFile', dest='outputFileName', default=None, help='Your output file')
    parser.add_option('--mergingType', dest='mergingType', default=4, type="int", help='If this is 2 only TrigMonRob and RoI are merged, if 3 only TrigMonTE are merged,  4 merges all of those, 1 nothing of those.')

    return parser

class UniqueEvent:
    """A unique event defined by second and nanosecond."""

    def __init__(self, sec, nsec):
        """Initialize this event using second and nanosecond."""
        self.sec = sec
        self.nsec = nsec
        self.l2index = -1
        self.efindex = -1
           
           
    def l2index(self, index):
        """set the ordinal location of this event in the L2 File."""
        self.l2index = index

    def efindex(self, index):
        """set the ordinal location of this event in the EF File."""
        self.efindex = index

    def __eq__(self, other):
        """test for equality of two events"""
        if self.sec == other.sec and self.nsec == other.nsec:
            return True
        else:
            return False


if __name__ == "__main__":


    # parse command line arguments.
    # the first two arguments are the input file; the last is the output file
    p = parse_options()
    (options, args) = p.parse_args()

    if options.inputFileNameL2 == "":
        print "***ERROR*** You must give an L2 input file"
        sys.exit()
    if options.inputFileNameEF == "":
        print "***ERROR*** You must give an EF input file"
        sys.exit()
    if options.outputFileName == "":
        print "***ERROR*** You must give an ouput file"
        sys.exit()

    if options.outputFileName == options.inputFileNameL2 or options.outputFileName == options.inputFileNameEF:
        print "***ERROR*** You cannot name the merged file to be the same as either input file!"
        sys.exit()

    # Load necessary external libraries for handling the special objects
    ROOT.gSystem.Load('libTrigMonitoringEvent')
    ROOT.gSystem.Load('libTrigMonitoringEventDict')
    ROOT.gSystem.Load('libTrigCostMonitor')
    ROOT.gSystem.Load('libTrigCostMonitorDict')

    inputFileL2 = ROOT.TFile(options.inputFileNameL2)
    inputFileEF = ROOT.TFile(options.inputFileNameEF)
    outputFile  = ROOT.TFile(options.outputFileName, "recreate")


    # now loop over the contents of inputFileL2's event tree
    configTree = inputFileL2.Get("config")
    eventTreeL2 = inputFileL2.Get("event")
    eventTreeEF = inputFileEF.Get("event")




    NumberOfEntriesL2 = eventTreeL2.GetEntries()
    NumberOfEntriesEF = eventTreeEF.GetEntries()
    print "There are ", NumberOfEntriesL2, " entries in the \"event\" TTree in ", options.inputFileNameL2
    print "There are ", NumberOfEntriesEF, " entries in the \"event\" TTree in ", options.inputFileNameEF

    # create an array of unique events
    eventList = []

    # Get the list of all Second,Nanosecond pairs from these files.
    # Then determine which pairs are in common in the files, and which
    # are unique. Merge the common and store the unique, all in the
    # correct order.

    # L2 file first
    print "getting L2 tree"
    for anEntry in range(NumberOfEntriesL2):
        NumberOfBytes = eventTreeL2.GetEntry(anEntry)
        if NumberOfBytes <= 0:
            continue
        
        eventObjectL2 = eventTreeL2.data

        anEvent = UniqueEvent(eventObjectL2.getSec(), eventObjectL2.getNanoSec())
        anEvent.l2index = anEntry

        eventList.append(anEvent)
        
    #EF file second
    print "getting EF tree"	
    for anEntry in range(NumberOfEntriesEF):
        NumberOfBytes = eventTreeEF.GetEntry(anEntry)
        if NumberOfBytes <= 0:
            continue

        eventObjectEF = eventTreeEF.data

        anEvent = UniqueEvent(eventObjectEF.getSec(), eventObjectEF.getNanoSec())

        # search the eventList and see if this one is already in there
        # if not, put it at the end of the eventList. We'll sort the
        # eventList afterward
        NumberOfExistingEvents = len(eventList)
        indexExistingEvent = 0
        foundEvent = False
        while indexExistingEvent < NumberOfExistingEvents:
            if anEvent == eventList[indexExistingEvent]:
                eventList[indexExistingEvent].efindex = anEntry
                foundEvent = True
                break
            indexExistingEvent = indexExistingEvent + 1

        if foundEvent == False:
            eventList.append(anEvent)
        

    # sort the list of unique events by second and then nanosecond
    finalEventList = sorted(eventList, key=attrgetter('sec', 'nsec'))
    print "got final event list"
    # now proceed through thr final event list. For each event
    # load the corresponding ordinal events from the TTrees
    # merge any TrigMonEvent objects in common and write to a new
    # tree, which will be stored in the output file.

    outputFile.cd()
    # clone and copy the config tree
    configTreeCopy = configTree.CloneTree()
    configTreeCopy.Write()
    print "config tree copied"   
    MergedEventTree = eventTreeL2.CloneTree(0)
    
    mergedEventObject = ROOT.TrigMonEvent()

    print "here"
    for anUniqueEvent in finalEventList:
        print "here1"
        if anUniqueEvent.l2index != -1 and anUniqueEvent.efindex != -1:
            print "here2" 
            # need to merge the TrigMonEvent objects
            #print "Merging event (",anUniqueEvent.sec,",",anUniqueEvent.nsec,") from the L2 and EF files"
            eventTreeL2.GetEntry(anUniqueEvent.l2index)
            eventTreeEF.GetEntry(anUniqueEvent.efindex)


            eventObjectL2 = eventTreeL2.data
            eventObjectEF = eventTreeEF.data

            # copy the L2 eventObject
            mergedEventObject = eventObjectL2
            # merge the EF event object with it
            print options.mergingType
            msg = ROOT.Trig.MergeEvent(mergedEventObject, eventObjectEF, options.mergingType)
#            print "The output of ROOT.Trig.MergeEvent() is:"
#            print msg
        elif anUniqueEvent.l2index != -1:
            # this event is unique to the L2 file - copy it
            #print "Copying event (",anUniqueEvent.sec,",",anUniqueEvent.nsec,") from the L2 file"
            eventTreeL2.GetEntry(anUniqueEvent.l2index)
            eventObjectL2 = eventTreeL2.data
            mergedEventObject = eventObjectL2
        elif anUniqueEvent.efindex != -1:
            # this event is unique to the EF file - copy it
            #print "Copying event (",anUniqueEvent.sec,",",anUniqueEvent.nsec,") from the EF file"
            eventTreeEF.GetEntry(anUniqueEvent.l2index)
            eventObjectEF = eventTreeEF.data
            mergedEventObject = eventObjectEF
            
        MergedEventTree.SetBranchAddress("data", mergedEventObject)
        MergedEventTree.Fill()


    print "Entries in copied config tree: ",configTreeCopy.GetEntries()
    print "Entries in merged event tree: ",MergedEventTree.GetEntries()


    MergedEventTree.Write()
    outputFile.Close()
    

        
        
        
    
    

    
    
    
        
 
