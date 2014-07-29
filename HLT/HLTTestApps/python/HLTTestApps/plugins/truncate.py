# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# author Andre Anjos <andre.dos.anjos@cern.ch>
# author Werner Wiedenmann <werner.wiedenmann@cern.ch>
# author Ricardo Abreu <ricardo.abreu@cern.ch>

"""Truncates a few of the ROBs in the input event according to a hit list.
"""

import eformat
import logging

from HLTTestApps.plugins import get_robhit
robhit = get_robhit()

def truncate(event, roblist, ratio=0.5):
  """Truncates the robs in the list you pass by the amount you choose.
  """
  if ratio >= 1.0: return event
  if ratio <= 0: return event

  logging.info('Truncating (at most) %d ROBs in event %d' % (len(roblist),
      event.lvl1_id()))
  newevent = eformat.write.FullEventFragment(event)
  for rob in newevent:
    if rob.source_id().code() in roblist:
      newdata = list(rob.rod_data())
      point = -1*int(len(newdata) * ratio)
      specific = 0x1 << (27 - 16) # this bit means truncation
      status = eformat.helper.Status(eformat.helper.GenericStatus.DATA_CORRUPTION, specific)
      rob.rod_data(newdata[:point])
      rob.status([status.code()])
  
  return newevent.readonly()

def modify(event):
  return truncate(event, robhit)
