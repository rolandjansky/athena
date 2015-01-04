#!/usr/bin/env python
#
# dumpLuminosity.py
#
# Utility to dump a text file with luminosity and machine parameters by time stamp.
#
#
import os
import sys
import array
import time, calendar

from optparse import OptionParser

# Get our global DB handler object
from CoolLumiUtilities.CoolDataReader import CoolDataReader
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler

class LumiDumper:

    def __init__(self):

        # Control output level
        self.verbose = False

        self.onlTDAQDB = 'COOLONL_TDAQ/COMP200'
        self.onlLumiFolder = '/TDAQ/OLC/LUMINOSITY'    # Onine luminosity by time
        self.currentFolder = '/TDAQ/OLC/LHC/LBDATA'    # Bunch currents

        self.fillDB = 'COOLOFL_DCS/COMP200'
        self.fillFolder = '/LHC/DCS/FILLSTATE'

        # Offline luminosity
        self.useOffline = False
        self.lumiTag = 'OflLumi-8TeV-002'
        
        # Channel to chose (preferred by default)
        self.lumiChan = 0
        
        # Starting and ending time strings (from command line)
        self.startTime = '2010-10-01'
        self.endTime = '2010-11-01'

        # Instantiate the LumiDBHandler, so we can cleanup all COOL connections in the destructor
        self.dbHandler = LumiDBHandler()

        # Output file (default stdout)
        self.outFile = None

        self.lumiDict = dict()

    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Convert start/end to COOL IOVs
        self.convertTime()

        # Open outfile if desired
        if self.outFile != None:
            f = open(self.outFile, 'w')
        else:
            f = sys.stdout

            # Read all COOL data
        self.readData()

        self.printData(f)

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

        parser.add_option("--channel",
                          dest='chan', metavar='N', default=self.lumiChan,
                          help = 'specify luminosity channel (default: %d)' % self.lumiChan)

        parser.add_option('-o', '--output',
                          dest='outfile', metavar = "FILE", default=self.outFile,
                          help="select output file")

        parser.add_option('--offline',
                          dest='useoffline', default=self.useOffline, action='store_true',
                          help = 'use offline luminosity folder (default: online)')

        parser.add_option('--lumiTag',
                          dest='lumitag', default=self.lumiTag, metavar='TAG',
                          help='offline luminosity tag (default: %s)' % self.lumiTag)
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        if options.verbose: self.verbose = options.verbose

        # Offline optinos
        self.useOffline = options.useoffline
        self.lumiTag = options.lumitag
        
        self.outFile = options.outfile
        self.startTime = options.start
        self.endTime = options.end
        self.lumiChan = int(options.chan)
        
    # Convert the time strings into proper COOL IOV records
    def convertTime(self):

        self.startIOV = self.parseTime(self.startTime)
        self.endIOV = self.parseTime(self.endTime)

        if self.startIOV == None:
            print >>sys.stderr,  'Invalid time specification:', self.startTime
            sys.exit()

        if self.endIOV == None:
            print >> sys.stderr, 'Invalid time specification:', self.endTime
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
    
    def readData(self):

        # Luminosity folder
        if self.verbose: print 'Reading', self.onlLumiFolder
        self.lumi = CoolDataReader(self.onlTDAQDB, self.onlLumiFolder)
        self.lumi.setIOVRange(self.startIOV, self.endIOV)
        self.lumi.setChannelId(self.lumiChan) # 0 - Preferred, 101 - Lucid Evt AND, 102 - Lucid Evt OR
        self.lumi.readData()

        # Beam folder
        if self.verbose: print 'Reading', self.currentFolder
        self.curr = CoolDataReader(self.onlTDAQDB, self.currentFolder)
        self.curr.setIOVRange(self.startIOV, self.endIOV)
        self.curr.setChannelId(1) # 0 - BPTX, 1 - BCT
        self.curr.readData()

        # LHC information
        if self.verbose: print 'Reading', self.fillFolder
        self.fill = CoolDataReader(self.fillDB, self.fillFolder)
        self.fill.setIOVRange(self.startIOV, self.endIOV)
        self.fill.readData()

        if not self.useOffline: return

        # Time->RunLB mapping
        if self.verbose: print 'Reading /TRIGGER/LUMI/LBTIME'
        self.lbtime = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBTIME')
        self.lbtime.setIOVRange(self.startIOV, self.endIOV)
        self.lbtime.readData()

        run = self.lbtime.data[0].payload()['Run']
        lb = self.lbtime.data[0].payload()['LumiBlock']
        startRunLB = (run<<32) + lb
        
        if self.verbose:
            print 'Reading offline lumi from %d/%d to ' % (run, lb) , # Comma surpresses new line
            
        run = self.lbtime.data[-1].payload()['Run']
        lb = self.lbtime.data[-1].payload()['LumiBlock']
        endRunLB = (run<<32) + lb
        if self.verbose:
            print '%d/%d' % (run, lb)

        self.ofllumi = CoolDataReader('COOLOFL_TRIGGER/COMP200', '/TRIGGER/OFLLUMI/LBLESTOFL')
        self.ofllumi.setIOVRange(startRunLB, endRunLB)
        self.ofllumi.setChannelId(self.lumiChan)
        self.ofllumi.setTag(self.lumiTag)
        self.ofllumi.readData()

        # Set up time->lumi map
        lumiD = dict()
        self.lumiDict = dict()
        for obj in self.ofllumi.data:        
            lumiD[obj.since()] = obj

        for obj in self.lbtime.data:
            run = obj.payload()['Run']
            lb = obj.payload()['LumiBlock']
            runlb = (run << 32) + lb
            if runlb in lumiD:
                self.lumiDict[obj.since()] = lumiD[runlb]

    def printData(self, f):

        # OK, now we want to go through the luminosity records and match the other data
        for obj in self.lumi.data:
            # Take average time in middle of IOV
            time = int((obj.since()/1E9+obj.until()/1E9)/2) # Time in seconds
            
            valid = obj.payload()['Valid']

            physvalid = valid & 0x003
            allvalid = valid & 0x030
            olcvalid = valid & 0x300

            # Choose online or offline luminosity
            if self.useOffline:

                if obj.since() not in self.lumiDict:
                    lumi = 0. # No offline lumi found

                else:
                    oflvalid = self.lumiDict[obj.since()].payload()['Valid']
                    lumi = self.lumiDict[obj.since()].payload()['LBAvInstLumi']

                    if oflvalid & 1: # Invalid data
                        runlb = self.lumiDict[obj.since()].since()
                        run = runlb >> 32
                        lb  = runlb & 0xFFFFFFFF
                        print >>sys.stderr, 'No valid offline luminosity for run/LB',run, '/', lb
                        continue

            else:
                
                # Depending on validity, take either Phys (with physics BG applied) or All
                if physvalid == 0:
                    lumi = obj.payload()['LBAvInstLumPhys']
                elif allvalid == 0:
                    lumi = obj.payload()['LBAvInstLumAll']
                elif olcvalid == 0:
                    lumi = obj.payload()['LBAvOLCInstLum']
                else:
                    run = obj.payload()['RunLB'] >> 32
                    lb =  obj.payload()['RunLB'] & 0xFFFFFFFF
                    print >>sys.stderr, 'No valid luminosity for run/LB',run, '/', lb
                    continue
            
            # Now, based on this time, lets find other data
            beamobj = self.matchTime(obj, self.curr.data)
            if beamobj == None:
                beam1 = -1.
                beam2 = -1.
            else:
                beam1 = beamobj.payload()['Beam1Intensity']
                beam2 = beamobj.payload()['Beam2Intensity']
                
            fillobj = self.matchTime(obj, self.fill.data)
            if fillobj == None:
                ebeam = -1
                stable = -1
                fill = -1
            else:
                ebeam = int(fillobj.payload()['BeamEnergyGeV'])
                stable = fillobj.payload()['StableBeams']
                fill =  fillobj.payload()['FillNumber']

            # OK, print it out
            print >>f, time, fill, stable, lumi, beam1, beam2, ebeam
        
    # Find a time match for the object in the list
    # Assume the list is sorted and queried in order, so we can drop elements already used
    def matchTime(self, theObj, theList):    

        # Match to midpoint time
        time = (theObj.since()+theObj.until())/2

        # Is the list empty
        if len(theList) == 0: return None

        # Check for quick match
        targetObj = theList[0]
        if targetObj.since() <= time and time < targetObj.until():
            if self.verbose: print 'Exact match'
            return targetObj

        # Check for problem
        if targetObj.since() > time:
            print >>sys.stderr, 'Trying to match time', time, 'but first list item starts at', targetObj.since()
            return None
        
        # Drop items until ending time is after the object we are matching
        while targetObj.until() <  time:
            if self.verbose: print 'Skipping record'
            theList.pop(0)
            if len(theList) == 0:
                return None
            targetObj = theList[0]
            
        # Check (again) for valid data
        if targetObj.since() <= time and time < targetObj.until():
            if self.verbose: print 'Match found'
            return targetObj

        # Something bad happened
        print >>sys.stderr, "Skipped records, but didn't find a match for time", time
        print >>sys.stderr, targetObj.since(), targetObj.until()
        return None
    
# Executed from the command line
if __name__ == '__main__':
    ld = LumiDumper()
    ld.execute()


                
