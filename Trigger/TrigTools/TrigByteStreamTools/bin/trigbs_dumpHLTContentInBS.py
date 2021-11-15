#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import eformat
import argparse
import operator
from functools import cache

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

parser.add_argument("--l1", action="store_true", default=False,
                    help="L1 trigger bits (from event header)")

parser.add_argument("--decodeItems", action="store_true", default=False,
                    help="Decode trigger item names")

parser.add_argument("--smk", action="store", type=int, default=0,
                    help="Specify SMK for trigger item decoding if not available in HLT result")

parser.add_argument("--l2", action="store_true", default=False,
                    help="L2 trigger bits (from event header)")

parser.add_argument("--ef", "--hlt", action="store_true", default=False,
                    help="EF/HLT trigger bits (from event header)")

parser.add_argument("--ctp", nargs="?", metavar="MID", default=False, const=1,
                    help="CTP ROB details of ROB with module_id MID [default=%(const)s]")

parser.add_argument("--stag", action="store_true", default=False,
                    help="stream tag")

parser.add_argument("--l2res", action="store_true", default=False,
                    help="details of L2 ROB payload")

parser.add_argument("--efres", "--hltres", action="store_true", default=False,
                    help="details of EF/HLT ROB payload")

parser.add_argument("--chains", action="store_true", default=False,
                    help="details about chains")

parser.add_argument("--features", action="store_true", default=False,
                    help="details about features")

parser.add_argument("--tes", action="store_true", default=False,
                    help="details about TriggerElements")

parser.add_argument("--conf", action="store_true", default=False,
                    help="configuration keys")

parser.add_argument("--sizes", action="store_true", default=False,
                    help="dump info about sizes")

parser.add_argument("--stats", action="store_true", default=False,
                    help="dump counts at the end only")

parser.add_argument("--sizeSummary", action="store_true", default=False,
                    help="dump info about sizes at the end")

parser.add_argument("--interactive", action="store_true", default=False,
                    help="after opening the file enter interactive prompt")



args = parser.parse_args() 

from TrigByteStreamTools.hltResult import hltResult, print_HLTResult, collect_feature_sizes
from TrigByteStreamTools import CTPfragment

res = hltResult()
#if args.interactive:
#  import code
#  code.interact(local=locals());


featureSizes={}
stats = {}
smk = None

def Lvl1_Info(event):
  info = event.lvl1_trigger_info()
  nwords = len(info)/3   # TBP,TAP,TAV
  return [CTPfragment.decodeTriggerBits(info[i*nwords:(i+1)*nwords]) for i in range(3)]
  
  
def CTP_Info(event, module_id=1):
  ctp_robs = [rob for rob in event.children() \
              if rob.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_CTP \
              and rob.source_id().module_id()==module_id]

  if len(ctp_robs)==0:
    print("No CTP ROB found")
    
  for rob in ctp_robs:
    x = CTPfragment.getExtraPayloadObject(rob)
    folderUpdates = CTPfragment.getFolderUpdates(x)
    upd = ''.join(['[%d,%d]' % (f.second.folderIndex,f.second.lumiBlock) for f in folderUpdates])
    print("ROB 0x%0x, L1ID %10d, LB %4d, Version %d, Bunch %d, HLT counter: %3d, Payload #%d %s L1PSK %d BGK %d COOLUPD %s" % (
      rob.source_id(),
      event.lvl1_id(),
      event.lumi_block(),
      CTPfragment.ctpFormatVersion(rob),
      CTPfragment.lvl1AcceptBunch(rob),
      CTPfragment.hltCounter(rob),
      CTPfragment.numberHltExtraPayloadWords(rob),
      CTPfragment.hltExtraPayloadWords(rob),
      x.getL1PSK(),
      x.getBGK(),
      upd
    ))
    for w in ['TBP','TAP','TAV']:
      items = CTPfragment.decodeTriggerBits(CTPfragment.getTriggerWords(rob,w))
      print("ROB 0x%0x, %s: %s" % (rob.source_id(), w, printL1Items(items,smk)))


@cache
def getL1Menu(smk):
  from CoolRunQuery.utils.AtlRunQueryTriggerUtils import getL1Menu
  return getL1Menu(str(smk))

def printL1Items(items,smk):
  if not args.decodeItems:
    return items
  
  l1menu = getL1Menu(smk)
  names = [l1menu[i].name for i in items]
  return names

  
