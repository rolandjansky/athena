# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# author Ricardo Abreu <ricardo.abreu@cern.ch

"""Prints the event header and returns the event untouched"""

import eformat.dump as edump

def modify(event):
  print 'Event Header:'
  print edump.fullevent_handler(event)
  return event # no changes required