# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Elliot Lipeles, Univ. of Pennsylvania (2011)
# Tae Min Hong, Univ. of Pennsylvania (2011, 2012)
#
# Read Rate from TriggerRatePresenter root files
#
# Results are one rate per lumiblock
#

#print 'Hello world TrigCostTRP.py'

import sys
import os
import re
import optparse
import string
import math
#print '\tLoaded standard packages'

import TrigCostAnalysis     # print '\tLoaded special TrigCostAnalysis package'
#print '\tDone prelim import for TrigCostTRP.py'

#----------------------------------------------------------------------
class TrigCostTRP:
    def __init__(self):
        pass

# for debugging
#import inspect
#print inspect.getframeinfo(inspect.currentframe())[:3]

#----------------------------------------------------------------------
def NonChainTRP(chname):
    return string.count(chname,"_grp_") != 0 \
           or string.count(chname,"_str_") != 0 \
           or string.count(chname,"L1_EMPTY_") != 0\
           or string.count(chname,"_recording") != 0\
           or string.count(chname,"_LB_Num") != 0\
           or string.count(chname,"_L1A") != 0\
           or string.count(chname,"_total") != 0\
           or string.count(chname,"_time") != 0\
           or string.count(chname,"L2_RATE") != 0\
           or string.count(chname,"L2_Providers") != 0\
           or string.count(chname,"EF_RATE") != 0\
           or string.count(chname,"EF_Providers") != 0

#----------------------------------------------------------------------
def GetFileName(runnumber, myafspath='', myhttppath=''):

    # AFS path for TRP ntuples
    afs  = "/afs/cern.ch/user/a/atlasdqm/dqmdisk1/histos/Cosmics08/online"

    path = ''
    if runnumber > 200497: # April 1, 2012
        path = os.path.join("tdaq-04-00-01", "coca/TRP-Rates")
    elif runnumber > 177531: # New TDAQ release on Sunday 13 March 2011
        path = os.path.join("tdaq-03-00-01", "coca/TRP-Rates")
    else:
        path = os.path.join("tdaq-02-00-03", "coca/TRP-Rates")

    # Default is AFS
    file = ''
    if len(myafspath)!=0:
        file = os.path.join(myafspath, "TriggerRates_ATLAS_%d.root" % runnumber)
    else:
        file = os.path.join(afs, path, "TriggerRates_ATLAS_%d.root" % runnumber)

    # If does not exist in AFS, then try HTTP
    if not os.path.exists(file):
        print "No AFS file at %s, now trying web path" % file

        user = os.environ['USER']
        passwd = ''
        if user=='tmhong' or user=='xmon':
            passfile = open('/afs/cern.ch/user/%s/%s/private/dqauth' % (user[0], user))
            passwd = passfile.read().strip()
            passfile.close()
        else:
            passwd = raw_input("%s@lxplus's password: " % user)

        http = "https://%s:%s@atlasdqm.cern.ch/tier0/Cosmics08_online_root" % (user, passwd)

        if len(myhttppath)!=0:
            file = os.path.join(myhttppath, "TriggerRates_ATLAS_%d.root" % runnumber)
        else:
            file = os.path.join(http, path, "TriggerRates_ATLAS_%d.root" % runnumber)

    return file

