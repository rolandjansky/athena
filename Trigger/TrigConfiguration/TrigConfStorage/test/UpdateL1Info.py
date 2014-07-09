#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, re, os

if __name__ == "__main__":
  for r in sys.argv[1:]:
    fd = os.popen("AtlCoolTrigger.py -r %s" % r)
    lines = fd.readlines()
    fd.close()
    for l in lines:
      m = re.match(".*run\W*(\d*).*smk\W*(\d*).*hltps\W*(\d*).*lvl1ps\W*(\d*)",l)
      if not m: continue
      os.popen("rm -f trigconf.db")
      xx = m.groups()
      logfn = "logDB2COOL_r%s.txt" % xx[0]
      copycmd = '''AtlCoolCopy.exe "COOLONL_TRIGGER/COMP200" "sqlite://;schema=trigconf.db;dbname=TRIGCONF" -create \\
    -exclude /TRIGGER/LUMI/LBLB \\
		-exclude /TRIGGER/LUMI/LBTIME \\
		-exclude /TRIGGER/LUMI/LVL1COUNTERS \\
    -exclude /TRIGGER/LVL1/BunchGroup \\
		-exclude /TRIGGER/LVL1/BunchGroupContent \\
		-exclude /TRIGGER/LVL1/BunchGroupDescription \\
    -exclude /TRIGGER/LVL1/CTPCOREINPUTS \\
		-exclude /TRIGGER/LVL1/CTPCORELBDATA \\
		-exclude /TRIGGER/LVL1/CTPCoreInputMapping \\
    -run %s''' % xx[0]
      print "Copying information for run %s from COOLONL_TRIGGER/COMP200 to trigconf.db" % xx[0]
      fd = os.popen(copycmd)
      output = fd.readlines()
      fd.close()


      
      cmd = """TrigConf2COOLApp -e write --trigdb TRIGGERDB \\
      --cooldb 'sqlite://;schema=trigconf.db;dbname=TRIGCONF;' \\
      --run %s --configkey %s --prescalekeyhlt %s --prescalekeylvl1 %s \\
      --restrictToFolders /TRIGGER/LVL1/Thresholds \\
      --restrictToFolders /TRIGGER/LVL1/ItemDef""" % xx
      print "Writing L1 thresholds for run %s with smk %s, hltpsk %s and l1psk %s to COOL" % xx,
      print "(logfile: %s)" % logfn
      fd = os.popen(cmd)
      output = fd.readlines()
      fd.close()
      log = open(logfn, "w")
      for o in output:
        print >> log, o,
      log.close()
