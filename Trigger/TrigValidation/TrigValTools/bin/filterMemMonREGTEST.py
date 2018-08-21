#!/usr/bin/env python

# Copyright (C) 2018- CERN for the benefit of the ATLAS collaboration

"""
filterMemMonREGTEST.py

Filter to remove DEBUG messages but keep REGTEST messages
and start memory monitoring.

Please note that the script is compatible with athenaHLT 
and runHLTMPPy.
"""

import sys
import re
import subprocess

def main():
   if len(sys.argv)>1:
      print __doc__
      return 1

   re_debug = re.compile('\S+\s+DEBUG.*')
   for line in sys.stdin:
      # Ignore DEBUG lines but keep REGTEST
      if re_debug.match(line)!=None and line.find("REGTEST")==-1:
         pass
      # Find pid of mother for memory monitoring
      elif line.find("4 Option : 'DF_PID'")!=-1:
         pid = int(re.findall('\d+', line)[1])
         mem_mon = subprocess.Popen(['MemoryMonitor','--pid','{}'.format(pid),'--filename',
				     'MemMon.txt','--json-summary','MemMonSummary.json',
				      '--interval','0.01'], stderr=subprocess.PIPE)
         print line,
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


