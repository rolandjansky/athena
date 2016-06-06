#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Elliot Lipeles, Univ. of Pennsylvania (2010, 2011)
# Tae Min Hong, Univ. of Pennsylvania (2011, 2012)
# Contributors:
#   Monica Dunford, CERN (2011)
#   Brian Petersen, CERN (2011)
#
# You can use four different sources of data to get rates.  People in
# the parentheses are the contact people for that piece of code.
#
#   1. COOL database (lipeles, tmhong)
#   2. TRP ntuples (lipeles, tmhong)
#   3. XML files (lipeles)
#   4. IS server (aagaard)
# 
# You might be interested in makeRoot.py: outputs to a ROOT file.
# 
# Examples:
"""
# Get Luminosity for given lumiblock range:
python exampleCost.py --cool --dolumi --run 156682 --lb-beg=450 --lb-end=514

# Dump Rates from cool for given lumiblock range:
python exampleCost.py --cool --dorates --run 156682 --lb-beg=450 --lb-end=514 --writexml output.xml

# Dump Rates from trp for given lumiblock range:
#    use --fast to speed it up uses only one sample per
#    lumiblock rather than averaging all samples
python exampleCost.py --trp --dorates --run 156682 --lb-beg=450 --lb-end=514 --writexml output.xml

# HLT was added to COOL around Nov 2010, so try 
python exampleCost.py --cool --dolumi --dorates --run 170482 --lb-beg=1 --lb-end=5 --printRates --quiet

# Bunch group info added.  It changes for enhanced bias run 178044 between lb 52-53.
python exampleCost.py --cool --dolumi --run 178044 --lb-beg=50 --lb-end=55
"""

#print 'Hello world exampleCost.py'
import sys
#cmd=' '.join(sys.argv)
#print cmd

# Parse command-line arguments
import optparse
preamble='usage: %prog [options]'
p = optparse.OptionParser(usage=preamble, add_help_option=False, version='%prog')

# Generic options
p.add_option('-?','--usage',        action='store_true', default=False,  dest='help',      help='show this message and exit         [default=%default]')
p.add_option('-h','--help',         action='store_true', default=False,  dest='help',      help='show this message and exit         [default=%default]')
p.add_option('-v', '--verbose',     action="store_true", default=False,  help="Run in verbose mode")
p.add_option('-d', '--debug',       action="store_true", default=False,  help="Run in debug mode")
p.add_option('-q', '--quiet',       action="store_true", default=False,  help="Run in quiet mode (suppresses chains with zero rates)")
p.add_option('-t', '--trp',         action="store_true", default=False,  help="Get data from TRP requires option, --run")
p.add_option('-c', '--cool',        action="store_true", default=False,  help="Get data from COOL requires option, --run")
p.add_option('-x', '--xml',         action="store_true", default=False,  help="Get data from an XML file requires option, --file")
p.add_option('-i', '--is',          action="store_true", default=False,  dest="IS",help="Get data from IS mirror requires option, --time-beg")
p.add_option('-w', '--writexml',    type="string",       default="",     help="Write an XML file to location given ")
p.add_option('','--nopsrangebreak', action="store_true", default=False,  help="Average over specified lb range even if PS change") 
p.add_option('-r', '--run',         type="int",          default="0",    dest="run",     help="Run Number")
p.add_option('-f', '--file',        type="string",       default="",     dest="file",    help="XML file name")
p.add_option('',   '--lb-beg',      type="int",          default=-1,     dest="lbbeg",   help="Starting LB")
p.add_option('',   '--lb-end',      type="int",          default=-1,     dest="lbend",   help="Ending LB")
p.add_option('',   '--dolumi',      action="store_true", default=False,  help="Get lumi info from COOL") 
p.add_option('',   '--doconfig',    action="store_true", default=False,  help="Get config info from COOL") 
p.add_option('',   '--dorates',     action="store_true", default=False,  help="Get rate info from COOL") 
p.add_option('',   '--printRates',  action="store_true", default=False,  help="Print rates from all levels") 
p.add_option('',   '--printLumi',   action="store_true", default=False,  help="Print lumi") 
p.add_option('',   '--printTBP',    action="store_true", default=False,  help="Print TBP, TAP, and TAV rates")
p.add_option('',   '--test',        action="store_true", default=False,  help="Test run with --cool --dolumi --dorates --run 170482 --lb-beg=1 --lb-end=5 --printRates --quiet")

