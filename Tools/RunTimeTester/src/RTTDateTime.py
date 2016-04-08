# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time

class RTTDateTime:
  def __init__(self, time=None):
    self.ttime = time
    self.valid = False
    if time: self.valid = True

  def isValid(self): return self.valid
  
  def date(self):
    if self.ttime:
      return time.strftime('%Y/%m/%d', time.gmtime(self.ttime))
    else:
      return '----/--/--'

  def time(self):
    if self.ttime:
      return time.strftime('%H:%M:%S', time.gmtime(self.ttime))
    else:
      return '--:--:--'

  def absTime(self): return self.ttime

  def sameDate(self, other):
    
    gmt1 = time.gmtime(self.ttime)
    
    try:
      gmt2 = time.gmtime(other.ttime)
    except Exception:
      return False
    
    return gmt1[0]==gmt2[0] and gmt1[1]==gmt2[1] and gmt1[2]==gmt2[2]
  
  def __str__(self):
    return '%s %s' % (self.date(), self.time())

  def __cmp__(self, other):
    return cmp(self.ttime, other.ttime)
    
if __name__ == '__main__':
  a = RTTDateTime(time.time())
  time.sleep(3)
  b = RTTDateTime(time.time())
  print a<b
  print b<a
  
