# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: LArRawConditions/python/LArConditionsContainer.py
# Author: RD Schaffer (R.D.Schaffer@cern.ch)

#
# Generic class for all python interface classes to LArConditionsContainers.
#
#   Main purpose is to add on python-style iterators over the containers
#
#   Usage: to add on iterators, simply create a python object for the
#   corresponding container, e.g.
#     cont = LArConditionsContainer("LArRampComplete")
#     then any LArRampComplete object will have iterators over
#     conditions, corrections, COOL channels, and IOVs.
#     i.e.  for r in ramps.conditionsIter(gain):  will loop over all
#     conditions
#

import cppyy

def conditionsIter(self, gain) :
  sequential = self.begin(gain)
  end        = self.end(gain)
  while sequential != end :
    yield sequential.__deref__(), sequential.channelId()
    sequential.__preinc__()
  raise StopIteration

def correctionsIter(self, gain) :
  sequential = self.correctionsBegin(gain)
  end        = self.correctionsEnd(gain)
  while sequential != end :
    yield sequential.__deref__()
    sequential.__preinc__()
  raise StopIteration

def coolChannelIter(self) :
  sequential = self.chan_begin()
  end        = self.chan_end()
  while sequential != end :
    yield sequential.__deref__()
    sequential.__preinc__()
  raise StopIteration

def coolIOVIter(self) :
  sequential = self.iov_begin()
  end        = self.iov_end()
  while sequential != end :
    yield sequential.__deref__()
    sequential.__preinc__()
  raise StopIteration

class LArConditionsContainer(object) :
  def __init__(self, t) :

    # t is either the class name or the class itself
    if type(t) is str :
      self.typeName = t
      t = cppyy.makeClass(t)
    else :
      self.typeName = t.type.__name__

    # Save type
    self.type = t

    # Add on iterators for the different containers
    self.type.conditionsIter  = conditionsIter
    self.type.correctionsIter = correctionsIter
    self.type.coolChannelIter = coolChannelIter
    self.type.coolIOVIter     = coolIOVIter 