# COOL specific
p.add_option('',  '--PSHistoryKeys',action="store_true", default=False,  help="Print history for a trigger, also needs --run <run> and --cool --doconfig")
p.add_option('',  '--PSHistoryTrg', type="string",       default="",     help="Print PS history for a trigger in a run, takes argument of trigger, also needs --run <run> and --cool --doconfig")
p.add_option('',  '--dumpCounters', action="store_true", default=False,  help="Print chain names and counters")

# TRP specific
p.add_option('',   '--fast',        action="store_true", default=False,  help="for TRP: one sample per lumi block for (~12 times faster)")

# IS specific
p.add_option('',   '--time-beg',    type="str",          default='',     dest="timebeg",   help="Starting Time")
p.add_option('',   '--time-end',    type="str",          default='',     dest="timeend",   help="Ending Time")

# opts is from above, args are the arguments without a flag
(opts, args) = p.parse_args()

# Help
if opts.help:
    p.print_help()
    sys.exit(1)

# Test mode
if opts.test:
    opts.cool = True
    opts.dolumi = True
    opts.dorates = True
    opts.run = 170482
    opts.lbbeg = 1
    opts.lbend = 5
    opts.printRates = True
    opts.printLumi = True
    opts.quiet = True

if not (opts.trp or opts.xml or opts.cool or opts.IS):
    print 'You must specify either TRP, XML, COOL or IS'
    sys.exit(2)

if (opts.xml and opts.file ==""): 
    print 'XML needs valid file name'
    sys.exit(2)

#
# Import packages after parsing options
#

import copy
import string
import math
from TrigCostPython import TrigCostAnalysis,TrigCostTRP,TrigCostCool,TrigCostXML #,TrigCostIS #<- is broken

##############################################################################
# Read inputs
##############################################################################

ratecoll=None
lbset=None
config=None
rates=None

options=[""]
if opts.fast:  options.append('fast')
if opts.debug: options.append('debug')
if opts.quiet: options.append('quiet')

# Need to use COOL to get bunch information
if opts.dolumi:
    print "Getting lumi info"
    lbset = TrigCostCool.GetLumiblocks(opts.run,opts.lbbeg,opts.lbend,options)

# INPUT OPTION 1: TRP
if opts.trp:
    if opts.run < 0: 
        print 'TRP needs valid run number'
        sys.exit(2)

    ratecoll = TrigCostTRP.ReadTRP(opts.run,opts.lbbeg,opts.lbend,options)

# INPUT OPTION 2: COOL    
if opts.cool:
    if opts.run < 0: 
        print 'COOL needs valid run number'
        sys.exit(2)

    if opts.doconfig: config   = TrigCostCool.GetConfig(opts.run,options)
    if opts.dorates:  ratecoll = TrigCostCool.GetRates(opts.run,opts.lbbeg,opts.lbend,options)

# INPUT OPTION 3: XML 
if opts.xml:
    if len(opts.file) == 0: 
        print 'XML needs valid input file'
        sys.exit(2)

    rates = TrigCostXML.ReadXmlFile(opts.file)