#----------------------------------------------------------------------
def ReadTRP(runnumber, lb_beg, lb_end, options=[], myafspath='', myhttppath=''):

    # Assume 10s sampling rate (current TRP setting)
    samplingrate = 10

    collection = TrigCostAnalysis.CostResultCollection()
    collection.run = runnumber
    collection.lbbeg = lb_beg
    collection.lbend = lb_end

    file = GetFileName(runnumber, myafspath, myhttppath)

    # Setup reading root file
    try:
        import ROOT                 ; #print '\tLoaded special ROOT package'
        ROOT.gROOT.SetBatch(True)   ; #print '\tROOT set batch mode'
    except:
        print "pyROOT not avaiable: setup the release and then "
        print "export PYTHONPATH=$PYTHONPATH:$ROOTSYS/lib"
        sys.exit(-1)

    prefix = ''
    if runnumber < 178292: # See e-mail thread below
        prefix = 'ISS_TRP.'
    tchains = { 'L1' : ROOT.TChain(prefix+'L1_Rate', 'L1'),
                'L2' : ROOT.TChain(prefix+'L2_Rate', 'L2'),
                'EF' : ROOT.TChain(prefix+'EF_Rate', 'EF') }

    # Open TFiles
    for tc in tchains.values():
        print 'Adding file=%s' % file
        tc.Add(file)

    # Read-in data
    # note prescale reading not used right now
    branches  = {}
    print '... Reading data from '+file
    for lvl in ['L1','L2','EF']:
        print "\nLevel",lvl

        # L1 nomenclature in TRP ntuple
        #   _TBP = Rate before applying L1 prescale rejection
        #   _TAP = Rate after applying L1 prescale rejection
        #   _TAV = Rate after applying L1 algorithmic rejection
        #   _PS  = Prescale factor, maybe?
        #   _DT  = Dead time calculated from above
        #
        # HLT nomenclature in TRP ntuple
        #   _input    = Rate before applying HLT prescale rejection
        #   _prescale = Rate after applying HLT prescale rejection
        #   _raw      = Rate after applying HLT algorithmic rejection
        #   _output   = Same as _raw (except for pass-thru & rerun)
        #   _algoIn   = Rate at which the chain was rerun
        #   _rerun    = In case of rerun or pass-thru, this counts the
        #               number of times the algorithms were really
        #               executed for this chain. (Something like
        #               resource usage)
        #
        if lvl=='L1':
            sfx_in  = 'TBP'
            sfx_ps  = 'TAP'
            sfx_out = 'TAV'
        else:
            sfx_in  = 'input'
            sfx_ps  = 'prescale'
            if runnumber >= 202466: # bug #92673
                sfx_ps = 'prescaled'

            sfx_out = 'output'

        # Historical that this gets special treatment: e.g., 'L1.*_TBP'
        br_out = lvl + '.*_' + sfx_out

        comp = re.compile(br_out)
        tree = tchains[lvl]

        if not tree.GetListOfBranches():
            print "Failed to open TRP file"
            sys.exit(-1)

#	        # tmhong: This method of listing has been replaced by below
#	        # find list of trigger chains (mathing br_out regexp)
#	        branches[lvl] = [b.GetName()[:-(len(br_out)-4)] \
#	                         for b in list(tree.GetListOfBranches())
#	                         if comp.match(b.GetName()) ]

        branches[lvl] = []
        for branch in list(tree.GetListOfBranches()):
            #
            # L1 example:
            #   bname = L1_EM3_TAV
            #   pos   = -4 = -(len(L1.*_TAV)-4)
            #   cname = L1_EM3
            # L2 example:
            #   bname = L2_total_output
            #   pos   = -7 = # -(len(L2.*_output)-4)
            #   cname = L2_total
            #
            if comp.match( branch.GetName() ):
                bname = branch.GetName()
                pos   = -(len(br_out)-4)
                cname = bname[:pos]
                branches[lvl].append( cname )

        # Skip rapidly (by 100s ~ 10s of lbs) to where lb_beg starts
        entries = tree.GetEntries()
        stepsize=100
        startpoint=0
        for startpoint in xrange(0,entries,stepsize):
            tree.GetEvent(startpoint)
            lb = tree.LumiBlock
            if lb >= lb_beg:
                break
        if startpoint>=stepsize:
            startpoint=startpoint-stepsize

        lblast=0 # Last LB number
        count =0 # Number of lb to average
        lbrate=0 # Initialize rate for one lb

        # Average samplings in lumiblock
        for e in xrange(startpoint,entries):
            tree.GetEvent(e)
            lb = tree.LumiBlock

#	            # Debug
#	            print 'DEBUG Event', e,

            if "fast" in options and lb==lblast:
                continue
            if lb < lb_beg:
                continue
            if lb > lb_end and lb_end!=-1:
                print "Done reading ",lvl
                break

            # If new LB, then dump previous LB data
            if lblast>0 and lb!=lblast:
                if lvl=='L1':
                    collection.results[lb] = TrigCostAnalysis.CostResult()
                print lb,",",
                sys.stdout.flush()

            # Loop over branch names
            for bname in branches[lvl]:

