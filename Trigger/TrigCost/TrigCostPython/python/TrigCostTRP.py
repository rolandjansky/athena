# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Elliot Lipeles, Univ. of Pennsylvania (2011)
# Tae Min Hong, Univ. of Pennsylvania (2011, 2012)
#
# Read Rate from TriggerRatePresenter root files
#
# Results are one rate per lumiblock
#

import sys
import os
import re
import optparse
import string
import math

import TrigCostAnalysis

# Assume 10s sampling rate (current TRP setting)
samplingrate = 10

#----------------------------------------------------------------------
class TrigCostTRP:
    def __init__(self):
        pass

# for debugging
#import inspect
#print inspect.getframeinfo(inspect.currentframe())[:3]

#----------------------------------------------------------------------
# Note: prescale reading not used right now
#
def ReadTRP(runnumber, lb_beg, lb_end, options=[], myafspath='', myhttppath='', levels='L1,HLT'):

    # Return object
    collection = TrigCostAnalysis.CostResultCollection()
    collection.run   = runnumber
    collection.lbbeg = lb_beg
    collection.lbend = lb_end

    # Retrieve data
    filename = GetFileName(runnumber, myafspath, myhttppath)
    tchains  = GetTChains(runnumber, filename)

    for lvl in levels.split(','):

        print '\nLevel',lvl

        # Retrieve lvl-dependent quantities
        sfx_in, sfx_ps, sfx_out = GetSuffixes(lvl, runnumber)
        tree          = tchains[lvl]                      # Get tree
        branches      = GetBranches(tree, lvl, sfx_out)   # Get branches

        # Counters for this tree
        entries       = tree.GetEntries()
        startpoint    = GetStartpoint(tree, lb_beg, entries)
        lblast        = 0                                 # Last LB number
        count         = 0                                 # Number of lb to average
        #print  ' tree name', tree.GetName(), ' Nebntries=', entries
        # Average samplings in lumiblock
        for i in xrange(startpoint, entries):

            tree.GetEvent(i)
            lb = tree.LumiBlock

            # Append to collection, if necessary
            if StopLoopOrInstantiate(lb, lb_beg, lb_end, lblast, lvl, collection, options):
                break

            # Loop over branch names
            for bname in branches:
                #print ' Branch', bname
                ProcessBranch(tree, lb, lblast, bname, lvl, count, sfx_in, sfx_ps, sfx_out, collection)

            # Increment
            count += 1
            lblast = lb

        # Record end of range read
        collection.lbend=lblast

    print
    return collection

#----------------------------------------------------------------------
def GetFileName(runnumber, myafspath='', myhttppath=''):

#    # AFS path for TRP ntuples
#    afs  = "/afs/cern.ch/user/a/atlasdqm/dqmdisk1/histos/Cosmics08/online"
#
#    path = ''
#    if   runnumber>200497: path = os.path.join("tdaq-04-00-01", "coca/TRP-Rates") # April 1, 2012
#    elif runnumber>177531: path = os.path.join("tdaq-03-00-01", "coca/TRP-Rates") # March 13, 2011
#    else:                  path = os.path.join("tdaq-02-00-03", "coca/TRP-Rates")
#
#    # Default is AFS
#    filename = ''
#    if len(myafspath)!=0: filename = os.path.join(myafspath, "TriggerRates_ATLAS_%d.root" % runnumber)
#    else:                 filename = os.path.join(afs, path, "TriggerRates_ATLAS_%d.root" % runnumber)
#
#    # If does not exist in AFS, then try HTTP
#    if not os.path.exists(filename):
#        print "No AFS file at %s, now trying web path" % filename
#
#        passwd = ''
#        user   = os.environ['USER']
#        if user=='tmhong' or user=='xmon':
#            passfile = open('/afs/cern.ch/user/%s/%s/private/dqauth' % (user[0], user))
#            passwd = passfile.read().strip()
#            passfile.close()
#        else:
#            passwd = raw_input("%s@lxplus's password: " % user)
#
#        http = "https://%s:%s@atlasdqm.cern.ch/tier0/Cosmics08_online_root" % (user, passwd)
#        if len(myhttppath)!=0: filename = os.path.join(myhttppath, "TriggerRates_ATLAS_%d.root" % runnumber)
#        else:                  filename = os.path.join(http, path, "TriggerRates_ATLAS_%d.root" % runnumber)
#
#    print '... Reading data from '+filename

    filename = "TriggerRates_ATLAS_%d.root" % runnumber

    return filename