# INPUT OPTION 4: IS -- Information Service only runs in a release for AtlasCAFHLT or AtlasP1HLT.
if opts.IS:
    if not opts.timebeg:
        print 'IS option needs at least a time start of form "hh:mm"'
        sys.exit(2)

    collections=TrigCostIS.ReadISRates(opts.timebeg,opts.timeend) #for now we always look at mirror partition (default)
    if not collections:
        print 'No data found in the specified interval [%s to %s]' % (opts.timebeg,opts.timeend)
        sys.exit(2)
    runs=collections.keys()
    runs.sort()
    if not opts.run:
        if len(runs)>1:
            print 'Warning more than one run found - using latest (%d)' % runs[-1]
        opts.run=runs[-1]
    if not opts.run in runs:
        print 'Did not find run %d in specified time interval' % (opts.run)
        print 'Available: '+(', '.join([str(run) for run in runs]))
        sys.exit(2)
    ratecoll=collections[opts.run]
    if opts.lbbeg==-1:
        opts.lbbeg=ratecoll.lbbeg

##############################################################################
# Write to screen
##############################################################################

if ratecoll and opts.printRates:
    print
    print "LB Range("+str(ratecoll.lbbeg)+","+str(ratecoll.lbend)+")"
    print

    for lvl in ['L1','HLT']:
        print "-------------------------------------------------------------------------"
        print "%-45s %4s" % ("Rates for %s" % lvl, "LB="),
        for lb in ratecoll.LBRange():
            print "% 7d" % lb,

        print
        print "-------------------------------------------------------------------------"
        for chname in ratecoll.GetChainNames(lvl):

            # Do not print if all zero...
            allzero = 'quiet' in options
            for lb in ratecoll.LBRange():
                ch = ratecoll.GetCostChain(lb,chname)
                if ch:
                    allzero = allzero and ch.GetRate()==0

            if allzero:
                continue

            print "%-50s" % (chname,),
            for lb in ratecoll.LBRange():
                ch = ratecoll.GetCostChain(lb,chname)
                if ch:
                    print "% 7.2g" % (ch.GetRate(),),
                else:
                    print "% 7s" % ('NA',),

            print

        print "Runtime",
        if 'quiet' in options:
            print "info: You are running in --quiet mode, which suppresses all zero-rate chains."
        else:
            print "hint: Run with --quiet mode to suppress the zero-rate chains"

