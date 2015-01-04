#!/usr/bin/env python
#
# fillDBFromNtuple.py
#
# Derived from calibNtupleMaker, read data from ntuple first
#
# Updated for use in 2012.  Removed historical stuff from 2011
#
# Eric Torrence - 29 April 2012
#
import os
import sys
import glob
import time
import array
import shutil
import subprocess

from optparse import OptionParser

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import forceOpen,ensureFolder,athenaDesc

from ROOT import TChain, AddressOf

class NtupleDBMaker:

    def __init__(self):

        # Control output level
        self.verbose = True

        # List of (integer) run numbers specified on the command line
        self.runList = []

        # Directory to find ntuples
        self.ntdir = 'ntuples'
        
        # Output DB parameters
        self.outDBString = "sqlite://;schema=./mycool.db;dbname=COMP200"
        self.oflLumiFolder = '/TRIGGER/OFLLUMI/LBLESTOFL'
        self.oflLumiTag = 'OflLumi-8TeV-001'
        self.outDB = None
        self.coolSpec = None
        self.outFolder = None
        
        self.nBCID = 3564

        self.outdir = '.'

        # All recalibrated channels
        self.algname = dict()
        self.algname[101] = 'LucAND'
        self.algname[102] = 'LucOR'
        self.algname[103] = 'LucHitOR'
        # self.algname[104] = 'LucHitAND'
        self.algname[105] = 'LucA'
        self.algname[106] = 'LucC'

        self.algname[201] = 'BcmHOR'
        self.algname[202] = 'BcmHAND'
        self.algname[206] = 'BcmHORC'

        self.algname[211] = 'BcmVOR'
        self.algname[212] = 'BcmVAND'
        self.algname[216] = 'BcmVORC'

        self.algname[221] = 'BcmTOR'
        self.algname[222] = 'BcmTAND'
        self.algname[226] = 'BcmTORC'

        self.algname[1001] = 'Bcm11HOR'
        self.algname[1002] = 'Bcm11HAND'
        
        self.algname[1011] = 'Bcm11VOR'
        self.algname[1012] = 'Bcm11VAND'


        
        # This are the channels we will fill
        self.chanList = self.algname.keys()
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        if not self.createOutputDB():
            print 'Error creating output DB!'
            sys.exit()

        # Load all ntuples into one TChain
        self.ch = TChain("lumiData")
        for run in self.runList:
            filename = '%s/r%d.root' % (self.ntdir, run)
            if not os.path.isfile(filename): continue
            print 'Loading file %s' % filename
            self.ch.Add(filename)

        # Setup ntuple
        self.ntupleSetup()

        # Process all entries
        for i in range(self.ch.GetEntries()):

            self.ch.GetEvent(i)

            run = self.lbData.fRun
            lb  = self.lbData.fLB

            # Skip bad data
            if run == 0 or lb == 0: continue
            
            runlb = (run << 32) + lb
            muToLumi = self.muData.fMuToLumi
            valid = 0 # Have to hardcode this as valid...
            
            lumiSum = dict()
            for chan in self.chanList:
                lumiSum[chan] = 0.

            nBunch = 0
            # Loop over all BCIDs
            for j in range (3564):

                # Colliding bunches only
                if self.muData.fStatus[j] != 1: continue

                # Keep track of colliding bunches
                nBunch += 1
                
                # Loop over defined algorithms
                for chan in self.chanList:
                    
                    lumiSum[chan] += muToLumi * self.muVal[chan][j]
                    

            # Now, fill DB object for each algorithm
            for chan in self.chanList:
                
                # Fill DB object
                payload = cool.Record(self.coolSpec)
                payload['LBAvInstLumi'] = lumiSum[chan]
                payload['LBAvEvtsPerBX'] = lumiSum[chan] / muToLumi / nBunch
                payload['Valid'] = valid

                # These are *not* afterglow corrected.  Don't add 50...
                self.outFolder.storeObject(runlb, runlb+1, payload, chan, self.oflLumiTag, True)

        # End of loop over LBs

        # Now save this into the final DB
        try:
            self.outFolder.flushStorageBuffer()
        except Exception, e:
            print e
            print 'Error storing lumi records to local DB!'


    # Define ntuple and set up pointers to read data
    def ntupleSetup(self):

        from calibNtupleMaker12 import NtupleHandler
        self.lbData = NtupleHandler().getLBDataStruct()
        self.ch.SetBranchAddress('LBDATA', AddressOf(self.lbData, 'fStartTime'))

        self.muData = NtupleHandler().getMuDataStruct()
        self.ch.SetBranchAddress('muToLumi', AddressOf(self.muData, 'fMuToLumi'))
        self.ch.SetBranchAddress('Status',   AddressOf(self.muData, 'fStatus'))

        
        for chan in self.chanList:
            self.ch.SetBranchAddress('Mu'+self.algname[chan], AddressOf(self.muData, 'fMu'+self.algname[chan]))

        self.muVal = dict()
        self.muVal[101] = self.muData.fMuLucAND
        self.muVal[102] = self.muData.fMuLucOR
        self.muVal[103] = self.muData.fMuLucHitOR
        self.muVal[105] = self.muData.fMuLucA
        self.muVal[106] = self.muData.fMuLucC

        self.muVal[201] = self.muData.fMuBcmHOR
        self.muVal[202] = self.muData.fMuBcmHAND
        self.muVal[206] = self.muData.fMuBcmHORC

        self.muVal[211] = self.muData.fMuBcmVOR
        self.muVal[212] = self.muData.fMuBcmVAND
        self.muVal[216] = self.muData.fMuBcmVORC
        
        self.muVal[221] = self.muData.fMuBcmTOR
        self.muVal[222] = self.muData.fMuBcmTAND
        self.muVal[226] = self.muData.fMuBcmTORC

        self.muVal[1001] = self.muData.fMuBcm11HOR
        self.muVal[1002] = self.muData.fMuBcm11HAND

        self.muVal[1011] = self.muData.fMuBcm11VOR
        self.muVal[1012] = self.muData.fMuBcm11VAND


    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-r", "--updateRun",
                          dest="runlist", metavar="RUN",
                          help="update specific run, or comma separated list")

        parser.add_option("-t", "--tag",
                           dest="lumitag", metavar="TAG", default=self.oflLumiTag,
                           help='output luminosity tag (default: %s)' % self.oflLumiTag)

        parser.add_option('-d', '--inputDir',
                          dest='ntdir', metavar = "DIR", default=self.ntdir,
                          help='input ntuple directory (default: %s)' % self.ntdir)
        
        parser.add_option("-o", "--outputDir",
                          dest="outdir", metavar="DIR", default=self.outdir,
                          help='directory for output ntuple (default: %s)' % self.outdir)
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.oflLumiTag = options.lumitag
        self.ntdir = options.ntdir
        self.outdir = options.outdir
            
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

    def createOutputDB(self):
        print 'calibNtupleMaker.createOutputDB() called'
               
        print 'calibNtupleMaker.createOutputDB() - connecting to DB', self.outDBString
        try:
            self.outDB = forceOpen(self.outDBString)
        except Exception, e:
            print e
            print "calibNtupleMaker.createOutputDB() - Could not open DB %s" % self.outDBString
            return False
        
        # Now create folder if needed
        self.coolSpec=cool.RecordSpecification()
        self.coolSpec.extend("LBAvInstLumi", cool.StorageType.Float)
        self.coolSpec.extend("LBAvEvtsPerBX",cool.StorageType.Float)
        self.coolSpec.extend("Valid", cool.StorageType.UInt32)
        try:
            self.outFolder=ensureFolder(self.outDB, self.oflLumiFolder, self.coolSpec, athenaDesc(True,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)
        except Exception,e:
            print e
            print "calibNtupleMaker.createOutputDB() - Could not access or create folder %s" % self.oflLumiFolder
            return False
               
        print "calibNtupleMaker.createOutputDB() - Opened %s %s" % (self.outDBString, self.oflLumiFolder)
        self.outFolder.setupStorageBuffer()
        
        return True
           
                
# Executed from the command line
if __name__ == '__main__':
    mhm = NtupleDBMaker()
    mhm.execute()

                
