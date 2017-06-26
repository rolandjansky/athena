#!/usr/bin/env python                                                                                    
#                                                                                                       
# testCoolLumiUtilities.py                                                                              
#                                                                                                       
# Eric Torrence - 25 June 2014                                                                           
#                                                                                                        
# Simple script to test the python interface to the CoolLumiUtilities objects                            #                                                                                                        
# Global DB handler object to read COOL                                                                   
from CoolLumiUtilities.CoolDataReader import CoolDataReader

# Raw objects to handle COOL data                                                                         
import array

from optparse import OptionParser
from ROOT import *
##### LBLB


data2010 = [167963,167885,167844,167776,167680,167661,167607,167576,167575,166964]
data2011 = [191933,191920,191715,191676,191635,191628,191517,191514,191513,191428]
data2012 = [215589,216432,216416,216399,215643,215589,215571,215564,215562,215561,215559]

runList = []

parser = OptionParser(usage="usage: %prog [options]", add_help_option=False)
parser.add_option("-r", "--updateRun",
                  dest="runlist", metavar="RUN",
                  help="update specific run, or comma separated list")
parser.add_option("--y", "--chooseYear",
                  dest="yearlist", metavar="YEAR",
                  help="update specific run, or comma separated list")

(options, args) = parser.parse_args()


    
if options.runlist != None:
    runList=[]

    runlist = options.runlist.split(',')
    if len(runlist)==0:
        print 'Invalid run list specified!'
        sys.exit()
    for runstr in runlist:
        subrunlist = runstr.split('-')
        if len(subrunlist)==1:
            runList.append(int(subrunlist[0]))
        elif len(subrunlist)==2:#Range of 2 runs                                                  
            for runnum in range (int(subrunlist[1])+1):
                runList.append(runnum)
        else:
            print 'Invalid run list segment found:', runstr
            sys.exit()
    runList.sort()
else:
    runList = data2010

print len(runList)
countlb = 0
countpbd = 0
for run in runList:
    print run

    lblbReader = CoolDataReader('COOLONL_TRIGGER/COMP200', '/TRIGGER/LUMI/LBLB')
    lblbReader.setIOVRangeFromRun(run)
    lblbReader.readData()
    iovSince = lblbReader.data[0].payload()['StartTime']
    iovUntil = lblbReader.data[-1].payload()['EndTime']

    countlb = 0

    for obj in lblbReader.data:
        countlb += 1

    bunchPrbcidDeadtime = CoolDataReader("COOLONL_TRIGGER/COMP200", "/TRIGGER/LUMI/PerBcidDeadtime")
    bunchPrbcidDeadtime.setIOVRangeFromRun(run)
    bunchPrbcidDeadtime.readData()

    countbpd = 0
    
    for obj in bunchPrbcidDeadtime.data:
        countbpd += 1
    
    print countbpd
    print countlb
    if (countbpd-countlb==0):
        print "The same number of LB's"
        print run
    else: 
        print "Here is a difference "
        print run
    print (iovUntil-iovSince)/(10**9*3600), '-> end time'
    print "years after 1970", (iovUntil)/(10**9*3600*24*365)