if (ratecoll and opts.printRates) or (lbset and opts.printLumi):
    if lbset==None:
        print "Runtime hint: Run with --dolumi mode to see luminosity info"
        sys.exit(-1)
    lbrange = xrange(lbset.lbbeg,lbset.lbend)
    if ratecoll:
        lbrange = ratecoll.LBRange()

    print "-------------------------------------------------------------------------"
    print "%-45s %4s" % ("Beam conditions", "LB="),
    for lb in lbrange:
        print "% 7d" % lb,

    print
    print "-------------------------------------------------------------------------"
    print "%-50s" % "# colliding bunches",
    for lb in lbrange:
        print "% 7d" % lbset.GetBunches(lb),

    print
    print "%-50s" % "# beam1 bunch",
    for lb in lbrange:
        print "% 7d" % lbset.GetNbunch1(lb),

    print
    print "%-50s" % "# beam2 bunch",
    for lb in lbrange:
        print "% 7d" % lbset.GetNbunch2(lb),

    print
    print "%-50s" % "Fill number",
    for lb in lbrange:
        print "% 7d" % lbset.GetFillNumber(lb),

    print
    print "%-50s" % "Mean Pileup",
    for lb in lbrange:
        print "% 7.2f" % lbset.GetMeanPileUp(lb),

    print
    print "%-50s" % "Stable beams?",
    for lb in lbrange:
        print "% 7s" % lbset.GetStableBeams(lb),

    print
    print "%-50s" % "Beam energy (GeV)",
    for lb in lbrange:
        print "% 7.1f" % lbset.GetBeamEnergyGeV(lb),

    print
    print "%-50s" % "LHC mode (abbreviated)",
    for lb in lbrange:
        print "% 7s" % lbset.GetLHCMode(lb).split(' ')[0],

    print
    print "%-50s" % "Beam mode (abbreviated)",
    for lb in lbrange:
        print "% 7s" % lbset.GetBeamMode(lb).split(' ')[0],

    for i in xrange(0,7):
        name = ''
        if   i==0: name = lbset.lbs[lbrange[0]].bgName0
        elif i==1: name = lbset.lbs[lbrange[0]].bgName1
        elif i==2: name = lbset.lbs[lbrange[0]].bgName2
        elif i==3: name = lbset.lbs[lbrange[0]].bgName3
        elif i==4: name = lbset.lbs[lbrange[0]].bgName4
        elif i==5: name = lbset.lbs[lbrange[0]].bgName5
        elif i==6: name = lbset.lbs[lbrange[0]].bgName6
        elif i==7: name = lbset.lbs[lbrange[0]].bgName7
        print
        print "%-10s" % ("BG%d length" % i),
        print "%-39s" % ("("+name+")"),
        for lb in lbrange:
            if   i==0: print "% 7s" % lbset.lbs[lb].bgLength0,
            elif i==1: print "% 7s" % lbset.lbs[lb].bgLength1,
            elif i==2: print "% 7s" % lbset.lbs[lb].bgLength2,
            elif i==3: print "% 7s" % lbset.lbs[lb].bgLength3,
            elif i==4: print "% 7s" % lbset.lbs[lb].bgLength4,
            elif i==5: print "% 7s" % lbset.lbs[lb].bgLength5,
            elif i==6: print "% 7s" % lbset.lbs[lb].bgLength6,
            elif i==7: print "% 7s" % lbset.lbs[lb].bgLength7,

    for i in xrange(0,7):
        print
        print "%-50s" % ("BG%d list of BCID starts with" % i),
        for lb in lbrange:
            bglist = []
            if   i==0: bglist = lbset.lbs[lb].bgBCIDList0
            elif i==1: bglist = lbset.lbs[lb].bgBCIDList1
            elif i==2: bglist = lbset.lbs[lb].bgBCIDList2
            elif i==3: bglist = lbset.lbs[lb].bgBCIDList3
            elif i==4: bglist = lbset.lbs[lb].bgBCIDList4
            elif i==5: bglist = lbset.lbs[lb].bgBCIDList5
            elif i==6: bglist = lbset.lbs[lb].bgBCIDList6
            elif i==7: bglist = lbset.lbs[lb].bgBCIDList7
            nbglist = len(bglist)
            bgitem = 'None'
            if nbglist>0:
                bgitem = str(bglist[0])

            print "% 7s" % ("%s%s" % (bgitem, '...' if nbglist>0 else '')),

    print

if opts.printTBP:
    print "here",ratecoll

if ratecoll and opts.printTBP:
    print
    print "LB Range("+str(ratecoll.lbbeg)+","+str(ratecoll.lbend)+")"
    print

    print "%-50s   %-10s   %-10s   %-10s   %-10s" % ("Chain","PS","TBP","TAP","TAV")
    
    for chname in ratecoll.GetChainNames('L1'):
        lb=ratecoll.FirstLB()
        ch = ratecoll.GetCostChain(lb,chname)
        if ch:
            print "%-50s   % 10.0g  % 10.2g   % 10.2g   % 10.2g" % (chname,
                                                                    ch.GetPrescale(),
                                                                    ch.GetTBPRate(),
                                                                    ch.GetTAPRate(),
                                                                    ch.GetTAVRate())
        else:
            print "%-50s   % 10s  % 10s   % 10s   % 10s" % (chname, 'NA', 'NA', 'NA', 'NA')
    print

if rates:
    print
    print "XML Rates"
    print

    for lvl in ['L1','HLT']:
        print "-------------------------------------------------------------------------"
        print "Rates for ",lvl
        print "-------------------------------------------------------------------------"
        for chname in rates.GetChainNames(lvl):
            print "%-50s" % (chname,),
            print "% 7.2g" % (rates.GetChain(chname).GetRate(),)

