# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import logging
log = logging.getLogger(__name__)

def lsroot(dir):
   """Return list of all keys in 'dir' (recursively)"""

   import ROOT
   
   def dols(dir, keys):
      """Do the recursive traversal"""
      dirList = dir.GetListOfKeys()
      for k in dirList:
         kname = k.GetName()
         if k.GetClassName()=="TDirectoryFile" or k.GetClassName()=="TDirectory":
            dir.cd(kname)
            dols(ROOT.gDirectory, keys)
         else:
            keys += [dir.GetPath()+"/"+kname]
      
      dir.cd("..")
      return

   keys = []
   basedir = dir.GetPath().rstrip("/") + "/"
   dols(dir,keys)

   # Return sorted list with base directory removed
   return sorted([k.replace(basedir,"") for k in keys])


def check_file(path):
   import ROOT
   log.info('Checking file {}'.format(path))
   rfile = ROOT.TFile.Open(path, "READ")
   if not rfile:
      log.error('ERROR: {}: Failed to open file'.format(path))
      return 1
   if rfile.IsZombie():
      log.error('ERROR: {}: Zombie'.format(path))
      return 1
   if rfile.TestBit(ROOT.TFile.kRecovered):
      log.error('ERROR: {}: Recovered'.format(path))
      return 1
   rfile.Close()
   return 0

def check_file_subproc(path):
   # Open Root file in a clean subprocess and parse error messages
   from subprocess import Popen, PIPE
   import time
   time.sleep(5)
   # Need to check the file in a clean environment, such that the classes in memory don't hide problems with classes in the file
   p = Popen(['python', "-c", "from TrigValTools.TrigRootUtils import check_file; check_file('{}')".format(path)], stdout=PIPE, stderr=PIPE)
   # Wait till finished
   stdout, stderr = p.communicate()
   rc = p.returncode
   time.sleep(5)
   # Detect error messages printed by ROOT when the file is opened
   if "error" in stdout.decode('utf-8').lower() or "error" in stderr.decode('utf-8').lower():
      rc = 1
      log.error('ERROR: Detected errors when opening root file')

   log.error(stdout.decode('utf-8'))
   log.error(stderr.decode('utf-8'))
   return rc
