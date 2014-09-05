#!/usr/bin/env python
#
# @file: athenaMTPT.py
# @purpose: Runs athenaMT -> athenaPT in a chain
# @author: Frank Winklmeier
#
# This script can be run in two ways:
# 1) Standalone
#    athenaMTPT.py --help for instructions
#    Example: athenaMTPT.py --l2Input='athenaMT.data' --jobOptions='myJobOpts.py'
#
# 2) As input to athena.py (for RTT)
#    Parameters that are listed under '--help' can be passed via the
#    the -c option (without the leading '--')
#    Example: athena -c"l2Input='athenaMT.data';jobOptions='myJobOpts.py'" athenaMTPT.py
#

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import sys
import os
import optparse
import logging
import re
from TrigP1Test.TestUtils import JobRecord, printJobSummary

# Populate the option parser (but don't parse yet)

parser = optparse.OptionParser(description="Chain athenaMT/PT jobs")

parser.add_option("--l2Input",
                  action = "store",
                  help = "BS input file for athenaMT")

parser.add_option("--jobOptions",
                  action = "store",
                  help = "job options for athenaMT/PT")

parser.add_option("--mtOptions",
                  action = "store",
                  help = "option file for athenaMT (via -F)")

parser.add_option("--ptOptions",
                  action = "store",
                  help = "option file for athenaPT (via -F)")

parser.add_option("--args",
                  action = "store",
                  help = "remaining arguments for athenaMT/PT")

logging.basicConfig(level=logging.INFO,
                    format="--> %(name)-5s %(asctime)s %(levelname)7s %(message)s",
                    datefmt="%Y-%m-%d %H:%M:%S")
   
log = logging.getLogger("athenaMTPT")

      
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
   
   l2Output = "Lvl2"
   l2OutputRE = "^"+l2Output+".*\.data"    # regexp to match athenaMT output
   mtLog = "athenaMT.log"
   ptLog = "athenaPT.log"
   
   athenaMTcmd = 'athenaMT.py %s -o %s' % (args,l2Output)
   
   if opts.l2Input:   athenaMTcmd += " -f %s" % opts.l2Input
   if opts.mtOptions: athenaMTcmd += " -F %s" % opts.mtOptions

   athenaMTcmd += " %s" % opts.jobOptions

   jobs = []
   jobs += [ call(athenaMTcmd, mtLog, "MT", ignore=[l2OutputRE]) ]

   # Get a list of athenaMT output files (in case there are more than one)
   files = os.listdir(os.getcwd())
   files = filter(lambda f:(re.match(l2OutputRE,f)!=None),files)

   if len(files)==0:
      log.fatal("Cannot find athenaMT output file.")
      return 1
   
   # Sort chronologically and use the last one created as input to athenaPT
   files.sort(key=lambda f:(os.stat(f).st_mtime,f))
   
   efInput  = files[-1]
   athenaPTcmd = 'athenaPT.py %s -f %s' % (args,efInput)   
   if opts.ptOptions: athenaPTcmd += " -F %s" % opts.ptOptions
   athenaPTcmd += " %s" % opts.jobOptions
   
   jobs += [ call(athenaPTcmd, ptLog, "PT") ]

   # Print summary and pickel job records
   printJobSummary(jobs,log)

   import pickle

   log.info("Pickling job records to %s" % JobRecord.summaryFile)
   pickle.dump(jobs,open(JobRecord.summaryFile,"wb"),-1)
   
   return sum([j.status for j in jobs])


def call(cmd, logFile, alias, dbg = False, ignore = []):
   """
   Execute the given command and write output to logFile.
   All output files produced by the job are renamed to 'file.alias'.
   'ignore' contains a list of regular expressions that should be ignored
   in the renaming.
   Returns a JobRecord object.
   """
   
   import subprocess as sp
   import time
   
   job = JobRecord()
   job.alias = alias
   
   log.info("-"*70)
   log.info("Executing '%s'" % (cmd))
   log.info("-"*70)
   if dbg: return job

   # List of files before execution
   beforeExec = set(os.listdir(os.getcwd()))

   t1 = time.time()
   job.status = sp.call("%s 2>&1 | tee %s" % (cmd,logFile), shell=True)
   job.walltime = time.time() - t1
   job.logFile = logFile

   # List of files after execution
   afterExec = set(os.listdir(os.getcwd()))
   
   msg = "Job '%s' finished. Return code = %d (Output: %s)" % (alias,job.status,logFile);
   log.info("-"*70)
   if job.status!=0: log.error(msg)
   else:             log.info(msg)

   beforeExec.add(logFile)      # never rename the log file
   newFiles = afterExec.difference(beforeExec)

   for f in newFiles:

      if sum([re.match(pat,f)!=None for pat in ignore])>0: continue
      
      l = list(os.path.splitext(f))
      l.insert(-1,"."+alias)   # add .alias before extension
      newName = "".join(l)
      log.info("Renaming %s -> %s" % (f,newName))
      os.system("mv %s %s" % (f,newName))
      job.files += [newName]
      
   log.info("-"*70)
      
   return job



   
######################################################################
# Only executed standalone or after the os.exec below
######################################################################
if not 'theApp' in dir():
   sys.exit(main())

   
######################################################################
# We only get here if this script is run within athena
######################################################################

# Convert the -c athena options to real options to this script
arglist = ["athenaMTPT.py"]
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
