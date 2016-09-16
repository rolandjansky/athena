# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# author Andre Anjos <andre.dos.anjos@cern.ch>
# author Ricardo Abreu <ricardo.abreu@cern.ch>

"""Fills ROBs which are considered empty by a list
"""

import eformat
import logging

from HLTTestApps.plugins import get_robhit
robhit = get_robhit()

def fill_empty(event, roblist, bitset=30):
  """Fills-in missing ROB fragments in the event, according to a hit list.

  This method will fill in the event object with missing ROBs so that the
  hit list defined by the second parameter gets completely satisfied. In each
  newly created ROB, the status bit indicated by the "bitset" parameter will be
  set. These are the ones which are currently in use for tdaq-01-08-00:

   bit | Meaning
   ----+---------->
   30  | Pending: the ROBIN did not have a fragment for the requested L1ID but
       | this fragment may still arrive. It therefore generated an empty
       | fragment (this is the default)
       |
   29  | Lost: the ROBIN did not have a fragment for the requested L1ID. It
       | therefore generated an empty fragment

  ROBs which do not exist in the hit list they will also be removed and not
  served via the data collector.

  More up-to-date information can be found here:
  https://twiki.cern.ch/twiki/bin/view/Atlas/ROBINFragmentErrors
  """

  logging.info('Filling empty ROBs in event %d' % event.lvl1_id())

  # now we rebuild the event with what is left.
  newevent = eformat.write.FullEventFragment(event)
  gen_robs = list(roblist) # deep copy so we don't change the input

  rob_model = None
  for rob in newevent:
    rob_model = rob
    if rob.source_id().code() in gen_robs:
      del gen_robs[gen_robs.index(rob.source_id().code())]

  for rob_id in gen_robs:
    logging.info('Instantiating empty ROB for fragment %s' % \
        eformat.helper.SourceIdentifier(rob_id))
    newrob = eformat.write.ROBFragment()
    newrob.copy_header(rob_model)
    newrob.minor_version(0)
    newrob.rod_minor_version(0)
    rob_source = eformat.helper.SourceIdentifier(rob_id)
    newrob.source_id(rob_source)
    specific = 0x1 << (bitset - 16)
    status = eformat.helper.Status(eformat.helper.GenericStatus.DATA_CORRUPTION, specific)
    newrob.status([status.code()])
    newevent.append(newrob)

  logging.info('Instantiated %d empty ROBs in event %d' % (len(gen_robs), \
    event.lvl1_id()))

  return newevent.readonly() 

def modify(event):
  return fill_empty(event, robhit)  
