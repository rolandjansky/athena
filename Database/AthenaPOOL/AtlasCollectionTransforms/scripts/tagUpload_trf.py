#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## $Log: not supported by cvs2svn $
## Revision 1.12  2009/02/04 23:41:39  cranshaw
## Add omitted pickle report needed by T0
##
## Revision 1.11  2008/12/10 16:08:42  cranshaw
## Fix for T0
##
## Revision 1.10  2008/12/04 21:27:43  cranshaw
## More multithreading fixes
##
## Revision 1.9  2008/12/04 16:56:49  cranshaw
## multithreading fixes
##
## Revision 1.8  2008/12/04 16:44:25  cranshaw
## Add multithreading
##
## Revision 1.7  2008/12/04 15:46:02  cranshaw
## multithreading changes
##
## Revision 1.6  2008/12/02 22:00:35  cranshaw
## Removing archive scripts and changing defaultRelease=AtlasPOOL,takeFromEnv
##
## Revision 1.5  2008/11/13 14:26:04  cranshaw
## Merging branches for 14.4 and 14.5
##
## Revision 1.4.2.2  2008/11/05 14:14:20  cranshaw
## help and consolidation
##
## Revision 1.4.2.1  2008/11/05 13:52:29  cranshaw
## help changes
##
## Revision 1.4  2008/09/08 21:00:11  cranshaw
## Update requested by T0 ops on Sep/4/08
##
## Revision 1.3  2008/09/05 17:13:10  cranshaw
## - New transform DBInit
## - archive some utilities
## - progress on validation
##
## Revision 1.2  2008/08/27 20:10:50  cranshaw
## Add validation to upload. fix typo
##
## Revision 1.1  2008/08/13 15:29:40  cranshaw
## Cleanup of directory contents to more accepted norms
##
## Revision 1.1  2008/08/12 20:37:02  cranshaw
## Simplify transforms.
## - move meat of code into modules
## - rename transforms to *_trf
##
## Revision 1.1  2008/08/07 19:25:02  cranshaw
## Merged syntax for release for merge,upload,extract
## Atlas,AtlasPOOL,POOL are now arguments in scripts
##
## Revision 1.4  2008/08/06 16:03:50  cranshaw
## Fix naming problem. Remove cranshaw from setup paths.
##
## Revision 1.3  2008/08/05 16:12:55  cranshaw
## Fix tests
##
## Revision 1.2  2008/08/04 15:17:14  cranshaw
## Fix some ommissions
##
## Revision 1.1  2008/07/29 21:14:14  cranshaw
## Standardization, and first steps toward adding atn tests.
##
## Revision 1.3  2008/07/24 14:21:43  cranshaw
## Fixes for compilation
##
## Revision 1.3  2007/10/11 15:57:26  atlast0
## produce gpickle report
##
##
## @author $Author: cranshaw $
## @version $Revision: 1.13 $
##

#########################################################################
##
## Transformation for the Tier-0 TAG uploading
##
##  - input: 
##    a. single TAG file  
##    b. TAG DB output destination (optional)
##       - default is project_physics if (a) follows nomenclature, otherwise take file name - extension  
##    c. Atlas release (optional), e.g. 14.2.10,
##       - "takeFromEnv" will cause it to assume setup was done outside script
##    d. TAG DB connect string (optional)
##       - default is oracle://int8r/atlas_tags_loading_test
##  - uploads TAGs to dedicated database
##  - no output data files
##  - creates gpickle report file
##
## Uses Atlas (version = 14.0.0 + patches)
##
## (C) Jack Cranshaw, Luc Goossens, Armin Nairz  (September 2006, October 2007)
## (U) Jack Cranshaw (Sep 2008)
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time
# For restart and fork
import os, subprocess

from AtlasCollectionTransforms.tagCopyTool import *

defaultRelease='Atlas,takeFromEnv'
defaultConnection='oracle://atlas_tags/atlas_tags_loading_test'
defaultDest='default'

def help():

   # First test whether this is running on the web service
   #  if so, then grab VERSION from release area
   server = False

   resultMsg = "---------------------------------------------------------------------"
   if server:
     resultMsg += " UPLOAD SERVER HELP\n"
     resultMsg += "---------------------------------------------------------------------"
   print "tagUpload_trf.py from AtlasCollectionTransforms"

   if server: 
     cmd = "cat $EXTRACT_RELEASE_AREA/Database/AthenaPOOL/AtlasCollectionTransforms/cmt/version.cmt"
     (s,o) = commands.getstatusoutput(cmd)
     if s==0:
       resultMsg += "Version: " +str(o) +"\n"
     else:
       resultMsg += "Version not available from $EXTRACT_RELEASE_AREA/Database/AthenaPOOL/AtlasCollectionTransforms/version.cmt\n"
   # PURPOSE
   resultMsg += "\n"
   resultMsg += "This transform loads a ROOT Collection with all references and all metadata \n"
   resultMsg += "  into a relational TAG database.\n"
   #INTERFACE
   resultMsg += "tagUpload_trf.py <inputfile> <outputcoll> <connectstring>\n"
   resultMsg += "Inputs:\n"
   resultMsg += "  (0) PID: the actual value will be filled in by the server (default=0)\n"
   resultMsg += "  (1) Input name: full file name of ROOT Collection being loaded (required not null)\n"
   resultMsg += "  (2) Output name: name for target Relational Collection (required not null)\n"
   resultMsg += "  (3) Connection: coral connect string for relational db (default='"+str(defaultConnection)+"')\n"
   resultMsg += "---------------------------------------------------------------------\n"
   print resultMsg
   return resultMsg

