# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Run repeating timer in additional thread
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
import threading

class RepeatingTimer(threading._Timer):
  '''To allow timing output in an additional thread. From http://stackoverflow.com/questions/5429577/'''
  def run(self):
    while True:
      self.finished.wait(self.interval)
      if self.finished.is_set():
        return
      else:
        self.function( *self.args, **self.kwargs )

  @classmethod
  def human_readable_time_interval( cls, seconds_elapsed ) :
    h = int( seconds_elapsed / (60 * 60) )
    m = int( (seconds_elapsed % (60 * 60)) / 60 )
    s = int( seconds_elapsed % 60. )
    return "{0}h {1:>02}m {2:>02}s".format(h, m, s)