if lbset:
    print "-------------------------------------------------------------------------"
    print "Lumiblock Info("+str(lbset.lbbeg)+","+str(lbset.lbend)+")"
    print "-------------------------------------------------------------------------"
    print
    print "%-40s % 9.5g %-10s" % ("Integrated Time:      ",lbset.GetTotalTime(),"seconds")
    print "%-40s % 9.5g %-10s" % ("Live Time:            ",lbset.GetLiveTime(),"seconds")
    print "%-40s % 9.2g %-10s" % ("Delivered Luminosity: ",lbset.GetDeliveredLumi(),"ub^{-1}")
    print "%-40s % 9.2g %-10s" % ("Recorded Luminosity:  ",lbset.GetRecordedLumi(),"ub^{-1}")
    print "%-39s % 9.3e %-10s" % ("Mean Instantaneous Luminosity:",lbset.GetDeliveredLumi()/lbset.GetTotalTime()*1e30,"1/(cm^2 s)")
    print "%-40s % 9.2e %-10s" % ("Live-Time-Averaged # Bunches:",lbset.GetLiveTimeAveragedBunches(),"")
    print "%-40s % 9.2e %-10s" % ("Integrated-Time-Averaged # Bunches:",lbset.GetIntegratedTimeAveragedBunches(),"")

    #for lb in lbset.LBRange():
    #    print lbset.GetLB(lb).AsString()

##############################################################################
# Write output XML
##############################################################################
if opts.writexml!="":
    if rates:
        TrigCostXML.WriteXmlFile(opts.writexml,rates,lbset)
    elif ratecoll:
        # get cool config
        config= TrigCostCool.GetConfig(opts.run)
        
        # make one output for each range with constant lbset
        for psrange in config.GetPrescaleRanges():
            lbbeg = max(psrange.FirstLB(),opts.lbbeg,ratecoll.FirstLB())

            if opts.lbend==-1:
                opts.lbend=ratecoll.LastLB()
            lbend = min(psrange.LastLB(),opts.lbend,ratecoll.LastLB())

            if opts.nopsrangebreak:
                lbbeg=max(ratecoll.FirstLB(),opts.lbbeg)
                lbend=min(opts.lbend,ratecoll.LastLB())
                
            length= lbend-lbbeg+1

            if length <= 0:
                #print psrange.FirstLB(),opts.lbbeg,psrange.LastLB(),opts.lbend
                continue

            avgrates = TrigCostAnalysis.CostResult()
            for chname in ratecoll.GetChainNames():
                avgch = copy.deepcopy(ratecoll.GetCostChain(lbbeg,chname))

                # average lumiblocks
                combrules = {
                    'sum' : ['tavcnt','tapcnt','tbpcnt','tavover','tapover','tbpover'],
                    'avg' : ['rate','taprate','tbprate','nevt'],
                    'quad': ['rateerr','taprateerr','tbprateerr','nevterr']
                    }
                for (rule,list) in combrules.items():
                    for attr in list:
                        comb = 0
                        if hasattr(avgch,attr):
                            for lb in range(lbbeg,lbend+1):
                                ch = ratecoll.GetCostChain(lb,chname)
                                if not ch or not hasattr(ch,attr):
                                    continue
                                if rule=='sum':
                                    comb+=getattr(ch,attr)
                                elif rule=='avg':
                                    comb+=getattr(ch,attr)/float(length)
                                elif rule=='quad':
                                    comb+=(getattr(ch,attr)/float(length))**2
                            if rule=='quad':
                                setattr(avgch,attr,math.sqrt(comb))
                            else:
                                setattr(avgch,attr,comb)

                # add prescales and passthroughs...
                # skip groups, streams, unused CTPs, "recording" info, time , ...
                # first set all PS to -1000 as a not valid flag
                avgch.SetPrescale(-1000)
                if not TrigCostTRP.NonChainTRP(chname) and not opts.nopsrangebreak:

                    try:
                        if avgch.GetLevel() == 'L1':
                            avgch.SetPrescale(config.L1Prescales[psrange.l1key][chname].ps)
                            print "Set PS: ",chname," PS=",config.L1Prescales[psrange.l1key][chname].ps
                        elif avgch.GetLevel() == 'HLT':
                            avgch.SetPrescale(config.HLTPrescales[psrange.hltkey][chname].ps)
                            print "Set PS: ",chname," PS=",config.HLTPrescales[psrange.hltkey][chname].ps
                            avgch.SetPassthrough(0) #config.HLTPrescales[psrange.hltkey][chname].pt)
                            avgch.SetRerun(config.HLTPrescales[psrange.hltkey][chname].rerun)
                    except:
                        print "Warning: Unable to set prescales for ",chname, " LVL:",  avgch.GetLevel(), " L1PSK:" ,psrange.l1key
                        
                # add lower chain info
                # skip groups, streams, unused CTPs, "recording" info, time , ...
                if not TrigCostTRP.NonChainTRP(chname):

                    try:
                        if avgch.GetLevel() == 'HLT':
                            avgch.SetLowerChain(config.ChainName2HLTChain[chname].LowerChainName)
                    except:
                        print "Warning: Unable to set lower chain for ",chname

                                                                                         
                avgrates.SetChain(chname,avgch)

            # write out results
            outname = opts.writexml
            outname = string.replace(outname,".xml",
                                     "_"+str(opts.run)+"_"+str(lbbeg)+"_"+str(lbend)+".xml")
            print "Writing: ",outname
            TrigCostXML.WriteXmlFile(outname,avgrates,lbset) # Don't remove PS
            print "Writing equivilant cost folder: ",
            TrigCostXML.WriteCsvFile(str(opts.run), str(lbbeg)+"-"+str(lbend), avgrates, lbset, outname) # Don't remove PS

            outname = outname.replace(".xml","_nops.xml")
            print "Writing: ",outname
            TrigCostXML.WriteXmlFile(outname,avgrates,lbset,True) # Remove PS
            print "Writing equivilant cost folder: ",
            TrigCostXML.WriteCsvFile(str(opts.run), str(lbbeg)+"-"+str(lbend), avgrates, lbset, outname, True) # Remove PS


            # if averaging all psset into one file we are done in one pass
            if opts.nopsrangebreak:
                break
    else:
        print "No rates to write to XML"