#----------------------------------------------------------------------
def GetTChains(runnumber, filename):

    # Setup reading root file
    try:
        import ROOT                 ; #print '\tLoaded special ROOT package'
        import subprocess
        #import mda                  ; #print '\tLoaded mda'
        ROOT.gROOT.SetBatch(True)   ; #print '\tROOT set batch mode'
    except:
        print "pyROOT not avaiable: setup the release and then "
        print "export PYTHONPATH=$PYTHONPATH:$ROOTSYS/lib"
        sys.exit(-1)

    subprocess.call(['/bin/sh', 'getfile.sh', filename])

    # Name change --- See e-mail thread below
    prefix = ''
    if runnumber < 178292:
        prefix = 'ISS_TRP.'

    tchains = { 'L1' : ROOT.TChain(prefix+'L1_Rate', 'L1'),
                'HLT' : ROOT.TChain(prefix+'HLT_Rate', 'HLT') }

    # Open TFiles
    for tc in tchains.values():
        print 'Adding tfile=%s' % filename
#        tc.Add(filename)
#        tc.Add(mda.FileRead().openFile(filename).GetEndpointUrl().GetUrl())
        tc.Add(filename)

    return tchains

#----------------------------------------------------------------------
# Skip rapidly (by 100s ~ 10s of lbs) to where lb_beg starts
#
def GetStartpoint(tree, lb_beg, entries):

    startpoint = 0
    stepsize = 1
    print "Find TRP start point"

    for startpoint in xrange(0,entries,stepsize):
        tree.GetEvent(startpoint)
        lb = tree.LumiBlock
        if lb >= lb_beg:
            break
    print "Found TRP start point"

    if startpoint>=stepsize:
        startpoint=startpoint-stepsize

    return startpoint

#----------------------------------------------------------------------
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
def GetSuffixes(lvl, runnumber):

    sfx_in  = 'TBP'
    sfx_ps  = 'TAP'
    sfx_out = 'TAV'

    if lvl!='L1':
        sfx_out = 'output'
        sfx_in  = 'input'
        sfx_ps  = 'prescale'
        if runnumber >= 202466:
            sfx_ps = 'prescaled' # Name change -- bug #92673

    return sfx_in, sfx_ps, sfx_out


#----------------------------------------------------------------------
# tmhong: This method of listing has been replaced by below
# find list of trigger chains (mathing br_out regexp)
#
# branches[lvl] = [b.GetName()[:-(len(br_out)-4)] \
#                  for b in list(tree.GetListOfBranches())
#                  if comp.match(b.GetName()) ]
#
def GetBranches(tree, lvl, sfx_out):

    # Return object
    branches = []

    # Exit
    if not tree.GetListOfBranches():
        print "Failed to open TRP file"
        sys.exit(-1)

    # Historical that this gets special treatment: e.g., 'L1.*_TBP'
    br_out = lvl + '.*_' + sfx_out

    # Regex pattern
    comp = re.compile(br_out)

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
            if "HLT" in bname :
                pos   = -(len(br_out)-5)
            else :
                pos   = -(len(br_out)-4)

            cname = bname[:pos]
            branches.append( cname )

    return branches

#----------------------------------------------------------------------
# Return True to break out of event loop
#
def StopLoopOrInstantiate(lb, lb_beg, lb_end, lblast, lvl, collection, options):

#	    if lb>20: return True # TODO

    if 'fast' in options and lb==lblast:
        return False

    if lb < lb_beg:
        return False

    # This breaks loop
    if lb > lb_end and lb_end!=-1:
        print "Done reading ",lvl
        return True

    if not (lblast>0 and lb!=lblast):
        return False

    # If new LB, then dump previous LB data
    if lvl=='L1':
        collection.results[lb] = TrigCostAnalysis.CostResult()

    print lb,",",
    sys.stdout.flush()

    return False

