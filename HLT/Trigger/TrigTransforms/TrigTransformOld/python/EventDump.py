#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import eformat
import array


### ************************************ ###

def chainsPassed(info):

  chains=[]
  cnt=0;
  for word in info:
    for i in range(32):
      if word&(1<<i):
        chains+=[cnt]
      cnt+=1
  return chains


    
### ************************************ ###
## Get Chain IDs event by event
    
def dump_chains(event):
    
  L2PassedChains=chainsPassed(event.lvl2_trigger_info())
  EFPassedChains=chainsPassed(event.event_filter_info())

  return (L2PassedChains,EFPassedChains) 



#print get_ChainIDs_that_passed("/tmp/sfyrla/testData.data")
