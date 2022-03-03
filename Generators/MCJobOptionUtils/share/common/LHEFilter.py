# Joboption fragment to filter on lhe files
# The base class BaseLHEFilter holds the important functions - it doesn't do any filtering
# The actual filters should derive from this base class, and have their pass_filter function defined together with relevant parameters
# The LHEFilters class holds a list of filters which are required to all be satisfied (i.e. they are ANDed) - it makes the actual filtering, creates the output lhe file and make the necessary prints
# Author: Timoth\'ee Theveneaux-Pelzer (modeled after code by Benjamin Nachman found in ATLMCPROD-4564, modeled after code from Fabian Wilk)
#
# Example of usage in a Joboption:
#
#include('MCJobOptionUtils/LHEFilter.py')
#include('MCJobOptionUtils/LHEFilter_NLeptons.py')
#include('MCJobOptionUtils/LHEFilter_LeadingTopPt.py')
#
#nleptonFilter = LHEFilter_NLeptons()
#nleptonFilter.NumLeptons = 1
#nleptonFilter.Ptcut = 0.
#lheFilters.addFilter(nleptonFilter)
#
#leadTopPt = LHEFilter_LeadingTopPt()
#leadTopPt.Ptcut = 300.
#lheFilters.addFilter(leadTopPt)
#
#lheFilters.inputLHE('myInputLHE.tar.gz') # only if the input file is not taken from runArgs.inputGeneratorFile
#
#lheFilters.run_filters()

from AthenaCommon import Logging
athMsgLog = Logging.logging.getLogger('MCJobOptionUtils/LHEFilter.py')

import os
import re
import sys
import glob
import tarfile

# This code is stored in gencontrol
include('MCJobOptionUtils/LHETools.py')
# This code is stored in gencontrol
include("EvgenProdTools/merge_lhe_files.py")

# base class for LHE filters
# this base class keeps all events, so one need to use derived classes where the pass_filter function is overriden
class BaseLHEFilter(object):
    name = None # name of the filter, usefull for logfiles
    inverted = None # set to True to invert the filter

    # dummy selection by default
    # this function should be extended in daughter classes
    def pass_filter(self,Evt):
            return True

