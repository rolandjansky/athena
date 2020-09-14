#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
Module for applying L1 prescales to the input data

Used as a "-Z" plugin to athenaMT/PT:
athenaMT/PT -Z TrigByteStreamTools.trigbs_prescaleL1 ...
"""

import cppyy
cppyy.load_library('libTrigByteStreamToolsDict')

def ints2bits(info):
  if type(info)==int:
    info=[info]
  bits=[]
  cnt=0
  for word in info:
    for i in range(32):
      if word&(1<<i):
        bits+=[cnt]
      cnt+=1
  return bits

def modify(event):
  """filter out events not triggered by L1_RD1_FILLED (bit 63) """

  L1TAV=ints2bits(event.lvl1_trigger_info()[16:24])
  if 63 in L1TAV:
    #print 'ACCEPTED'
    return event
  else:
    #print 'REJECTED'
    return False