#	                # Debug
#	                if bname!='L1_EM3_EMPTY': continue
#	                print 'found bname', bname,

                # Is CPS = "coherent prescale"?
                cpsval=None
                if string.count(bname,"CPS"):
                    cpsbr  = tree.GetBranch(bname+'_'+sfx_out)
                    if string.count(bname,":")==0:
                        leaf   = bname+'_'+sfx_out
                    else:
                        leaf   = string.split(bname,":")[1]+'_'+sfx_out
                    #cpsbr.GetListOfLeaves().Print()
                    cpsval = cpsbr.GetLeaf(leaf).GetValue()
                    #print bname,cpsbr.GetLeaf(leaf),cpsval
                    continue

                # If new LB, then dump previous LB data
                if lb!=lblast:
                    ch = TrigCostAnalysis.CostChain()
                    count = 1
                    ch.SetName(bname)
                    ch.SetLevel(lvl)
                    if cpsval:
                        ch.SetRate(cpsval)
                    else:
                        rate = getattr(tree,bname+'_'+sfx_out)
                        ch.SetRate(rate)
#	                        # Debug
#	                        print bname, 'New lb=', lb, 'lblast=', lblast, 'count=', count, 'rate=', rate

                        # Add all rates, not just output rates
                        if lvl == 'L1':
                            ch.SetPrescale(getattr(tree,bname+'_PS'))

                            # Reverse-engineer counts -- will be approximate
                            ch.SetTBPCnt (getattr(tree,bname+'_TBP')*samplingrate)
                            ch.SetTAPCnt (getattr(tree,bname+'_TAP')*samplingrate)
                            ch.SetTAVCnt (getattr(tree,bname+'_TAV')*samplingrate)

                            # Rates are stored in TRP ntuples
                            ch.SetTBPRate(getattr(tree,bname+'_TBP'))
                            ch.SetTAPRate(getattr(tree,bname+'_TAP'))
#	                        ch.SetTAVRate(getattr(tree,bname+'_TAV')) # Don't uncomment!! Already done in SetRate

                        else:
#	                        ch.SetPrescale(getattr(tree,bname+'_prescale'))


                            # Reverse-engineer counts -- will be approximate
                            ch.SetTBPCnt (getattr(tree,bname+'_'+sfx_in )*samplingrate)
                            ch.SetTAPCnt (getattr(tree,bname+'_'+sfx_ps )*samplingrate)
                            ch.SetTAVCnt (getattr(tree,bname+'_'+sfx_out)*samplingrate)

                            #if getattr(tree,bname+'_output'     )-getattr(tree,bname+'_raw'     ) > 1e-10:
                                #print "not equal",bname,getattr(tree,bname+'_'+sfx_out     ),getattr(tree,bname+'_raw'     )

                            # Rates are stored in TRP ntuples
                            ch.SetTBPRate(getattr(tree,bname+'_'+sfx_in ))
                            ch.SetTAPRate(getattr(tree,bname+'_'+sfx_ps ))

#	                        ch.SetTAVRate(getattr(tree,bname+'_raw'     )) # Don't uncomment!! Already done in SetRate

                    collection.SetCostChain(lb,bname,ch)

                    if collection.lbbeg==-1:
                        collection.lbbeg=lb

                else:
                    # Combine sample previous samples (#samples=count)
                    ch          = collection.GetCostChain(lb,bname)
                    pastrate    = ch.GetRate()
                    cumulative  = ch.GetRate()*count
                    lbrate      = getattr(tree,bname+'_'+sfx_out)
                    avgrate     = (cumulative+lbrate)/(count+1)
                    ch.SetRate(avgrate)

#	                    # Debug
#	                    print 'Old lb=', lb, 'lblast=', lblast, 'count=', count, 'pastrate=', pastrate, 'cum=', cumulative, 'lbrate', lbrate, 'avgrate=', avgrate

                    # err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
                    err2 = ch.GetRate() / ((count+1)*samplingrate)
                    ch.SetRateErr(math.sqrt(err2))

                    if lvl == 'L1':
                        ch.SetPrescale(getattr(tree,bname+'_PS'))

                        # Reverse-engineer counts -- will be approximate
                        ch.SetTBPCnt (getattr(tree,bname+'_TBP')*samplingrate+ch.GetTBPCnt())
                        ch.SetTAPCnt (getattr(tree,bname+'_TAP')*samplingrate+ch.GetTAPCnt())
                        ch.SetTAVCnt (getattr(tree,bname+'_TAV')*samplingrate+ch.GetTAVCnt())

                        # Rates are stored in TRP ntuples
                        ch.SetTBPRate((getattr(tree,bname+'_TBP')*count+ch.GetTBPCnt())/(count+1))
                        ch.SetTAPRate((getattr(tree,bname+'_TAP')*count+ch.GetTAPCnt())/(count+1))