#----------------------------------------------------------------------
def ProcessBranch(tree, lb, lblast, bname, lvl, count, sfx_in, sfx_ps, sfx_out, collection):

#	    if bname!='L2_e24vh_medium1': return # TODO
#	    if lb==5: print bname, # TODO

    # Is CPS = "coherent prescale"?
    cpsval=None
    if string.count(bname,"CPS"):
        cpsval = IsCPS(bname, tree, sfx_out)
        return

    # Set rate, counts, errors
    if lb!=lblast: ch = SetNewBranch(tree, lb, bname, lvl, count, sfx_in, sfx_ps, sfx_out, cpsval, collection)
    else:          ch = SetOldBranch(tree, lb, bname, lvl, count, sfx_in, sfx_ps, sfx_out, collection)

    # Print
    #PrintChain(bname, ch)
    return

#----------------------------------------------------------------------
def SetNewBranch(tree, lb, bname, lvl, count, sfx_in, sfx_ps, sfx_out, cpsval, collection):

    # If new LB, then dump previous LB data
    ch = TrigCostAnalysis.CostChain()
    count = 1
    ch.SetName(bname)
    ch.SetLevel(lvl)

    if cpsval:
        ch.SetRate(cpsval)
    else:
        rate = getattr(tree,bname+'_'+sfx_out)
        ch.SetRate(rate)
#	        # Debug
#	        print bname, 'New lb=', lb, 'lblast=', lblast, 'count=', count, 'rate=', rate

        # Add all rates, not just output rates
        if lvl=='L1': SetNewChainL1 (tree, ch, bname)
        else:         SetNewChainHLT(tree, ch, bname, sfx_in, sfx_ps, sfx_out)

    collection.SetCostChain(lb,bname,ch)
    if collection.lbbeg==-1:
        collection.lbbeg=lb

    return ch

#----------------------------------------------------------------------
def SetOldBranch(tree, lb, bname, lvl, count, sfx_in, sfx_ps, sfx_out, collection):
    # Combine sample previous samples (#samples=count)
    ch = GetChain(tree, bname, lb, count, sfx_out, collection)

    if lvl=='L1': SetOldChainL1 (tree, ch, bname, count)
    else:         SetOldChainHLT(tree, ch, bname, count, sfx_in, sfx_ps, sfx_out)

    return ch

#----------------------------------------------------------------------
def PrintChain(bname, ch):
#	    print 'ch.GetRate()', bname, ch.GetRate()
#	    print 'ch.GetTBPCnt()', bname, ch.GetTBPCnt()
#	    print 'ch.GetTAPCnt()', bname, ch.GetTAPCnt()
#	    print 'ch.GetTAVCnt()', bname, ch.GetTAVCnt()
#	    print 'ch.GetTBPRate()', bname, ch.GetTBPRate()
#	    print 'ch.GetTAPRate()', bname, ch.GetTAPRate()
    print 'ch.GetTAVRate()', bname, ch.GetTAVRate()
    return

#----------------------------------------------------------------------
def IsCPS(bname, tree, sfx_out):
    cpsbr  = tree.GetBranch(bname+'_'+sfx_out)
    if string.count(bname,":")==0:
        leaf   = bname+'_'+sfx_out
    else:
        leaf   = string.split(bname,":")[1]+'_'+sfx_out
#       cpsbr.GetListOfLeaves().Print()
        cpsval = cpsbr.GetLeaf(leaf).GetValue()
#       print bname,cpsbr.GetLeaf(leaf),cpsval

    return cpsval

#----------------------------------------------------------------------
# Note the formula:
#   err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
#
def GetChain(tree, bname, lb, count, sfx_out, collection):
    ch          = collection.GetCostChain(lb,bname)
    # TimM - no longer do any rates here
    #pastrate    = ch.GetRate()
    #cumulative  = ch.GetRate()*count
    #lbrate      = getattr(tree,bname+'_'+sfx_out)
    #avgrate     = (cumulative+lbrate)/(count+1)
    #err2        = ch.GetRate() / ((count+1)*samplingrate)

    #ch.SetRate(avgrate)
    #ch.SetRateErr(math.sqrt(err2))