# LHEFilters class, to combine several filters
class LHEFilters(object):
    def __init__(self):
        self.inputLHE = '' # if left empty, runArgs.inputGeneratorFile will be used
        self.outputLHE = 'filtered_lhef._0.events' # runArgs.inputGeneratorFile will be updated with it if self.inputLHE is empty

        self.total_events = 0
        self.total_weights = 0.
        self.accepted_events = 0
        self.accepted_weights = 0.

        self.buggy_events = 0 # number of buggy events seen in the input lhe file

        self.filters = [] # list of LHEFilters - it should be a list of objects deriving from the BaseLHEFilter class

    def run_filters(self):
        self.initialise()
        self.copy_header()
        while self.process_event():
            pass
        self.copy_footer()
        self.finalise()

    def addFilter(self,filt):
        # checking the list of lhe filters
        try:
            if not isinstance(filt,BaseLHEFilter):
                athMsgLog.error("Provided LHEFilter doesn't derive from BaseLHEFilter class")
                raise RuntimeError
            self.filters.append(filt)
            athMsgLog.info("Using LHEFilter {}{}".format("not " if filt.inverted else "",filt.name))
        except:
            athMsgLog.error("Impossible to retrieve the list of LHEFilters")
            raise RuntimeError

    def initialise(self):
        # if user provided an inputLHE, use it as input, otherwise use runArgs.inputGeneratorFile
        inFile = runArgs.inputGeneratorFile if self.inputLHE == '' else self.inputLHE
        if self.inputLHE != '':
            athMsgLog.info("Using provided input lhe file(s): '{}'".format(inFile))
        else:
            athMsgLog.info("No provided input lhe file(s) - using runArgs.inputGeneratorFile instead: '{}'".format(inFile))
        # opening the input lhe file, and merge them if there are more than one of them
        try:
            # this portion of code was adapted from MCJobOptionUtils/Herwig7_LHEF.py
            if ".tar.gz" in inFile or ".tgz" in inFile:
                athMsgLog.info("input lhe File '{}' is compressed - will look for uncompressed LHE file".format(inFile))
                tarredLhe_files = inFile.split(',') # if there are several input files, their names are separated by comas
                lhe_files = []
                for compressedFile in tarredLhe_files:
                    lhe_files.append(tarfile.open(compressedFile).getnames()[0]) # retrieve the name of the compressed lhe file
                athMsgLog.info("Number of lhe files: {}".format(len(lhe_files)))
                if len(lhe_files) == 0:
                    athMsgLog.error("Could not find uncompressed LHE file")
                    raise RuntimeError
                for lhe_file in lhe_files:
                    athMsgLog.info("Checking for duplicates in "+lhe_file)
                    FindDuplicates(inFileName=lhe_file)
                if len(lhe_files) > 1:
                    athMsgLog.info("Found more than one uncompressed LHE file: {}".format(lhe_files))
                    # skeleton.GENtoEVGEN splits the file name on "._" (in rel. 20.7.9.9.6,MCProd,
                    # so insert this in the merged file name - to make it run also for this release)
                    my_lhe_file = "merged_lhef._0.events"
                    merge_lhe_files(lhe_files, my_lhe_file )
                    lhe_files[0] = my_lhe_file
                athMsgLog.info("Using uncompressed LHE file '{}' as input of LHEFilter".format(lhe_files[0]))
                self.fIn = open(lhe_files[0], 'r')
            else:
                athMsgLog.info("Checking for duplicates in "+lhe_files[0])
                FindDuplicates(inFileName=lhe_files[0])
                self.fIn = open(inFile, 'r')
        except:
            athMsgLog.error("Impossible to use input lhe file {}".format(inFile))
            raise RuntimeError
        
        # opening the output file
        try:
            self.fOut = open(self.outputLHE, 'w')
        except:
            athMsgLog.error("Impossible to open output lhe file {}".format(self.outputLHE))
            raise RuntimeError

    def finalise(self):
        # closing the opened files
        self.fIn.close()
        self.fOut.close()
        
        # removing merged lhe file if it exists - it's large and we only care about the filtered one
        if (self.fIn.name == "merged_lhef._0.events"):
            os.remove(self.fIn.name)

        # update number of events in header of output lhe file
        self.update_Nevents()
        
        # build filter sequence for printing
        sequence = ''
        for filt in self.filters:
            if len(sequence) != 0:
                sequence += ' and '
            if filt.inverted:
                sequence += 'not '
            sequence += filt.name

        # some important prints
        athMsgLog.info("LHEFilter Names = {}".format(sequence))
        athMsgLog.info("LHEFilter Unweighted Efficiency = {} [{} / {}]".format(float(self.accepted_events) / self.total_events, self.accepted_events, self.total_events))
        athMsgLog.info("LHEFilter Weighted Efficiency = {} [{} / {}]".format(self.accepted_weights / self.total_weights, self.accepted_weights, self.total_weights))
        athMsgLog.info("Number of buggy events seen (and skipped) by the LHEFilter / total: {} / {}".format(self.buggy_events, self.buggy_events+self.total_events))

        # for the meta-data
        print "MetaData: %s = %s" % ("lheFilterNames", sequence)
        print "MetaData: %s = %e" % ("LHEFiltEff", self.accepted_weights / self.total_weights)

        # finally, use the filtered lhe file as inputGeneratorFile, but only if self.inputLHE was empty
        if (self.inputLHE == ''):
            runArgs.inputGeneratorFile = self.outputLHE

    # copying header of the possibly merged lhe file
    def copy_header(self):
        rx = re.compile(r'<event>\n')

        last_pos = self.fIn.tell()
        line = self.fIn.readline()
        matched = False

        while line:
            if rx.match(line):
                self.fIn.seek(last_pos)
                matched = True
                break

            else:
                self.fOut.write(line)
                last_pos = self.fIn.tell()
                line = self.fIn.readline()

        if not matched:
            athMsgLog.error("Failure to find header")
            raise RuntimeError

    # this function is called for each event
    # it returns False if we are at the last event, and True otherwise
    def process_event(self):
        rx_start = re.compile('<event>')
        rx_end = re.compile('</event>')

        last_pos = self.fIn.tell()
        line = self.fIn.readline()

        if not rx_start.match(line):
            self.fIn.seek(last_pos)
            return False

        # Acquire all content lines.
        line = self.fIn.readline()

        data = []
        matched = False

        while line:
            if rx_end.match(line):
                matched = True
                break
            else:
                data.append(line)
                last_pos = self.fIn.tell()
                line = self.fIn.readline()

        if not matched:
            athMsgLog.warning("Failure to find event object: can't find </event> markup")
            athMsgLog.warning("Skipping buggy event from input lhe file, and stopping the processing (since it's probably the last event).")
            athMsgLog.warning(str(self.total_events)+" valid events had been read so far")
            self.buggy_events += 1
            return False # we ignore this buggy event, and don't even include it in the total eventw count

        # building the lhe event object
        event = Event.from_lines(data)
        if event.is_buggy():
            athMsgLog.warning("Skipping buggy event from input lhe file")
            athMsgLog.warning(str(self.total_events)+" valid events had been read so far")
            self.buggy_events += 1
            return True # we ignore this buggy event, and don't even include it in the total eventw count

        # increment counters
        self.total_events += 1
        self.total_weights += event.info.weight

        if not self.pass_filters(Evt=event):
            return True # we won't keep this event

        # increment counters
        self.accepted_events += 1
        self.accepted_weights += event.info.weight

        # writing the kept event in the output
        self.fOut.write('<event>\n')
        self.fOut.write(event.to_string())
        self.fOut.write('</event>\n')

        return True

    # footer of the lhe file
    def copy_footer(self):
        rx = re.compile('</LesHouchesEvents>\n')

        last_pos = self.fIn.tell()
        line = self.fIn.readline()

        if rx.match(line):
            while line:
                self.fOut.write(line)
                line = self.fIn.readline()
        else:
            athMsgLog.error("Failure to find footer")
            raise RuntimeError

    # update number of events in filtered lhe file
    def update_Nevents(self):
        inHeader = True
        f = open(self.outputLHE,'r')
        temp_file = open('temporaryLHE.events','w')
        for line in f:
            if("<event" in line and inHeader):
                inHeader = False
            if inHeader:
                # Format for storing number of events different in MG and Powheg
                if("nevents" in line): # MG5 format is "n = nevents"
                    tmp = line.split("=")
                    line = line.replace(tmp[0],str(self.accepted_events))
                elif("numevts" in line): # Powheg format is numevts n
                    tmp = line.split(" ")
                    line = line.replace(tmp[1],str(self.accepted_events))
            temp_file.write(line)
        f.close()
        temp_file.close()
        # remove old output file and replace it with new one
        os.remove(self.outputLHE)
        os.rename('temporaryLHE.events',self.outputLHE)
        # for consistency, update self.fOut
        self.fOut = open(self.outputLHE,'r')
        self.fOut.close()

    # returns true if the event passes all filters of the sequence
    def pass_filters(self,Evt):
        # loop on all filters in the list; if one of them fails, return False
        for filt in self.filters:
            # taking into account the case where the filter has been inverted
            event_pass = filt.pass_filter(Evt)
            event_actuallypass = event_pass if not filt.inverted else not event_pass
            if not event_actuallypass:
                return False
        return True

# create one instance - it has to be the single one
lheFilters = LHEFilters()