#	                    ch.SetTAVRate((getattr(tree,bname+'_TAV')*count+ch.GetTAVCnt())/(count+1)) # Don't uncomment!! Already done in SetRate

                        # Rate errors
                        ch.SetTBPRateErr(math.sqrt(ch.GetTBPRate() / ((count+1)*samplingrate)))
                        ch.SetTAPRateErr(math.sqrt(ch.GetTAPRate() / ((count+1)*samplingrate)))
#	                    ch.SetTAVRateErr(math.sqrt(ch.GetTAVRate() / ((count+1)*samplingrate))) # Don't uncomment!! Already done in SetRateErr

                    else:
#	                        ch.SetPrescale(getattr(tree,bname+'_prescale'))

                        # Reverse-engineer counts -- will be approximate
                        ch.SetTBPCnt (getattr(tree,bname+'_'+sfx_in )*samplingrate+ch.GetTBPCnt())
                        ch.SetTAPCnt (getattr(tree,bname+'_'+sfx_ps )*samplingrate+ch.GetTAPCnt())
                        ch.SetTAVCnt (getattr(tree,bname+'_'+sfx_out)*samplingrate+ch.GetTAVCnt())

                        # Rate are stored in TRP ntuples
                        ch.SetTBPRate((getattr(tree,bname+'_'+sfx_in )*count+ch.GetTBPCnt())/(count+1))
                        ch.SetTAPRate((getattr(tree,bname+'_'+sfx_ps )*count+ch.GetTAPCnt())/(count+1))
#                       ch.SetTAVRate((getattr(tree,bname+'_'+sfx_out)*count+ch.GetTAVCnt())/(count+1)) # Don't uncomment!! Already done in SetRate

                        # Rate errors
                        ch.SetTBPRateErr(math.sqrt(ch.GetTBPRate() / ((count+1)*samplingrate)))
                        ch.SetTAPRateErr(math.sqrt(ch.GetTAPRate() / ((count+1)*samplingrate)))
#                       ch.SetTAVRateErr(math.sqrt(ch.GetTAVRate() / ((count+1)*samplingrate))) # Don't uncomment!! Already done in SetRateErr

#                    print 'ch.GetRate()', bname, ch.GetRate()
#                    print 'ch.GetTBPCnt()', bname, ch.GetTBPCnt()
#                    print 'ch.GetTAPCnt()', bname, ch.GetTAPCnt()
#                    print 'ch.GetTAVCnt()', bname, ch.GetTAVCnt()
#                    print 'ch.GetTBPRate()', bname, ch.GetTBPRate()
#                    print 'ch.GetTAPRate()', bname, ch.GetTAPRate()
#                    print 'ch.GetTAVRate()', bname, ch.GetTAVRate()
#
            # Increment
            count += 1
            lblast = lb

        # record end of range read
        collection.lbend=lblast

    print
    return collection


#----------------------------------------------------------------------
# E-mail thread for the Naming change after 178292
#
#	On 04/15/2011 Antonio Sidoti wrote to Tomasz, Ivana, Francesca, Tae
#	    Yeah sorry for that, this was not really something I wanted to
#	    change but it is less cumbersome....  So the change (for run
#	    in ATLAS partitions) is for run >=178292 (runs took after the
#	    28/03/2011 morning)
#
#	On 04/15/2011 02:17 PM, Tomasz Bold wrote:
#	    Hi Antonio, Uff. Do you know from which run we should change?
#
#   On 15 April 2011 14:06, Antonio Sidoti wrote:
#	Dear TRP-Rates power users,
#	    I've just discovered that when we've installed the new version
#	    of the archiver just before the technical shutdown, the suffix
#	    ISS_TRP was dropped from the TTree name containing the rates
#	    of the TimePoint.  So for example, the name of the TTree that
#	    was containing the L1 rates (from CTP) passed from
#	    "ISS_TRP.L1_Rate" to "L1_Rate". This was not foreseen but I
#	    think it is less cumbersome as it is now.  So please update
#	    your scripts.

#eof
