#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import subprocess
import EventApps
import eformat

import argparse
import operator

from PyUtils.Decorators import memoize

__doc__ = """\
Dump content of the HLT result and HLT related details from the event header.
"""
parser = argparse.ArgumentParser(description = __doc__)

parser.add_argument("files", metavar="FILE", nargs='+',
                    help="RAW file to inspect")
                    
parser.add_argument("-n", "--events", metavar="N", action="store", type=int,
                     help="Process N events")

parser.add_argument("-s", "--skip", metavar="N", action="store", type=int,
                     help="Skip N events")

parser.add_argument("--interactive", action="store_true", default=False,
                    help="after opening the file enter interactive prompt")

parser.add_argument("-v", "--verbosity",type=int, choices=[0, 1, 2], default=2,
                    help="change output verbosity (default 2)")

args = parser.parse_args() 

from TrigByteStreamTools.hltResult import  *
res = hltResult()
  
def ds_dump(bsfile):
  """Runs the dumping routines"""
  
  # open a file
  if (args.verbosity > 0): 
    print "="*100
    print "Opening", bsfile

  input = eformat.istream(bsfile)

  if args.interactive:
    import code
    code.interact(local=locals());
    
  event_count = 0
  ds_ROB_counts = {}
  offset = args.skip if args.skip else 0
  for event in input:
    if offset>0:
      offset -= 1
      continue

    event_count += 1
    if args.events!=None and event_count>args.events: 
      event_count -=1
      break      
    
    # Extract the DS module IDs and set the flag for finding the corresponding ROB
    ds_module_ids={}
    for stag in event.stream_tag():
      if (stag.type == 'calibration') and ((stag.name).startswith('DataScouting_')):
        ds_module_ids[int((stag.name).split('_')[1])] = (False, stag.name) 
        if not ds_ROB_counts.has_key(stag.name):
          ds_ROB_counts[stag.name] = {'ROBfound':0, 'ROBnotFound':0, 'noContainers':0, 'emptyContainers':0}

    # loop over the SubDetFragments and find DS ROBs
    for f in event.children():
      if (f.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_HLT) and (ds_module_ids.has_key(f.source_id().module_id())):
        if (args.verbosity >= 1): 
          print "======================= RunNumber : %d , Event: %d,  LB: %d, LVL1_ID: %d, Global_ID: %d bunch-x: %d TT: x%x ==========================" \
              % ( event.run_no(), event_count, event.lumi_block(), event.lvl1_id(), event.global_id(), event.bc_id(), event.lvl1_trigger_type())
        # Print the stream tags
        if (args.verbosity >= 1): 
          print ".. Stream Tags:", [(s.name, s.type) for s in event.stream_tag()]

        featureSizes={}
        if (args.verbosity >= 1): 
          print '.. %s %s %s bytes' % (f.__class__.__name__, f.source_id(), f.fragment_size_word()*4)
        try:
          res.load(f)
          ds_module_ids[f.source_id().module_id()] = (True, ds_module_ids[f.source_id().module_id()][1])  # The expected ROB was found
          if (args.verbosity >= 2):           
            print_all_navigation(res)
          for feature in res.nav_payload:
	    key = feature[0]+'#'+feature[1]
	    if not featureSizes.has_key(key):
	      featureSizes[key] = 0
            featureSizes[key] += feature[2]
        except Exception, ex:
          print '... **** problems in analyzing payload', ex
          print '... **** raw data[:10]', list(f.rod_data())[:10]
        # check that feature containers are there and that they are not empty
        if (featureSizes == {}):
          if (args.verbosity >= 1): 
            print " FATAL : No containers are available for Data Scouting HLTResult with module ID = ",f.source_id().module_id(),", Stream name = ",ds_module_ids[f.source_id().module_id()][1]
          ds_ROB_counts[ ds_module_ids[f.source_id().module_id()][1] ]['noContainers'] += 1
        for item in featureSizes.iteritems():
          if (item[1] == 0):
            if (args.verbosity >= 1): 
              print " ERROR : Empty container for feature = ",item[0]
            ds_ROB_counts[ ds_module_ids[f.source_id().module_id()][1] ]['emptyContainers'] += 1

        if (args.verbosity >= 2): 
          print ".. EOF DS HLTResult with module ID = ", f.source_id().module_id()

    # check if all expected DS ROBs from the StreamTags were found
    for item in ds_module_ids.iteritems():
      if (item[1][0] == False):
        if (args.verbosity >= 1): 
          print " FATAL : No Data Scouting HLTResult found for expected module ID = ",item[0],", Stream name = ",item[1][1]
        ds_ROB_counts[ item[1][1] ]['ROBnotFound'] += 1
      else:
        ds_ROB_counts[ item[1][1] ]['ROBfound'] += 1

  if (args.verbosity >= 0): 
    print "\n Events read: ", event_count
    print "\n Summary of Data Scouting information:\n"," -------------------------------------\n", ds_ROB_counts            

if __name__ == "__main__":
  for f in args.files:
    ds_dump(f)
