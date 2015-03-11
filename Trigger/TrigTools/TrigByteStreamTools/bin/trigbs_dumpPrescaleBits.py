#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """\
Dumping HLT chains PS bit vs the chain counter and vs the L1 event number
"""

import eformat
import cPickle
import optparse
import cppyy
from TrigByteStreamTools.hltResult import hltResult

optparser = optparse.OptionParser(usage = "%prog [options] FILES",
                                  description = __doc__)

optparser.add_option("", "--pickle", default='l2bits.pickle',
                     help="output file for the result pickle")

optparser.add_option("", "--comp", action="store_true", default=False,
                     help="Run comparison")

optparser.add_option("", "--one", default='',
                     help="1st file for the comparison")

optparser.add_option("", "--two", default='',
                     help="2nd file for the comparison")

optparser.add_option("", "--ef", action="store_true", default=False,
                     help="Use EF result instead of L2")

(opt, args) = optparser.parse_args() 



res = hltResult()

featureSizes={}
stats = {}
HLTChain = cppyy.makeClass('HLT::Chain')

def my_dump(bsfile):
  """Runs the dumping routines"""
  
  # open a file
  print "="*100
  print "Opening", bsfile
  
  
  events = {}  
  
  input = eformat.istream(bsfile)  
  subdet = eformat.helper.SubDetector.TDAQ_EVENT_FILTER if opt.ef else eformat.helper.SubDetector.TDAQ_LVL2
  
  for event in input:
    ev = ( event.global_id(), event.lvl1_id() )
    
    chains = {}
    for f in event.children():
      if f.source_id().subdetector_id() == subdet:
        res.load(f)
        chains_data = list(res.getChainResult())
        #nchains = chains_data[0]
        for c in chains_data[1:]:
          chain = HLTChain(c)
          chains[chain.getChainCounter()] = chain.isPrescaled()
        break

    events[ev] = chains
    
  output = file(opt.pickle, "w")
  cPickle.dump(events, output)
  output.close()
    

def my_comp(f1, f2):
  import pprint

  one = cPickle.load(file(f1))
  two = cPickle.load(file(f2))
  print '.. eventdumps read in correctly %s %s' % (f1, f2)

  if one == two:
    print ".. the prescale bits are the same"
  else:
    diff=False
    kone = set(one.keys())
    ktwo = set(two.keys())
    if kone != ktwo:
      print ".... the event sets are different in the two files; one has: %d  two has: %d events" % (len(kone), len(ktwo))
      print ".... will compare only events present in both sets"
    komon = kone & ktwo
    for k in komon:      
      if one[k] != two[k]:
        print ".. event difference global_id: %d l1_id: %d, fmt: chain_counter: PS bit,... " %k
        print ".. event in file: %s " % opt.one
        pprint.pprint(one[k])
        print ".. event in file: %s" % opt.two
        pprint.pprint(two[k])
        diff=True
    if not diff:
      print '.. prescale bits are the same in all %d common events ' % len(komon)
    else:
      print '.. there are differences !'

  

if __name__ == "__main__":
  if not opt.comp:
    for f in args:    
      my_dump(f)
  else:
    my_comp(opt.one, opt.two)
      
