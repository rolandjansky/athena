#!/usr/bin/env python
#
# RunLumiTime.py
#
# Simple utility script to take a range of run numbers and output a text list of times associated
# with the luminosity blocks in those runs.
#
# This is also a useful example of how to use the CoolLumiUtilities tools
#
import os
import sys
import glob
import array
import shutil
import subprocess

from optparse import OptionParser

# Get our global DB handler object
from CoolLumiUtilities.CoolDataReader import CoolDataReader
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler

class RunLumiTime:

    def __init__(self):

        # Control output level
        self.verbose = False

        # Online luminosity database
        self.onlLumiDB = 'COOLONL_TRIGGER/COMP200'
        
        # Convert LB -> time
        self.onlLBLBFolder = '/TRIGGER/LUMI/LBLB'

        # List of (integer) run numbers specified on the command line
        self.runList = []

        # Instantiate the LumiDBHandler, so we can cleanup all COOL connections in the destructor
        self.dbHandler = LumiDBHandler()

        # Output file (default stdout)
        self.outFile = None
        
    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Open outfile if desired
        if self.outFile != None:
            f = open(self.outFile, 'w')
            
        # Get our COOL folder
        lblb = CoolDataReader(self.onlLumiDB, self.onlLBLBFolder)
        
        # Load data for each run specified
        for run in self.runList:

            lblb.setIOVRangeFromRun(run)
            if not lblb.readData():
                print 'RunLumiTime - No LBLB data found for run %d!' % run
                continue

            for obj in lblb.data:
                # IOV is equal to (Run << 32) + LB number.
                run = obj.since() >> 32
                lb = obj.since() & 0xFFFFFFFF
                # Time is UTC nanoseconds
                startTime = obj.payload()['StartTime']
                endTime = obj.payload()['EndTime']

                # Write this out as seconds
                outstr = "%d %d %f %f" % (run, lb, float(startTime)/1E9, float(endTime)/1E9)
                if self.outFile != None:
                    f.write(outstr+'\n')
                else:
                    print outstr

    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-r", "--run",
                          dest="runlist", metavar="RUN",
                          help="update specific run, or comma separated list")

        parser.add_option('-o', '--output',
                          dest='outfile', metavar = "FILE", default=self.outFile,
                          help="write results to output file")
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.outFile = options.outfile
        
        # Parse run list
        if options.runlist != None:

            # Clear the old one
            self.runList = []
            
            # Can be comma-separated list of run ranges
            runlist = options.runlist.split(',')
            if len(runlist) == 0:
                print 'Invalid run list specified!'
                sys.exit()

            # Go through and check for run ranges
            for runstr in runlist:
                subrunlist = runstr.split('-')
                
                if len(subrunlist) == 1: # Single run
                    self.runList.append(int(subrunlist[0]))
                    
                elif len(subrunlist) == 2: # Range of runs
                    for runnum in range(int(subrunlist[0]), int(subrunlist[1])+1):
                        self.runList.append(runnum)

                else: # Too many parameters
                    print 'Invalid run list segment found:', runstr
                    sys.exit()

            self.runList.sort()
            if self.verbose:
                print 'Finished parsing run list:',
                for runnum in self.runList:
                    print runnum,
                print

# Executed from the command line
if __name__ == '__main__':
    rlt = RunLumiTime()
    rlt.execute()

                
