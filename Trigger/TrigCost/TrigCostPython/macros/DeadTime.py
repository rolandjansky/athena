#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import optparse,sys,string
from TrigCostPython import TrigCostAnalysis,TrigCostCool

# parse options
preamble=""

optparser = optparse.OptionParser(usage=preamble)

optparser.add_option('', '--xml',     type="string",  default="",
                     dest="xml", help="Write a DQ XML list")

optparser.add_option('', '--maxdeadtime', type="float", default=-1.0,
                     dest='maxdeadtime', help="Maximum deadtime for xml good LB list")

optparser.add_option('', '--runs',      type="string",           default="",
                     dest="runs",      help="Run Number")

optparser.add_option('', '--dump',   action="store_true",  default=False,
                     help="Print dead time to stdout")

optparser.add_option('', '--trigger', type="string", default="L1_RD0_FILLED",
                     dest="trigger",  help="Trigger to use for monitoring")

(opts, args) = optparser.parse_args()

# check the run number is valid
if opts.runs=="": 
    print 'Needs valid run number, --runs is a required option'
    sys.exit(2)

runs=[]
for runstr in string.split(opts.runs,","):
    run=-1
    try:
        run=string.atoi(runstr)
    except Exception,e:
        print "Failed to convert run string to integer"
        raise e
    runs.append(run)

# get rates from cool
ratecolls={}
for run in runs:
    ratecolls[run] = TrigCostCool.GetRates(run,-1,-1)

# make a deadtime[run][lb] map of deadtimes
deadtime={}
for (run,ratecoll) in ratecolls.items():
    deadtime[run]={}
    for lb in ratecoll.LBRange():
        ch=ratecoll.GetCostChain(lb,opts.trigger)
        if ch.GetTAPRate()==0:
            print "TAP=0? ",ch.GetTAVRate(),ch.GetTAPRate()
            deadtime[run][lb]=1.0
        else:
            deadtime[run][lb]=1.0-ch.GetTAVRate()/ch.GetTAPRate()
    
# dump dead time to stdout?
if opts.dump:
    for run in runs:
        print "Run = ",run
        print "lb     deadtime"
        print "---------------"
        for lb in deadtime[run].keys():
            print "% 5d    %4.3f" % (lb,deadtime[run][lb])

# write XML
if opts.xml:
    maxdeadtime=0.02
    if opts.maxdeadtime>0:
        maxdeadtime=opts.maxdeadtime
    else:
        print "Using default max deadtime = ",maxdeadtime

    f = open(opts.xml,"w")

    # header info
    f.write('<?xml version="1.0"?>\n')
    f.write('<LumiRangeCollection>\n')
    f.write('  <NamedLumiRange>\n')
    f.write('    <Name>Deadtime_below_%4.3f</Name>\n' %(maxdeadtime,))
    f.write('    <LumiBlockCollection>\n')

    for run in runs:
        f.write('      <Run>%d</Run>\n' % (run,))

        # find the ranges with deadtime less than maxdeadtime
        first=0
        for lb in deadtime[run].keys():
            if deadtime[run][lb]>maxdeadtime:
                if lb-1!=first:
                    f.write('      <LBRange Start="%d" End="%d"/>\n' %(first+1,lb-1))
                    print '      <LBRange Start="%d" End="%d"/>' %(first+1,lb-1)
                first=lb
        if lb!=first:
            f.write('      <LBRange Start="%d" End="%d"/>\n' %(first+1,lb))
            print '      <LBRange Start="%d" End="%d"/>' %(first+1,lb)

    # footer
    f.write('      <BunchId>1</BunchId>\n') # currently no bunch-by-bunch info
    f.write('    </LumiBlockCollection>\n')
    f.write('  </NamedLumiRange>\n')
    f.write('</LumiRangeCollection>\n')
