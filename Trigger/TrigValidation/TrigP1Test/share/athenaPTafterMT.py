#!/usr/bin/env python
#
# @file: athenaPTafterMT.py
# @purpose: Waits for the athenaMT RTT job to finish and runs athenaPT
# @author: Frank Winklmeier
#
# This script can be run in two ways:
# 1) Standalone
#    athenaPTafterMT.py --help for instructions
#    Example: athenaPTafterMT.py --jobOptions='myJobOpts.py'
#
# 2) As input to athena.py (for RTT)
#    Parameters that are listed under '--help' can be passed via the
#    the -c option (without the leading '--')
#    Example: athena -c"jobOptions='myJobOpts.py'" athenaPTafterMT.py
#

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import sys
import os
import optparse
import logging
import time

# Populate the option parser (but don't parse yet)

parser = optparse.OptionParser(description="Chain athenaMT/PT jobs")

parser.add_option("--jobOptions",
                  action = "store",
                  help = "job options for athenaMT/PT")

parser.add_option("--ptOptions",
                  action = "store",
                  help = "option file for athenaPT (via -F)")

parser.add_option("--ptInput",
                  action = "store", default = "Lvl2*.data",
                  help = "input file name for athenaPT (wildcards OK)")

parser.add_option("--args",
                  action = "store",
                  help = "remaining arguments for athenaMT/PT")

logging.basicConfig(level=logging.DEBUG,
                    format="--> %(name)-5s %(asctime)s %(levelname)7s %(message)s",
                    datefmt="%Y-%m-%d %H:%M:%S")
   
log = logging.getLogger("athenaPTafterMT")

      
######################################################################
# When run in athena, we will execute only the end of this script
######################################################################

def main():
   
   # Now parse the options
   (opts, args) = parser.parse_args()

   if not opts.jobOptions:
      parser.error("Option --jobOptions required")

   if not opts.args: args = ""
   else: args = opts.args
   
   sleeptime = 30   # seconds
   maxwait = 60*60
   totalwait = 0
   lvl2Out = None   
   while True:
      lvl2Out = findLvl2Output(opts.ptInput)
      if lvl2Out:
         break
      if totalwait>=maxwait:
         log.error("Timeout: Cannot find athenaMT output file")
         break
      totalwait += sleeptime
      log.info("Waiting for athenaMT job to finish [%d/%d] secs..." % (totalwait,maxwait))
      time.sleep(sleeptime)

   # Timeout
   if not lvl2Out: return 99

   # Assemble athenaPT command
   log.info("Using %s as athenaPT input file" % lvl2Out)
   athenaPTcmd = 'athenaPT.py %s -f %s' % (args,lvl2Out)   
   if opts.ptOptions: athenaPTcmd += " -F %s" % opts.ptOptions
   athenaPTcmd += " %s" % opts.jobOptions

   # Run athenaPT
   import subprocess as sp
   log.info("Executing '%s'" % athenaPTcmd)
   log.info("-"*70)
   status = sp.call(athenaPTcmd, shell=True)
   log.info("-"*70)
   
   # Delete athenaMT output file (to save RTT disk space)
   log.info("Deleting %s" % lvl2Out)
   os.remove(lvl2Out)
   
   return status


def findLvl2Output(ptInput):
   """Return the athenaMT output file (or None)"""
   import stat
   import fnmatch

   basedir = "../../runHLT_standaloneRTT"
   for d in os.listdir(basedir):
      path = os.path.join(basedir,d)
      if stat.S_ISDIR(os.stat(path)[stat.ST_MODE]):
         files = os.listdir(path)
         log.debug("Searching athenaMT output file '%s' in %s: %s" % (ptInput,path,files))
         lvl2Out = [f for f in files if fnmatch.fnmatch(f,ptInput)]
         if len(lvl2Out)>0: return os.path.join(path,lvl2Out[0])

   return None            

   
######################################################################
# Only executed standalone or after the os.exec below
######################################################################
if not 'theApp' in dir():
   sys.exit(main())

   
######################################################################
# We only get here if this script is run within athena
######################################################################

# Convert the -c athena options to real options to this script
arglist = ["athenaPTafterMT.py"]
for opt in parser.option_list:
   if opt.dest in dir():
      arglist.append("%s=%s" % (opt.get_opt_string(),eval(opt.dest)))

# Replace the current (athena) process by ourself.
# The exectuion will start again from the top of
# *this* file. Only this time in 'standalone' mode.

path = "TrigP1Test/"+arglist[0]
from AthenaCommon.Include import optionsPath
from AthenaCommon.Utils.unixtools import FindFile
path = FindFile(path,optionsPath,os.R_OK)

log.info("Executing %s %s" % (path," ".join(arglist)))
os.execvp(path,tuple(arglist))
