#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
Module for applying L1 prescales to the input data

Used as a "-Z" plugin to athenaHLT:
athenaHLT -Z TrigByteStreamTools.trigbs_prescaleL1 ...
"""

import random
import zlib
import sys
from AthenaCommon.Logging import logging

import eformat

import cppyy
cppyy.load_library('libTrigByteStreamToolsDict')
from ROOT import CTPdataformat
from TrigByteStreamTools import CTPfragment

prescales = None
log = logging.getLogger('trigbs_prescaleL1.py')
log.setLevel(logging.INFO)

def loadPrescales(config):
  """Load L1 prescales from DB"""

  database = config['db-server']
  l1psk = int(config['db-extra']['lvl1key'])

  log.info('Applying L1 prescale set %s from %s', l1psk, database)
  from TrigConfigSvc.TrigConfigSvcUtils import getL1Prescales
  name, prescales = getL1Prescales(database,l1psk)
  log.debug('Prescales: %s', str(prescales))
  return prescales

  
def modify_general(**kwargs):
  """Prescale L1 items in CTP fragment used by HLT"""  
  global prescales
  
  if prescales is None:
    prescales = loadPrescales(kwargs['configuration'])

  event = kwargs["event"]
  
  # Set seed based on event time stamp to make it reproducible
  seed = '%s%s' % (event.bc_time_seconds(),event.bc_time_nanoseconds())
  random.seed(zlib.crc32(seed))
  
  new_event=eformat.write.FullEventFragment()
  new_event.copy_header(event)
  for rob in event:
    if rob.source_id()!=eformat.helper.SourceIdentifier(eformat.helper.SubDetector.TDAQ_CTP,1):
      new_event.append_unchecked(rob)
    else:
      data = [d for d in rob.rod_data()]
      v = CTPfragment.ctpFormatVersion(rob)
      TBPpos = CTPfragment._versioned(CTPdataformat,'TBPpos',v)
      TBPwords = CTPfragment._versioned(CTPdataformat,'TBPwords',v)
      TAVpos = CTPfragment._versioned(CTPdataformat,'TAVpos',v)
            
      L1TBP = CTPfragment.decodeTriggerBits(data[TBPpos:TBPpos+TBPwords])
      log.debug('L1TBP: %s', L1TBP)
      newL1TAV = []
      for ctp in L1TBP:
        if prescales[ctp]<=0:
          continue
        if random.uniform(0,prescales[ctp])>1:
          continue
        newL1TAV.append(ctp)

      log.debug('New L1TAV: %s', newL1TAV)
      newL1TAVBits = CTPfragment.encodeTriggerBits(newL1TAV,TBPwords)
      for i,value in enumerate(newL1TAVBits):
        data[TAVpos+i] = value

      # Write new CTP ROB
      newrob = eformat.write.ROBFragment(rob)        
      newrob.rod_data(data)
      new_event.append(newrob)
      # Update event header
      l1bits = [b for b in event.lvl1_trigger_info()]
      new_event.lvl1_trigger_info(l1bits[0:TBPwords]+data[TAVpos:TAVpos+TBPwords]*2)

  return new_event.readonly()



if __name__ == "__main__":
  if len(sys.argv)<=1:
    print("Syntax: trigbs_prescaleL1.py FILE")
    sys.exit(1)

  log.setLevel(logging.DEBUG)    
  kwargs = {'configuration' : {'db-server':'TRIGGERDB_RUN1',
                               'db-extra' : {'lvl1key' : 300}}}

  kwargs = {'configuration' : {'db-server':'TRIGGERDBREPR',
                               'db-extra' : {'lvl1key' : 30}}}  

  ost = eformat.ostream()
  for e in eformat.istream(sys.argv[1]):
    kwargs['event'] = e
    new_event = modify_general(**kwargs)
    ost.write(new_event)