def my_dump(bsfile):
  """Runs the dumping routines"""

  global smk
  
  # open a file
  print("="*100)
  print("Opening", bsfile)

  input = eformat.istream(bsfile)

  if args.interactive:
    import code
    code.interact(local=locals())
    
  event_count = 0
  l2_event_count = 0
  ef_event_count = 0
  offset = args.skip if args.skip else 0
  for event in input:
    if offset>0:
      offset -= 1
      continue
      
    event_count += 1
    
    if args.events is not None and event_count>args.events:
      break
    
    print("======================= RunNumber : %d , Event: %d,  LB: %d, LVL1_ID: %d, Global_ID: %d bunch-x: %d TT: x%x =========================="
          % ( event.run_no(), event_count, event.lumi_block(), event.lvl1_id(), event.global_id(), event.bc_id(), event.lvl1_trigger_type()))

    smk = args.smk
    if args.decodeItems and args.smk==0:  # Need to get SMK from HLT result
      hltrob = [f for f in event.children() if f.source_id().subdetector_id() in [eformat.helper.SubDetector.TDAQ_LVL2,eformat.helper.SubDetector.TDAQ_EVENT_FILTER] ]
      if len(hltrob)==0:
        print("ERROR: Cannot find HLT result. Will not decode trigger item names.")
        args.decodeItems = False
      else:
        res.load(hltrob[0])
        smk = res.getConfigSuperMasterKey()
        if smk==0:
          print("ERROR: No SMK stored in HLT result. Will not decode trigger item names.")
          args.decodeItems = False
      
    if args.l1:
      #print "L1 TriggerInfo: ", ["0x%x"%i for i in event.lvl1_trigger_info() ]
      words = Lvl1_Info(event)
      print("L1 CTP IDs - TBP: ", printL1Items(words[0],smk))
      print("L1 CTP IDs - TAP: ", printL1Items(words[1],smk))
      print("L1 CTP IDs - TAV: ", printL1Items(words[2],smk))

    if args.ctp:
      CTP_Info(event,int(args.ctp))
                  
    if args.l2:
      print("L2 TriggerInfo: ", ["0x%x"%i for i in event.lvl2_trigger_info() ])

    # loop over the SubDetFragments and find LVL2
    if args.l2res or args.sizeSummary:
      found=False
      for f in event.children():
        if f.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_LVL2:
          print('.. %s %s %s bytes' % (f.__class__.__name__, f.source_id(), f.fragment_size_word()*4))
          res.load(f)
          found=True
          l2_event_count += 1

          if args.l2res:
            print_HLTResult(res, args)
          if args.sizeSummary:
            collect_feature_sizes(featureSizes, res)            
          print(".. EOF HLTResult for L2")
      if not found:
        print(".. No HLTResult for L2")
        
      
    if args.ef:
      print("EF TriggerInfo: ", ["0x%x"%i for i in event.event_filter_info()])


    # loop over the SubDetFragments and find EF
    if args.efres or args.sizeSummary:
      found=False
      for f in event.children():
        if f.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_EVENT_FILTER:
          print('.. %s %s %s bytes' % (f.__class__.__name__, f.source_id(), f.fragment_size_word()*4))          
          try:
            res.load(f)
            found = True
            ef_event_count += 1
          
            if args.efres:
              print_HLTResult(res, args)
            if args.sizeSummary:
              collect_feature_sizes(featureSizes, res)
          except Exception as ex:
            print('... **** problems in analyzing payload', ex)
            print('... **** raw data[:10]', list(f.rod_data())[:10])
          print(".. EOF HLTResult for EF")
      if not found:
        print(".. No HLTResult for EF")

    if args.stag:
      print("StreamTag: ", [(s.name, s.type) for s in event.stream_tag()])
    
  event_count = max(l2_event_count, ef_event_count)   
  if args.sizeSummary:
    print('... '+20*'-'+'sizes by type')
    for f,s in sorted(featureSizes.iteritems(),key=operator.itemgetter(1),reverse=True):
      if '#' not in f:
        print(".... %-70s %6d B %6d B/ev" %(f, s, (1.*s)/event_count))
    print('... '+20*'-'+'sizes by type#key')        
    for f,s in sorted(featureSizes.iteritems(),key=operator.itemgetter(1),reverse=True):
      if '#' in f:
        print(".... %-70s %6d B %6d B/ev" %(f, s, (1.*s)/event_count))        


if __name__ == "__main__":
  for f in args.files:
    my_dump(f)
