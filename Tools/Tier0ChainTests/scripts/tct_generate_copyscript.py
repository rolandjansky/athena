#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# usage: tct_GenerateCopyScript <samplefile>

import sys
import os
import string, commands
import random
from datetime import datetime
from Tier0ChainTests.CastorFileTool import CastorFileTool

fallbackfile = []
fallbackfile.append("/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_cos.00135664.physics_IDCosmic.daq.RAW._lb0000._SFO-2._0001.data")
fallbackfile.append("/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_cos.00135664.physics_IDCosmic.daq.RAW._lb0000._SFO-2._0002.data")
fallbackfile.append("/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/data09_cos.00135664.physics_IDCosmic.daq.RAW._lb0000._SFO-2._0003.data")

def GenerateCopyLinkScript(jobindex,castorsubdir,nfilesperjob,castorbasedir,useBaseSubdirs,lumiblock):

  timenow = "## Auto-generated at: %s\n\n" % datetime.today().strftime("%d%b%y.%Hh%M")

  outfile = "copylinkscript.sh"
  inStream = ["physics"]
  nJobsMax = 5 
  tmpdir = "${WORKDIR}/"
  linkdir = tmpdir

  ## Castor file retrieval 
  ## tool searches for (nfilesperjob*nJobsMax) files in castor dir: castorbasedir/castorsubdir
  filetool = CastorFileTool(baseCastorDir=castorbasedir, inStream=inStream, nJobs=nJobsMax,
                            tmpdir=tmpdir, linkdir=linkdir, useBaseSubdirs=useBaseSubdirs, 
                            nFilesPerJob=nfilesperjob, runSubdirToSearch=castorsubdir,
                            lumiBlock = lumiblock)
  filetool.init()
  dict = filetool.GetNextJob(jobIdx=jobindex,addFilesEndOfRun=False)

  ## generate copy & link script
  filemap = {}
  file = open(outfile,'w')
  file.write("#!/bin/sh\n")
  file.write(timenow)
  file.write("echo \"STAGE_SVCCLASS is set to ${STAGE_SVCCLASS}\"\n")
  file.write("echo \"STAGE_HOST is set to ${STAGE_HOST}\"\n")
  file.write("echo \"RFIO_USE_CASTOR_V2 is set to ${RFIO_USE_CASTOR_V2}\"\n")
  file.write("echo \">> Now copying file(s) from Castor. This may take some time.\"\n")
  ## copying
  for line in dict["cpCommands"]:
    file.write(line + "\n")
  file.write("\nTEST=\"OK\"\n")
  ## testing all files exist
  for i in range(len(dict["linkFiles"])):
    linkFile = linkdir + dict["linkFiles"][i]
    file.write("if [ ! -f %s ] ; then\n" % linkFile)
    file.write("  echo file %s was not copied properly\n" % linkFile)
    file.write("  TEST=\"NO\"\nfi\n")
  ## linking & fallback
  file.write("if [ \"IS${TEST}\" = \"ISOK\" ] ; then\n")
  file.write("  echo All files copied from castor are ok !\n")
  for i in range(len(dict["linkFiles"])):
    linkFile = linkdir + dict["linkFiles"][i]
    toFile = "%d.data" % i
    rmcmd = "rm -f %s" % (toFile)
    file.write("  %s ; ln -s %s %s\n" % (rmcmd,linkFile,toFile))
    filemap[toFile] = dict["jobFileList"][i]
  file.write("else\n")
  file.write("  echo WARNING: Some files copied from castor were corrupted !\n")
  for i in range(len(dict["linkFiles"])):
    randFile = random.choice(fallbackfile)
    toFile = "%d.data" % i
    rmcmd = "rm -f %s" % (toFile)
    file.write("  %s ; ln -s %s %s\n" % (rmcmd,randFile,toFile))
  file.write("fi\n")
  file.write("\n")
  file.close()

  cmd = "chmod +x %s" % outfile 
  os.system(cmd)

  ## file map:
  print "\nPrincipal file dictionary:"
  print filemap
  print ""

  return

## main
if __name__ == "__main__":

  usageline = "usage: " + sys.argv[0] + " <jobindex> [<castorsubdir>] [<nfilesperjob>] [<castorbasedir>] [<findlatestcastordir>] [<lumiblock>]"
  if (len(sys.argv)<2):
    print usageline
    sys.exit(1)

  ## job index
  jobindex = int(sys.argv[1])

  ## castor subdir to search
  castorsubdir = "physics_IDCosmic"
  if (len(sys.argv) >= 3):
    castorsubdir = sys.argv[2]

  ## number of files per job
  nfilesperjob = 3
  if (len(sys.argv) >= 4):
    nfilesperjob = int(sys.argv[3])

  ## castor base directory to search from
  castorbasedir = "/castor/cern.ch/grid/atlas/DAQ/2008/91890"
  if (len(sys.argv) >= 5):
    castorbasedir = sys.argv[4]

  ## search in castor subdirectories
  useBaseSubdirs = False
  if len(sys.argv)>= 6:
    if sys.argv[5]=="True": useBaseSubdirs = True

  ## select a given lumi block
  lumiblock = None
  if len(sys.argv)>=7: lumiblock = sys.argv[6]

  ## generate copy script
  GenerateCopyLinkScript(jobindex,castorsubdir,nfilesperjob,castorbasedir,useBaseSubdirs,lumiblock)

