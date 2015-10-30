#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
filterREGTEST.py
Filter to remove DEBUG messages but keep REGTEST messages.
"""

import sys
import re

def main():
   if len(sys.argv)>1:
      print __doc__
      return 1

   re_debug = re.compile('\S+\s+DEBUG.*')
   for line in sys.stdin:
      # Ignore DEBUG lines but keep REGTEST
      if re_debug.match(line)!=None and line.find("REGTEST")==-1:
         pass
      else:
         print line,
         
   return 0

if __name__ == "__main__":
   try:
      sys.exit(main())
   except IOError, e:
      (code, msg) = e
      if (code==32): pass   # ignore broken pipe exception
      else: raise e
   except KeyboardInterrupt:
      sys.exit(1)