#	  # Debug
#	  print 'Old lb=', lb, 'lblast=', lblast, 'count=', count, 'pastrate=', pastrate, 'cum=', cumulative, 'lbrate', lbrate, 'avgrate=', avgrate
    return ch

#----------------------------------------------------------------------
def SetNewChainL1(tree, ch, bname):
    ch.SetPrescale(getattr(tree,bname+'_PS'))

    #if ch.GetName() == 'L1_EM10VH':
    #    print "DBG NEW L1 " , getattr(tree,bname+'_TBP')

    # Reverse-engineer counts -- will be approximate
    ch.SetTBPCnt (getattr(tree,bname+'_TBP')*samplingrate)
    ch.SetTAPCnt (getattr(tree,bname+'_TAP')*samplingrate)
    ch.SetTAVCnt (getattr(tree,bname+'_TAV')*samplingrate)

    # Rates are stored in TRP ntuples
    ch.SetTBPRate(getattr(tree,bname+'_TBP'))
    ch.SetTAPRate(getattr(tree,bname+'_TAP'))
    ch.SetTAVRate(getattr(tree,bname+'_TAV')) # Don't uncomment!! Already done in SetRate
    return

#----------------------------------------------------------------------
def SetNewChainHLT(tree, ch, bname, sfx_in, sfx_ps, sfx_out):
#	  ch.SetPrescale(getattr(tree,bname+'_prescale'))

    # Reverse-engineer counts -- will be approximate
    ch.SetTBPCnt (getattr(tree,bname+'_'+sfx_in )*samplingrate)
    ch.SetTAPCnt (getattr(tree,bname+'_'+sfx_ps )*samplingrate)
    ch.SetTAVCnt (getattr(tree,bname+'_'+sfx_out)*samplingrate)

#   if getattr(tree,bname+'_output'     )-getattr(tree,bname+'_raw'     ) > 1e-10:
#   print "not equal",bname,getattr(tree,bname+'_'+sfx_out     ),getattr(tree,bname+'_raw'     )

    # Rates are stored in TRP ntuples
    ch.SetTBPRate(getattr(tree,bname+'_'+sfx_in ))
    ch.SetTAPRate(getattr(tree,bname+'_'+sfx_ps ))
    ch.SetTAVRate(getattr(tree,bname+'_raw'     )) # Don't uncomment!! Already done in SetRate
    return

#----------------------------------------------------------------------
def SetOldChainL1(tree, ch, bname, count):
    ch.SetPrescale(getattr(tree,bname+'_PS'))


    # Reverse-engineer counts -- will be approximate
    ch.SetTBPCnt (getattr(tree,bname+'_TBP')*samplingrate + ch.GetTBPCnt())
    ch.SetTAPCnt (getattr(tree,bname+'_TAP')*samplingrate + ch.GetTAPCnt())
    ch.SetTAVCnt (getattr(tree,bname+'_TAV')*samplingrate + ch.GetTAVCnt())

    # Rates are stored in TRP ntuples
    # This needs to be a running average over the entries in the LB

    setattr(ch,"ratesCounts", ch.GetAttrWithDefault("ratesCounts",0) + 1.0)

    setattr(ch,"cumulativeTBP", ch.GetAttrWithDefault("cumulativeTBP",0) + getattr(tree,bname+'_TBP'))
    setattr(ch,"cumulativeTAP", ch.GetAttrWithDefault("cumulativeTAP",0) + getattr(tree,bname+'_TAP'))
    setattr(ch,"cumulativeTAV", ch.GetAttrWithDefault("cumulativeTAV",0) + getattr(tree,bname+'_TAV'))

    ch.SetTBPRate(getattr(ch,"cumulativeTBP") / getattr(ch,"ratesCounts"))
    ch.SetTAPRate(getattr(ch,"cumulativeTAP") / getattr(ch,"ratesCounts"))
    ch.SetTAVRate(getattr(ch,"cumulativeTAV") / getattr(ch,"ratesCounts")) 

    #if ch.GetName() == 'L1_EM10VH':
    #    print "DBG OLD L1 " , ch.GetAttrWithDefault("cumulativeTBP",0) , " - " , ch.GetTBPRate()

    # Rate errors
    ch.SetTBPRateErr(math.sqrt( (getattr(ch,"cumulativeTBP")/getattr(ch,"ratesCounts")) / (getattr(ch,"ratesCounts")*samplingrate) ))
    ch.SetTAPRateErr(math.sqrt( (getattr(ch,"cumulativeTAP")/getattr(ch,"ratesCounts")) / (getattr(ch,"ratesCounts")*samplingrate) ))
    ch.SetTAVRateErr(math.sqrt( (getattr(ch,"cumulativeTAV")/getattr(ch,"ratesCounts")) / (getattr(ch,"ratesCounts")*samplingrate) )) 
    return


