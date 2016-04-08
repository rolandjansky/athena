#!/usr/bin/env python
#
# vdMScanMaker.py
#
# Main application script to generate ntuples of vdM scan data
#
import os
import sys
import shutil
import array
import time, calendar

from optparse import OptionParser

# Utility to unpack BCID blobs
from AtlDataSummary.AtlDataSumLumiData import LumiBCIDData

# General COOL reader object
from CoolLumiUtilities.CoolDataReader import CoolDataReader

# Get our global DB handler object
from CoolLumiUtilities.LumiDBHandler import LumiDBHandler

# Ntuple definition
from CoolLumiUtilities.ScanNtupleHandler import ScanNtupleHandler

from PyCool import cool

from ROOT import TFile, TTree, gROOT, AddressOf
        
# Simple utility for converting time strings
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
                return None

def timeString(iovkey):
    # Convert the IOVkey (63 bit) to a string representing time stamp
    if (iovkey == cool.ValidityKeyMin):
        return 'ValidityKeyMin'
    elif (iovkey == cool.ValidityKeyMax):
        return 'ValidityKeyMax'
    else:
        stime=int(round(iovkey/1000000000L))
        return time.strftime('%Y-%m-%d:%H:%M:%S/UTC', time.gmtime(stime))
    

class OflLumiMaker:

    def __init__(self):

        # Control output level
        self.verbose = False

        # Output root file name
        self.fileName = 'scan.root'

        # Location of scan data 
        self.tdaqDB = 'COOLONL_TDAQ/CONDBR2'

        self.scanFolder = '/TDAQ/OLC/LHC/SCANDATA'     # vdM Scan parameter data
        self.beamFolder = '/TDAQ/OLC/LHC/BEAMPOSITION'
        self.fillFolder = '/TDAQ/OLC/LHC/FILLPARAMS'
        self.lbdataFolder = '/TDAQ/OLC/LHC/LBDATA'
        self.bunchFolder = '/TDAQ/OLC/LHC/BUNCHDATA'
        self.bunchLumiFolder = '/TDAQ/OLC/BUNCHLUMIS' 
        self.lumiFolder = '/TDAQ/OLC/LUMINOSITY'

        # Calendar time range to look for vdM scan data
        self.startTime = '2010-10-01:07:00:00/UTC'
        self.endTime ='2010-10-01:14:00:00/UTC'

        # Time as COOL IOV
        self.startTimeIOV = None
        self.endTimeIOV = None
        
        self.startLBIOV = None
        self.endLBIOV = None
        
        # Pointers to CoolDataReader return objects
        self.lhcData = None
        self.lumiData = None
        self.scanData = None
        self.lbdataData = None
        self.bunchdataData = None

        self.ions = False
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        # Find the scan data
        self.findScanData()

        # Write the data to ntuple
        nt = ScanNtupleHandler()
        nt.fileName = self.fileName
        nt.ions = self.ions
        
        nt.open()
        nt.init()
        nt.fill(self) # Must pass self reference to get access to data
        nt.close()
        
    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-o", "--output",
                          dest="outfile", metavar="FILE", default=self.fileName,
                          help="specify output ROOT file name - default: "+self.fileName)

        parser.add_option("--startTime",
                          dest="starttime", metavar="TIME",
                          help="set starting time (YYYY-MM-DD:HH:MM:SS)")

        parser.add_option("--endTime",
                          dest="endtime", metavar="TIME",
                          help="set ending time (YYYY-MM-DD:HH:MM:SS)")

        parser.add_option("--ions",
                          dest="ions", default=self.ions, action="store_true",
                          help="Use Heavy Ion variable list (not used currently!)")
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.ions = options.ions
         
        # Parse times
        if options.starttime != None:
            self.startTime = options.starttime
        if options.endtime != None:    
            self.endTime = options.endtime

        self.fileName = options.outfile
        
    def findScanData(self):
        print 'vdMScanMaker.findScanData() called'

        # Based on the (text) starting and ending times, find the available scan entries in COOL

        # First, convert time strings to COOL IOV times
        self.startTimeIOV = timeVal(self.startTime)
        if self.startTimeIOV == None:
            print 'OflLumiMaker.findScanData - Error converting start time', self.startTime,'!'
            sys.exit()
            
        self.endTimeIOV = timeVal(self.endTime)
        if self.endTimeIOV == None:
            print 'OflLumiMaker.findScanData - Error converting end time', self.endTime,'!'
            sys.exit()
            
        # Load the scan data
        self.scanData = CoolDataReader(self.tdaqDB, self.scanFolder)
        self.scanData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.scanData.readData():
            print 'OflLumiMaker.findScanData - No scan data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()

        if self.verbose or True:
            for obj in self.scanData.data:
                run = obj.payload()['RunLB'] >> 32
                lb = obj.payload()['RunLB'] & 0xFFFFFFFF
                print timeString(obj.since()), run, lb, (obj.until()-obj.since())/1E9, obj.payload()['StepProgress'], obj.payload()['ScanningIP'], obj.payload()['AcquisitionFlag'], obj.payload()['NominalSeparation'], obj.payload()['ScanInPlane']

        # Load the beam positions
        self.beamData = CoolDataReader(self.tdaqDB, self.beamFolder)
        self.beamData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.beamData.readData():
            print 'OflLumiMaker.findScanData - No beam separation data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.beamData.data:
                run = obj.payload()['RunLB'] >> 32
                lb = obj.payload()['RunLB'] & 0xFFFFFFFF
                print run, lb, obj.payload()['B1_PositionAtIP_H'], obj.payload()['B1_PositionAtIP_V'], obj.payload()['B2_PositionAtIP_H'], obj.payload()['B2_PositionAtIP_V']
                
        # Load the fill parameters
        self.fillData = CoolDataReader(self.tdaqDB, self.fillFolder)
        self.fillData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.fillData.readData():
            print 'OflLumiMaker.findScanData - No fill parameters data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.fillData.data:
                print obj.payload()['Beam1Bunches'], obj.payload()['Beam2Bunches'], obj.payload()['LuminousBunches']


        # Load the lbdata parameters
        self.lbdataData = CoolDataReader(self.tdaqDB, self.lbdataFolder)
        self.lbdataData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.lbdataData.readData():
            print 'OflLumiMaker.findScanData - No LBDATA data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.lbdataData.data:
                print 'LBDATA', obj.channelId(), obj.payload()['Beam1Intensity'], obj.payload()['Beam2Intensity']

        # Load the BUNCHDATA parameters
        self.bunchData = CoolDataReader(self.tdaqDB, self.bunchFolder)

        self.bunchData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.bunchData.readData():
            print 'OflLumiMaker.findScanData - No BUNCHDATA data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.bunchData.data:
                print 'BUNCHDATA', obj.channelId(), obj.payload()['B1BunchAverage'], obj.payload()['B2BunchAverage']

        # Load the BUNCHLUMI parameters
        self.bunchLumi = CoolDataReader(self.tdaqDB, self.bunchLumiFolder)

        self.bunchLumi.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.bunchLumi.readData():
            print 'OflLumiMaker.findScanData - No BUNCHLUMIS data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
        if self.verbose:
            for obj in self.bunchLumi.data:
                print 'BUNCHLUMI', obj.channelId(), obj.payload()['AverageRawInstLum']

        # Load the luminosity data
        self.lumiData = CoolDataReader(self.tdaqDB, self.lumiFolder)
        self.lumiData.setIOVRange(self.startTimeIOV, self.endTimeIOV)
        if not self.lumiData.readData():
            print 'OflLumiMaker.findScanData - No LUMINOSITY data found in range', self.startTime, 'to', self.endTime,'!'
            sys.exit()
            
# Executed from the command line
if __name__ == '__main__':
    olm = OflLumiMaker()
    olm.execute()

                
