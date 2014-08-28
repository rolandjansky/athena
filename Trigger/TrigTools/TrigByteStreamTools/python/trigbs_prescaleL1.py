#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Module for applying L1 prescales to the input data

Used as a "-Z" plugin to athenaMT/PT:
athenaMT/PT -Z TrigByteStreamTools.trigbs_prescaleL1 ...
"""

import os
import random
import zlib
import sys
from AthenaCommon.Logging import logging

import eformat
import libpyevent_storage as EventStorage

import PyCintex
PyCintex.loadDictionary('TrigByteStreamToolsDict')
CTPdataformat = PyCintex.makeNamespace('CTPdataformat')()

def ints2bits(info):
  if type(info)==int:
    info=[info]
  bits=[]
  cnt=0;
  for word in info:
    for i in range(32):
      if word&(1<<i):
        bits+=[cnt]
      cnt+=1
  return bits

def bits2ints(bits,len=1):
    words = [0] * len
    for bit in bits:
      words[bit//32] |= 1<<(bit%32)
    return words

def modify(event):
  """Prescale L1 items in CTP fragment used by L2."""  

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
      L1TBP = ints2bits(data[CTPdataformat.TBPpos : CTPdataformat.TBPpos+CTPdataformat.TBPwords])    # 8:16
      newL1TAV = []
      for ctp in L1TBP:
        if prescales[ctp]<=0: continue
        if random.uniform(0,int(prescales[ctp]))>1: continue
        newL1TAV.append(ctp)

      newL1TAVBits = bits2ints(newL1TAV,CTPdataformat.TBPwords)
      for i,value in enumerate(newL1TAVBits):
        data[CTPdataformat.TAVpos+i] = value

      newrob = eformat.write.ROBFragment(rob)        
      newrob.rod_data(data)
      new_event.append(newrob)

  return new_event.readonly()


#read in L1 prescales to be applied
L1Key=None
database=None
for arg in sys.argv:
  if arg.find('DBServer')!=-1:
    args=arg.split(':')
    for aa in args:
      if aa.startswith('DBServer='):
        database=aa.split('=')[1]
      if aa.startswith('DBLVL1PSKey=') or aa.startswith('DBL1PSKey'):
        L1Key=int(aa.split('=')[1])

log = logging.getLogger('trigbs_prescaleL1.py')
if database!=None and L1Key!=None:
  log.warning('Applying L1 prescale set %s from %s' % (L1Key,database))
  from TrigConfigSvc.TrigConfigSvcUtils import getL1Prescales
  name, prescales = getL1Prescales(database,L1Key)
else:
  log.warning('Applying L1 prescale set from default XML file')
  from TriggerMenuPython.XMLReader import L1MenuXMLReader
  r = L1MenuXMLReader("TriggerMenuXML/LVL1config_Physics_pp_v1.xml")
  name = dict(r.LVL1Config.items)['name']
  prescales = [ps.element.text for ps in r.LVL1Config.PrescaleSet.Prescales]

