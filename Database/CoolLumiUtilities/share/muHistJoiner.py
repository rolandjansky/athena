#!/usr/bin/env python
#
# muHistJoiner.py
#
# Simple script to combine histograms created by muHistMaker
# Can specify a run range or a GRL
#
# Eric Torrence - 4 April 2011
#
import os
import sys
import glob
import time
import array
import shutil
import subprocess

from optparse import OptionParser

from CoolLumiUtilities.LumiGRLParser import LumiGRLParser

from ROOT import TFile, TH1D, gDirectory

class MuHistJoiner:

    def __init__(self):

        # Control output level
        self.verbose = True

        # List of (integer) run numbers specified on the command line
        self.runList = []

        # Parsed GRL
        self.grl = None
        
        # Output file
        self.outFile = 'joined.root'

        # Input directory
        self.inDir = '.'

        # Output histogram name
        self.houtName = 'intperbx'
        
    # Called in command-line mode
    def execute(self):

        # Handle command-line switches
        self.parseOpts()

        self.out = None
        self.hout = None

        if len(self.runList) > 1:
            print 'Processing %d runs from %d to %d' % (len(self.runList), self.runList[0], self.runList[-1])

        elif len(self.runList) == 1:
            print 'Processing run %d' % self.runList[0]
            
        # Process each run in self.runList
        for run in self.runList:

            # Look for histogram file
            file = '%s/run%d_mu.root' % (self.inDir, run)

            if not os.path.exists(file): continue

            print 'Adding %d' % run
            
            # open file and read histogram
            infile = TFile(file, 'read')
            
            if self.grl == None:
                self.addRuns()

            else:
                self.addLumiBlocks(run)

            if self.hout != None:
                self.hout.Print()
                
            infile.Close()    

        if self.hout == None:
            print 'No output histogram created!'
            return

        # Write output file
        self.out.cd()
        self.hout.Write()
        self.out.Close()

    # Simple routine to just add summary histograms per run
    def addRuns(self):
        
        # Read summary histogram
        hin = gDirectory.Get('readyMuDist')

        if hin == None:
            print 'No mu histogram found for run', run
            return

        hin.Print()

        # Check bin 1
        bin1 = hin.GetBinContent(1)
        if bin1 > 100:
            print '==> Zero bin: %f' %  bin1
        
        if self.hout == None:
            print 'Creating new output histogram'
            self.out = TFile(self.outFile, 'recreate')
            self.hout = TH1D(hin)
            self.hout.SetName(self.houtName)
        else:
            self.hout.Add(hin)

        return

    # More involved routine to add histograms per lumi block
    def addLumiBlocks(self, runnum):

        # Read LBs
        hlist = []
        for lb in self.grl.lumiBlockList(runnum):

            hin = gDirectory.Get(str(lb))
            if hin == None:
                print "Can't find histogram for Run/LB: %d/%d!" % (run, lb)
                continue

            hlist.append(hin)

        for h in hlist:
            if self.hout == None:
                print 'Creating new output histogram'
                self.out = TFile(self.outFile, 'recreate')
                self.hout = TH1D(h)
                self.hout.SetName(self.houtName)
            else:
                self.hout.Add(h) 

        return
    
    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-g", "--grl",
                          dest="grlname", metavar="FILE",
                          help="read run/LB range from GRL")
        
        parser.add_option("-r", "--updateRun",
                          dest="runlist", metavar="RUN",
                          help="update specific run, or comma separated list")

        parser.add_option("-o", "--outputFile",
                          dest="outfile", metavar="FILE", default=self.outFile,
                          help="output histogram to filename")

        parser.add_option("-i", "--inputDir",
                           dest="indir", metavar="DIR", default=self.inDir,
                           help="directory to find input ROOT files")

        parser.add_option("--histName",
                          dest='hname', metavar="NAME", default=self.houtName,
                          help="name of output histogram (Default: %s)" % self.houtName)
        
        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.verbose = options.verbose
        self.outFile = options.outfile
        self.inDir = options.indir
        self.houtName = options.hname
        
        # Look for data specifiers
        if options.grlname != None:
            if self.verbose:
                print 'Parsing GRL', options.grlname
                
            self.grl = LumiGRLParser(options.grlname)
            self.runList = self.grl.runList
            
            if self.verbose:
                print 'Finished parsing GRL list:',
                for runnum in self.runList:
                    print runnum,
                print

        # Parse run list
        elif options.runlist != None:

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
    mhj = MuHistJoiner()
    mhj.execute()

                
