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
defaultDest='default'

def restart():
 print "restarting ..."
 subprocess.Popen("python tagCopy_trf.py", shell=True)

def fork(port):
 print "forking on " + port + " ..."
 os.environ["ATHENAEUM_PORT"] = port
 subprocess.Popen("python tagCopy_trf.py", shell=True)

def help():

   # First test whether this is running on the web service
   #  if so, then grab VERSION from release area
   server = True
   try: 
      borg = os.environ['TAGEXSRV']
   except:
      server=False

   resultMsg = "---------------------------------------------------------------------"
   if server:
     resultMsg += " COPY SERVER HELP\n"
     resultMsg += "---------------------------------------------------------------------"
   print "tagCopy_trf.py from AtlasCollectionTransforms"

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
   resultMsg += "tagCopy_trf.py <inputfile> <outputcoll> <connectstring>\n"
   resultMsg += "Inputs:\n"
   resultMsg += "  (0) PID: the actual value will be filled in by the server (default=0)\n"
   resultMsg += "  (1) Input URL list: list of url's of inputs Collection (required not null)\n"
   resultMsg += "  (2) Output URL: url for target Collection (required not null)\n"
   resultMsg += "  (4) Run List: comma separated list of runs in quotes, e.g. ('1,2,3,4')\n"
   resultMsg += "---------------------------------------------------------------------\n"
   print resultMsg
   return resultMsg

def tagCopy(pid, copymap) :

  # Open output pickle file
  f = open('jobReport.gpickle', 'w')

  # Initialize
  resultMsg = ""
  retcode = 0
  acronym = ""

  dtwrite = 0
  nevt = 0
  retc_ld = 256
  level = 4

  #resultMsg += "Second, load the file into the database\n"
  copymap['query'] = "RunNumber in ("+copymap['runList']+")"
  tool = tagCopyTool(copymap)
  (retc_ld, output_ld, dtcmt, dtwrite) = tool.multiTagCopy()
  resultMsg += "multiTagCopy return values (retcode,dtwrite) = (%x,%s)\n" % (retc_ld, dtwrite)

  # Print return codes as a summary
  resultMsg += "SUMMARY: retc_ld="+str(retc_ld)+"\n" 

  # Check the log file
  (level,acronym,retcode,nevt) = tool.checkLog()

  txt1 = ''
  if level == 4:
    resultMsg += "DID NOT ATTEMPT COPY\n"
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
    resultMsg += "FINAL COPY SUCCEEDED\n"
    acronym = "OK"
  
  if level == 0 : 
    txt1 = "trf finished OK"
  else :
    txt1 = resultMsg  

  dttot = dtwrite
  map = {'prodsys' : {'jobOutputs': [],
                      'trfCode' : retc_ld,
                      'trfAcronym' : acronym,
                      'nevents': nevt,
                      'more' : {'num1': dttot, 'num2': nevt, 'txt1':txt1}
                     }
        }

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

tagexsrv = True
try:
  borg = os.environ['TAGEXSRV']
except: 
  tagexsrv = False

if tagexsrv:
  argumentInteractiveServerNoAlgTools = True
  argumentInteractiveServerServerPort = os.environ["ATHENAEUM_PORT"]
  argumentInteractiveServerServerKey = "insider"
  execfile ("InteractiveServer.py")
else:

  copymap = {}
  copymap['urls_in'] = []
  copymap['urls_out'] = []
  copymap.setdefault('runList','')
  copymap.setdefault('query','')

  if sys.argv[1] == '-h' or sys.argv[1]=='--help':
    help()
    sys.exit(-1)

  # Pull files out of pickle file
  import pickle, sys

  fname=""
  pid = 0

  if len(sys.argv) == 2 and sys.argv[1].startswith( '--argdict=') :
    fname=sys.argv[1][len( '--argdict='):]

    if (fname!=""):
      f = open(fname, 'r')
      copymap = pickle.load(f)
      f.close()
    else:
      print "pickle filename was empty"
      sys.exit(-1)

  else:

    copymap['urls_in'] += [sys.argv[1]]

    try : copymap['urls_out'] = sys.argv[2]
    except : pass

    try : copymap['runList'] = sys.argv[3]
    except : pass

  global answer
  answer = {}

  tagCopy("0",copymap)

