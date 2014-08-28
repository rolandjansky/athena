#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import subprocess
import EventApps
import eformat

import optparse
import operator

__doc__ = """\
Dumping content of HLT result and HLT related detais from event header.
"""


optparser = optparse.OptionParser(usage = "%prog [options] FILES",
                                  description = __doc__)

optparser.add_option("-n", "--events", metavar="N", action="store", type='int',
                     help="Process N events")

optparser.add_option("", "--l1", action="store_true", default=False,
                     help="L1 trigger bits (from event header)")

optparser.add_option("", "--l2", action="store_true", default=False,
                     help="L2 trigger bits (from event header)")

optparser.add_option("", "--ef", action="store_true", default=False,
                     help="EF trigger bits (from event header)")

optparser.add_option("", "--stag", action="store_true", default=False,
                     help="stream tag")

optparser.add_option("", "--l2res", action="store_true", default=False,
                     help="details of L2 ROB payload")

optparser.add_option("", "--efres", action="store_true", default=False,
                     help="details of EF ROB payload")

optparser.add_option("", "--hltres", action="store_true", default=False,
                     help="details of HLT ROB(s) payload")

optparser.add_option("", "--chains", action="store_true", default=False,
                     help="details about chains")

optparser.add_option("", "--features", action="store_true", default=False,
                     help="details about features")

optparser.add_option("", "--tes", action="store_true", default=False,
                     help="details about TriggerElements (unimplemented)")
optparser.add_option("", "--conf", action="store_true", default=False,
                     help="configuration keys")

optparser.add_option("", "--sizes", action="store_true", default=False,
                     help="dump info about sizes")

optparser.add_option("", "--stats", action="store_true", default=False,
                     help="dump counts at the end only")

optparser.add_option("", "--sizeSummary", action="store_true", default=False,
                     help="dump info about sizes at the end")

optparser.add_option("", "--interactive", action="store_true", default=False,
                     help="after opening the file enter interactive prompt")



(opt, args) = optparser.parse_args() 
# options validation

if len(args)==0:
  optparser.error("No FILES specified")

from TrigByteStreamTools.hltResult import  *

res = hltResult()
#if opt.interactive:
#  import code
#  code.interact(local=locals());


featureSizes={}
stats = {}

def my_dump(bsfile):
  """Runs the dumping routines"""
  
  # open a file
  print "="*100
  print "Opening", bsfile

  input = eformat.istream(bsfile)

  if opt.interactive:
    import code
    code.interact(local=locals());
    
  event_count = 0
  l2_event_count = 0
  ef_event_count = 0
  
  for event in input:
    event_count += 1
    if opt.events!=None and event_count>opt.events: break
    
    print "======================= Event: %d,  LB: %d, LVL1_ID: %d, Global_ID: %d bunch-x: %d TT: x%x ==========================" \
          % ( event_count, event.lumi_block(), event.lvl1_id(), event.global_id(), event.bc_id(), event.lvl1_trigger_type())
    if opt.l1:
      print "L1 TriggerInfo: ", ["0x%x"%i for i in event.lvl1_trigger_info() ]
  
    if opt.l2:
      print "L2 TriggerInfo: ", ["0x%x"%i for i in event.lvl2_trigger_info() ]


    # loop over the SubDetFragments and find LVL2
    if True:
      found=False
      for f in event.children():
        if f.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_LVL2:
          print '.. %s %s %s bytes' % (f.__class__.__name__, f.source_id(), f.fragment_size_word()*4)
          res.load(f)
          found=True
          l2_event_count += 1

          if opt.l2res:
            print_HLTResult(res, opt)
          if opt.sizeSummary:
            collect_feature_sizes(featureSizes, res)            
          print ".. EOF HLTResult for L2"
      if not found:
        print ".. No HLTResult for L2"
        
      
    if opt.ef:
      print "EF TriggerInfo: ", ["0x%x"%i for i in event.event_filter_info()]


    # loop over the SubDetFragments and find EF
    if True:
      found=False
      for f in event.children():
        if f.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_EVENT_FILTER:
          print '.. %s %s %s bytes' % (f.__class__.__name__, f.source_id(), f.fragment_size_word()*4)          
          try:
            res.load(f)
            found = True
            ef_event_count += 1
          
            if opt.efres or opt.hltres:
              print_HLTResult(res, opt)
            if opt.sizeSummary:
              collect_feature_sizes(featureSizes, res)
          except Exception, ex:
            print '... **** problems in analyzing payload', ex
            print '... **** raw data[:10]', list(f.rod_data())[:10]
          print ".. EOF HLTResult for EF"
      if not found:
        print ".. No HLTResult for EF"
          
    if opt.stag:
      print "StreamTag: ", [(s.name, s.type) for s in event.stream_tag()]
    
  event_count = max(l2_event_count, ef_event_count)   
  if opt.sizeSummary:
    print '... '+20*'-'+'sizes by type'
    for f,s in sorted(featureSizes.iteritems(),key=operator.itemgetter(1),reverse=True):
      if '#' not in f:
        print ".... %-70s %6d B %6d B/ev" %(f, s, (1.*s)/event_count)
    print '... '+20*'-'+'sizes by type#key'        
    for f,s in sorted(featureSizes.iteritems(),key=operator.itemgetter(1),reverse=True):
      if '#' in f:
        print ".... %-70s %6d B %6d B/ev" %(f, s, (1.*s)/event_count)        

if __name__ == "__main__":
  for f in args:
    my_dump(f)