def tagUpload(pid,argsmap) :

  # Initialize
  resultMsg = ""
  retcode = 0
  acronym = ""

  dtwrite =0
  nevt = 0
  retc_ld = 256
  level = 4
  txt1 = ''

  flist = [] 
  for f in argsmap['inputTagFile'] :
    (path,fn2) = os.path.split(f)
    fn2 += ".tmp"
    cmd = "cp -f " + f + " " + fn2
    print cmd 
    (s,o) = commands.getstatusoutput(cmd)
    flist.append(fn2)
  argsmap['inputTagFile'] = flist

  if (uploadFilesToUrls(argsmap)):

    # mc12 hack - move run number into cond run number
    try:
      if (argsmap['mc12_hack']):
        collMap = { 'RunNumber' : 'Cond_RunNumber' }
        for url in argsmap['urls_in'] :
          renameColumns(url, collMap)
    except: print "NOT doing mc12"

    # Execute copy using tool
    tool = tagCopyTool(argsmap)
    (retc_ld,copystate,dtcmt,dtwrite) = tool.multiTagCopy()
    resultMsg += "Copy Tool return values (retcode,dtwrite) = (%x,%s)\n" % (retc_ld, dtwrite)

    try:
      if (argsmap['mc12_hack']):
        annoMap = { 'RunNumber' : 'UNIT=NUMBER;GRP=TIME;' }
        for url in argsmap['urls_out'] :
          annotateColumns(url, annoMap)
    except: print "NOT doing mc12"

    # Print return codes as a summary
    resultMsg += "SUMMARY: retc_ld="+str(retc_ld)+"\n" 

    # Check the log file with tool
    (level,acronym,retcode,nevt) = tool.checkLog()

    if level == 4:
      resultMsg += "DID NOT ATTEMPT LOAD\n"
      acronym = "UPSTREAM_PROBLEM"
    elif level == 3:
      resultMsg += "FATAL encountered during load\n"
      acronym = "FATAL"
    elif level == 2:
      resultMsg += "ERROR encountered during load\n"
      acronym = "UNKNOWN"
    elif level == 1:
      resultMsg += "Transient problem encountered during load\n"
      acronym = "RECOVERABLE"
    elif level == 0:
      resultMsg += "FINAL LOAD SUCCEEDED\n"
      acronym = "OK"
  
    if level == 0 : 
      txt1 = "trf finished OK"
    else :
      txt1 = resultMsg 

  else:
    print "WARNING:: Check arguments"

  # Put summary into pickle file
  map = {'prodsys' : { 'jobOutputs': [],
                       'trfCode': retc_ld,
                       'trfAcronym': acronym,
                       'nevents': nevt,
                       'more': {'num1':dtwrite,'num2':nevt,'txt1':txt1}
                     }
        }

  f = open('jobReport.gpickle', 'w')
  pickle.dump(map, f)
  f.close()

  resultMsg += str(retcode) + " " + acronym + "\n"

  resultMsg += "done for " + pid
  print resultMsg
  global answer
  answer = {}
  answer[pid] = resultMsg

  return level


#########################################################################


if sys.argv[1] == '-h' or sys.argv[1]=='--help':
  help()
  sys.exit(-1)

# Pull files out of pickle file
import pickle, sys

fname=""
uploadmap = {}
uploadmap['inputTagFile'] = []
uploadmap.setdefault('dbCollection','default')
uploadmap.setdefault('dbConnect',defaultConnection)

# check for pickle input
if len(sys.argv) == 2 and sys.argv[1].startswith( '--argdict=') :
  fname=sys.argv[1][len( '--argdict='):]

  if (fname!=""):
    f = open(fname, 'r')
    uploadmap = pickle.load(f)
    f.close()
  else:
    print "pickle filename was empty"
    sys.exit(-1)

# or try command line parameters
else:

  uploadmap['inputTagFile'] += [sys.argv[1]]

  try : uploadmap['dbCollection'] = sys.argv[2]
  except : pass

  try : uploadmap['dbConnect'] = sys.argv[4]
  except : pass

  global answer
  answer = {}

# Now run the function defined above
tagUpload("0",uploadmap)   

