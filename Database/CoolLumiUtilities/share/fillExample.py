#!/usr/bin/env python
#
# fillExample.py
#
# Utility to dump a text file with information from each fill stable beams period
#
#
import os
import sys
import array
import time, calendar

from optparse import OptionParser
from PyCool import cool

# Get our global DB handler object
from CoolLumiUtilities.CoolDataReader import CoolDataReader
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler

# Class to keep track of information for each stable beam interval
class StableData:

    def __init__(self):
        self.startTime = cool.ValidityKeyMax
        self.endTime   = cool.ValidityKeyMin

        self.fillNumber = 0

    def __str__(self):
        startStr = time.asctime(time.localtime(self.startTime/1E9))
        endStr   = time.asctime(time.localtime(self.endTime/1E9))
        return "Fill %s (%s - %s)" % (self.fillNumber, startStr, endStr)

class FillDumper:

    def __init__(self):

        # Control output level
        self.verbose = False

        # Starting and ending time strings (from command line)
        self.startTime = '2010-10-01'
        self.endTime = '2010-11-01'

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

        # Convert start/end to COOL IOVs
        self.convertTime()

        # Read Fill information
        self.readFillData()

        # Read all COOL data
        self.readRunData()

        self.printData()

    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("--startTime",
                          dest="start", metavar="YYYY-MM-DD:HH:MM:SS", default=self.startTime,
                          help="specify starting time")

        parser.add_option("--endTime",
                          dest="end", metavar="YYYY-MM-DD:HH:MM:SS", default=self.endTime, 
                          help="specify ending time")

        parser.add_option('-o', '--output',
                          dest='outfile', metavar = "FILE", default=self.outFile,
                          help="select output file")
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        if options.verbose: self.verbose = options.verbose

        self.outFile = options.outfile
        self.startTime = options.start
        self.endTime = options.end
        
    # Convert the time strings into proper COOL IOV records
    def convertTime(self):

        self.startIOV = self.parseTime(self.startTime)
        self.endIOV = self.parseTime(self.endTime)

        if self.startIOV == None:
            print >>sys.stderr,  'Invalid starting time specification:', self.startTime
            sys.exit()

        if self.endIOV == None:
            print >> sys.stderr, 'Invalid ending time specification:', self.endTime
            sys.exit()

        if self.verbose:
            print 'convertTime found startTime:', self.startTime, '->', str(self.startIOV)
            print 'convertTime found endTime:  ', self.endTime, '->', str(self.endIOV)
            
    def parseTime(self, timestr):

        # Try to parse time string in various formats

        # Fully specified
        try:
            ts = time.strptime(timestr, '%Y-%m-%d:%H:%M:%S/%Z')
            return int(calendar.timegm(ts))*1000000000L
        except ValueError:
            pass

        # Try again with UTC attached
        try:
            ts = time.strptime(timestr+'/UTC', '%Y-%m-%d:%H:%M:%S/%Z')
            return int(calendar.timegm(ts))*1000000000L
        except ValueError:
            pass

        # Try again with just day specified
        try:
            ts = time.strptime(timestr, '%Y-%m-%d')
            return int(calendar.timegm(ts))*1000000000L
        except ValueError:
            pass

        # OK, I think we are out of luck
        return None
    
    def readFillData(self):

        # Use defined IOV range and read all data from FILLSTATE folder
        # Then parse this to produce a dict with stable beam ranges

        # LHC information
        if self.verbose: print 'Reading', self.fillFolder
        self.fill = CoolDataReader('COOLOFL_DCS/COMP200', '/LHC/DCS/FILLSTATE')
        self.fill.setIOVRange(self.startIOV, self.endIOV)
        self.fill.readData()

        self.stableList = []
        stableData = None

        # Parse information by going through each record in folder
        for obj in self.fill.data:
            fillNumber = obj.payload()['FillNumber']
            stable = obj.payload()['StableBeams']

            # Protection against bogus values
            if fillNumber == None:
                print 'Fill number is NULL for ', range,'!'
                fillNumber = 0

            if stable == None:
                print 'StableBeams is NULL for ', range,'!'
                stable = False

            # Check if we are not in stable beams
            if not stable: 
                
                # Do we have a valid StableData object?
                if stableData != None:
                    # Yes, save it to the list
                    self.stableList.append(stableData)
                    stableData = None

                continue

            # We have found a stable beams period

            # Is this new?
            if stableData == None:
                stableData = StableData()
                stableData.startTime = obj.since()
                stableData.endTime = obj.until()
                stableData.fillNumber = obj.payload()['FillNumber']
                continue

            # Not new, is this an extension of the previous one?
            if obj.since() == stableData.endTime:
                # Yes, adjust end time and continue
                stableData.endTime = obj.until()
                continue

            # No, this is a new stable period (note this shouldn't happen, but handle it anyways)

            if self.verbose:
                print "Found successive records that aren't contiguous in time!"
                print stableData

            # Save previous
            self.stableList.append(stableData)
            
            # Create new
            stableData = StableData()
            stableData.startTime = obj.since()
            stableData.endTime = obj.until()
            stableData.fillNumber = obj.payload()['FillNumber']

            if self.verbose:
                print stableData

        # OK, all done
        if self.verbose:
            for obj in self.stableList:
                print obj



    def readRunData(self):

        # Set up data readers here
        # Time->RunLB mapping
        self.lbtime = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBTIME')

        # RunLB->Time mapping
        self.lblb = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLB')

        # ATLAS ready flag
        self.ready = CoolDataReader('COOLONL_TDAQ/COMP200', '/TDAQ/RunCtrl/DataTakingMode')

        # For each stable beams time interval, find the run range along with other useful information
        for sb in self.stableList:

            if self.verbose: print 'Reading run data for %s' % sb

            # Time->RunLB mapping
            self.lbtime.setIOVRange(sb.startTime, sb.endTime)
            self.lbtime.readData()

            run = self.lbtime.data[0].payload()['Run']
            lb = self.lbtime.data[0].payload()['LumiBlock']
            startRunLB = (run<<32) + lb

            run = self.lbtime.data[-1].payload()['Run']
            lb = self.lbtime.data[-1].payload()['LumiBlock']
            endRunLB = (run<<32) + lb
        
            sb.startRunLB = startRunLB
            sb.endRunLB = endRunLB

            # Now, load the ATLAS ready flag (this is RunLB keyed)
            self.ready.setIOVRange(startRunLB, endRunLB)
            self.ready.readData()

            # Look for first ready lumiblock
            sb.readyRunLB = None
            for obj in self.ready.data:
                if not obj.payload()['ReadyForPhysics']:
                    continue

                sb.readyRunLB = obj.since()
                break  # Only need first one

            # To find time from stable beams to ATLAS ready need to translate ready RunLB back into time
            if sb.readyRunLB == None:
                sb.dTime = -1.
                continue

            self.lblb.setIOVRange(sb.readyRunLB, sb.readyRunLB)
            self.lblb.readData() # Will read one IOV which spans the RunLB when ATLAS ready is declared

            sb.dTime = (self.lblb.data[0].payload()['StartTime'] - sb.startTime)/1E9 # Cool time is in ns, convert to seconds here
            

    def printData(self):

        # Open outfile if desired
        if self.outFile != None:
            f = open(self.outFile, 'w')
        else:
            f = sys.stdout

        # OK, now we want to go through the luminosity records and match the other data
        for sb in self.stableList:

            # OK, print it out
            if sb.readyRunLB == None:
                print >>f, sb
            else:
                print >>f, "%s %d/%d %.1f" % (sb, sb.readyRunLB>>32, sb.readyRunLB&0xFFFF, sb.dTime)

    
# Executed from the command line
if __name__ == '__main__':
    fd = FillDumper()
    fd.execute()


                
