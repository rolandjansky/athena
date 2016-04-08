#!/usr/bin/env python
#
# trigNtupleMaker.py
#
# Test application to make ntuple of trigger rates vs. luminosity.
# This assumes a proper ATLAS release has been set up.
# Script should be called in the main production directory.  
# The needed directory structure will be set up if it doesn't already exist 
#

import os
import sys
import glob
import shutil
import filecmp
import datetime

from optparse import OptionParser

from CoolLumiUtilities.TriggerHandler  import TriggerHandler, runLBString
from CoolLumiUtilities.TrigNtupleHandler import TrigNtupleHandler

from CoolLumiUtilities.LumiDBHandler import LumiDBHandler
from CoolLumiUtilities.CoolDataReader import CoolDataReader
from CoolLumiUtilities.LumiDBCache import LumiDBCache

from PyCool import cool

class trigNtupleMaker:

    def __init__(self):

        #
        # Steering booleans to control execution
        #

        # Only ntuple atlasReady
        self.ready = True
        self.lumiChannel = 0
        self.lumiTag = 'OflLumi-8TeV-003'
        
        # Run helpers in verbose mode
        self.verbose = True
        
        # List of specific fills or runs to update (only)
        self.runList = []

        # Utlity routine for handling COOL connections
        self.dbHandler = LumiDBHandler()

        # Output directory for ntuple
        self.outdir = '.'

        # For diagnostic output only
        self.trigChan = 'L1_EM30'

    # Explicitly clean up our DB connections
    def __del__(self):
        self.dbHandler.closeAllDB()

    def execute(self):

        self.parseOpts()

        # Read offline luminosity
        lumiReader = CoolDataReader('COOLOFL_TRIGGER/COMP200', '/TRIGGER/OFLLUMI/LBLESTOFL')
        lumiReader.setChannelId(self.lumiChannel)
        lumiReader.setTag(self.lumiTag)

        # Read LAr noise bursts
        larReader = CoolDataReader('COOLOFL_LAR/COMP200', '/LAR/BadChannelsOfl/EventVeto')
        larReader.setTag('LARBadChannelsOflEventVeto-UPD4-04')

        # Time to use with LAr noise
        # lbtimeReader = CoolDataReaderCache('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBTIME')
        lblbReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLB')
        
        # Read ATLAS ready flag
        readyReader = LumiDBCache('COOLONL_TDAQ/COMP200', '/TDAQ/RunCtrl/DataTakingMode')
        
        for run in self.runList:

            print
            print 'Generating run', run

            rootfile = self.outdir+'/run'+str(run)+'.root'

            # Define ntuple - will open existing file if present 
            nt = TrigNtupleHandler()
            nt.fileName = rootfile
            nt.open(update=False)
            nt.initLBData()
            nt.initL1TrigData()
            
            # Load run information
            print 'Load trigger information'
            th = TriggerHandler()
            th.allL1Triggers = True
            th.trigList = []
            th.loadDataByRun(run)

            # Load lumi information
            print 'Load lumi information'
            lumiReader.setIOVRangeFromRun(run)
            lumiReader.readData()

            # Read ATLAS ready information
            print 'Load ATLAS ready information'
            readyReader.reader.setIOVRangeFromRun(run)
            readyReader.reader.readData()

            # Load time stamps
            print 'Load LBLB information'
            lblbReader.setIOVRangeFromRun(run)
            lblbReader.readData()
            startTime = lblbReader.data[0].payload()["StartTime"]
            endTime = lblbReader.data[-1].payload()["EndTime"]

            # Read bad LAr periods
            print 'Load LAr information'
            larReader.setIOVRange(startTime, endTime)
            larReader.readData()

            # Now make a list of bad lumi blocks
            print 'Finding bad LBs'
            badLB = set()
            for larData in larReader.data:

                if larData.payload()["EventVeto"] == 0:
                    continue
                
                tlo = larData.since()
                thi = larData.until()

                # Find all lumi blocks spanned by this range
                for lb in lblbReader.data:
                    if lb.payload()["EndTime"] <= tlo: continue
                    ss = lb.since()
                    if lb.payload()["StartTime"] < tlo:
                        badLB.add(ss)
                        print runLBString(ss)
                    if lb.payload()["StartTime"] < thi:
                        badLB.add(ss)
                        print runLBString(ss)
                    if lb.payload()["StartTime"] > thi: break

                    
            # Process 
            for obj in lumiReader.data:

                ss = obj.since()

                lumi = obj.payload()["LBAvInstLumi"]
                mu = obj.payload()["LBAvEvtsPerBX"]
                
                if ss not in th.trigL1Dict:
                    continue

                trigcount = th.trigL1Dict[ss].TBP[self.trigChan]
                dtime = th.trigL1Dict[ss].dtime
                if (dtime > 0.):
                    trigrate = trigcount/dtime
                else:
                    trigrate = 0.
                    
                atlasReady = False
                readypay = readyReader.getPayload(obj.since())
                if readypay != None:
                    atlasReady = readypay['ReadyForPhysics']
                    
                print runLBString(ss), atlasReady, lumi, mu, trigcount, trigrate,
                if trigrate > 0.:
                    print lumi/trigrate
                else:
                    print

                # ATLAS Ready only
                if self.ready and (not atlasReady): continue
                
                nt.clear()    
                nt.lbData.coolStartTime = th.trigL1Dict[ss].startTime
                nt.lbData.coolEndTime = th.trigL1Dict[ss].endTime
                nt.lbData.startTime = nt.lbData.coolStartTime/1.E9
                nt.lbData.endTime = nt.lbData.coolEndTime/1.E9
                nt.lbData.lbTime = dtime

                nt.lbData.run = obj.since() >> 32
                nt.lbData.lb = obj.since() & 0xFFFFFFFF

                nt.lbData.onlInstLum = lumi
                nt.lbData.onlEvtsPerBX = mu

                nt.lbData.ready = atlasReady
                nt.lbData.larVeto = (ss in badLB)
                
                # And save trigger counts
                nt.fillL1Trig(th.trigL1Dict[ss], th.trigChan) 
                nt.save()

                #for chan in range(256):
                #    print chan, nt.l1TBP[chan]

            nt.close()
            
    def parseOpts(self):

        parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)

        parser.add_option("-?", "--usage", action="store_true", default=False, dest="help",
                          help="show this help message and exit")
        
        parser.add_option("-v", "--verbose",
                     action="store_true", default=self.verbose, dest="verbose",
                     help="turn on verbose output")

        parser.add_option("-r", "--run",
                          dest="runList", metavar="RUN",
                          help="update specific run, or comma separated list.")

        parser.add_option("--notReady",
                  action="store_false", default=self.ready, dest="ready",
                  help="turn off AtlasReady requirement")

        parser.add_option("--lumiChan",
                  dest="lumichan", metavar="CHANNEL", default=self.lumiChannel,
                  help="specify luminosity channel (default %d)" % self.lumiChannel)
                  
        parser.add_option("--lumiTag",
                    dest="lumitag", metavar="TAG", default=self.lumiTag,
                    help="specify luminosity tag (default %s)" % self.lumiTag)
                                    
        parser.add_option("-o", "--outputDirectory",
                    dest="outdir", metavar="DIR", default=self.outdir,
                    help="specify output directory (default %s)" % self.outdir)
                                    

        parser.add_option("--trigChan",
           dest="trigchan", metavar="CHAN", default=self.trigChan,
           help="specify trigger channel for diagnostic output (default %s)" % self.trigChan)

        (options, args) = parser.parse_args()

        if options.help:
            parser.print_help()
            sys.exit()

        self.ready = options.ready
        self.lumiChannel = int(options.lumichan)
        self.lumiTag = options.lumitag
        self.outdir = options.outdir
        self.trigChan = options.trigchan

        if options.runList != None:

            # Parse list and enter into fill list
            self.runList = self.parseRunString(options.runList)
            
            if len(self.runList) == 0:
                print 'Invalid run list specified: %s!' % options.runList
                sys.exit()

            print 'Processing runs:', self.runList

    # Parse any list of numbers including comma-separated values and ranges
    def parseRunString(self, liststr):

        retlist = []
        tokenlist = liststr.split(',')
        if len(tokenlist) == 0:
            print 'Invalid list found:', liststr
            return []

        for valstr in tokenlist:
            subvallist = valstr.split('-')

            if len(subvallist) == 1: # Single value
                retlist.append(int(subvallist[0]))

            elif len(subvallist) == 2: # Range of values
                for val in range(int(subvallist[0]), int(subvallist[1])+1):
                    retlist.append(val)

            else: # Too many parameters
                print 'Invalid list segment found:', valstr
                return []

        # Make sure this is sorted
        retlist.sort()
        return retlist

        
# Executed from the command line
if __name__ == '__main__':
    tnm = trigNtupleMaker()
    tnm.execute()

    
