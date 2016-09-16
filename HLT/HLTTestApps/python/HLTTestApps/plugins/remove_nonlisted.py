# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# author Andre Anjos <andre.dos.anjos@cern.ch>
# author Werner Wiedenmann <werner.wiedenmann@cern.ch>
# author Ricardo Abreu <ricardo.abreu@cern.ch>

"""Removes ROBs from the event, which are not listed in the ROB hit list.
"""

import eformat
import logging

from HLTTestApps.plugins import get_robhit
robhit = get_robhit()

def cleanup(event, roblist):
  """Removes ROBs in the event which are not in the hitlist you specify
  """

  newevent = eformat.write.FullEventFragment()
  newevent.copy_header(event)
  for rob in event:
    if rob.source_id().code() not in roblist:
      logging.info('Removing ROB %s from event %d (not at hit list)' % \
                   (rob.source_id(), event.lvl1_id()))
      continue
    else:
      newrob = eformat.write.ROBFragment(rob)
      newevent.append(newrob)

  # return modified event
  # ---------------------
  return newevent.readonly()

def modify(event):
  return cleanup(event, robhit)