#----------------------------------------------------------------------
def SetOldChainHLT(tree, ch, bname, count, sfx_in, sfx_ps, sfx_out):
#	  ch.SetPrescale(getattr(tree,bname+'_prescale'))

    if ch is None:
      return

    # Reverse-engineer counts -- will be approximate
    ch.SetTBPCnt (getattr(tree,bname+'_'+sfx_in )*samplingrate+ch.GetTBPCnt())
    ch.SetTAPCnt (getattr(tree,bname+'_'+sfx_ps )*samplingrate+ch.GetTAPCnt())
    ch.SetTAVCnt (getattr(tree,bname+'_'+sfx_out)*samplingrate+ch.GetTAVCnt())

    # Rate are stored in TRP ntuples
    setattr(ch,"ratesCounts", ch.GetAttrWithDefault("ratesCounts",0) + 1.0)

    setattr(ch,"cumulativeTBP", ch.GetAttrWithDefault("cumulativeTBP",0) + getattr(tree,bname+'_'+sfx_in))
    setattr(ch,"cumulativeTAP", ch.GetAttrWithDefault("cumulativeTAP",0) + getattr(tree,bname+'_'+sfx_ps))
    setattr(ch,"cumulativeTAV", ch.GetAttrWithDefault("cumulativeTAV",0) + getattr(tree,bname+'_'+sfx_out))

    ch.SetTBPRate(getattr(ch,"cumulativeTBP") / getattr(ch,"ratesCounts"))
    ch.SetTAPRate(getattr(ch,"cumulativeTAP") / getattr(ch,"ratesCounts"))
    ch.SetTAVRate(getattr(ch,"cumulativeTAV") / getattr(ch,"ratesCounts")) 

    # Rate errors
    ch.SetTBPRateErr(math.sqrt( (getattr(ch,"cumulativeTBP")/getattr(ch,"ratesCounts")) / (getattr(ch,"ratesCounts")*samplingrate) ))
    ch.SetTAPRateErr(math.sqrt( (getattr(ch,"cumulativeTAP")/getattr(ch,"ratesCounts")) / (getattr(ch,"ratesCounts")*samplingrate) ))
    ch.SetTAVRateErr(math.sqrt( (getattr(ch,"cumulativeTAV")/getattr(ch,"ratesCounts")) / (getattr(ch,"ratesCounts")*samplingrate) )) 
    return

# tmhong: Not used anywhere, so commenting out
##----------------------------------------------------------------------
def NonChainTRP(chname):
    return string.count(chname,"_grp_") != 0 \
           or string.count(chname,"_str_") != 0 \
           or string.count(chname,"L1_EMPTY_") != 0\
           or string.count(chname,"_recording") != 0\
           or string.count(chname,"_LB_Num") != 0\
           or string.count(chname,"_L1A") != 0\
           or string.count(chname,"_total") != 0\
           or string.count(chname,"_time") != 0\
           or string.count(chname,"HLT_RATE") != 0\
           or string.count(chname,"HLT_Providers") != 0

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
# On 15 April 2011 14:06, Antonio Sidoti wrote:
#	    Dear TRP-Rates power users,
#	    I've just discovered that when we've installed the new version
#	    of the archiver just before the technical shutdown, the suffix
#	    ISS_TRP was dropped from the TTree name containing the rates
#	    of the TimePoint.  So for example, the name of the TTree that
#	    was containing the L1 rates (from CTP) passed from
#	    "ISS_TRP.L1_Rate" to "L1_Rate". This was not foreseen but I
#	    think it is less cumbersome as it is now.  So please update
#	    your scripts.

#eof