##############################################################################
# Write to screen
##############################################################################

# print PS history for trigger
if opts.PSHistoryTrg!="":
    trgs=string.split(opts.PSHistoryTrg,",")
    print "Prescale history for ",
    form="LB=(%4d,%4d)"
    for trg in trgs:
        print "%20s" % (trg,)
        form +=  "   %10.1f   "
    if opts.dolumi:
        form +=  " %6.0f  "
    for psr in config.GetPrescaleRanges():
        lastLB=psr.LastLB()
        if lastLB>9999:
            lastLB=9999
        pars=[psr.FirstLB(),lastLB]
        for trg in trgs:
            if string.count(trg,"L1"):
                pars.append(config.L1Prescales[psr.l1key][trg].ps)
            elif string.count(trg,"HLT"):
                pars.append(config.HLTPrescales[psr.hltkey][trg].ps)
            else:
                print "Trigger not found:",trg
                sys.exit(-1)
        if opts.dolumi:
            pars.append(lbset.GetLB(psr.FirstLB()).avr_lumi)
        print form % tuple(pars)

# print PS history of keys
if opts.PSHistoryKeys:
    print "Prescale history"
    for psr in config.GetPrescaleRanges():
        lastLB=psr.LastLB()
        if lastLB>9999:
            lastLB=9999
        print "LB=(%4d,%4d) L1Key=%4d HLTKey=%4d " % (psr.FirstLB(),lastLB,psr.l1key,psr.hltkey)

# dump counters
if opts.dumpCounters:
    if not config:
        print "Need to run --doconfig to get counters"

    print "------ L1 -----"
    for name,ctpid in config.L1ChainName2CtpId.items():
        print "%-40s %6d" % (name,ctpid)


    print "------ HLT -----"
    for name,ch in config.ChainName2HLTChain.items():
        print "%-40s %6d" % (name,ch.ChainCounter)

#eof
