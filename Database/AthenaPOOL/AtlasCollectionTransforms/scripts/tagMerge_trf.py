#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## $Log: not supported by cvs2svn $
## Revision 1.6  2008/12/12 22:33:55  cranshaw
## Add fork and restart def's
##
## Revision 1.5  2008/12/04 21:27:43  cranshaw
## More multithreading fixes
##
## Revision 1.4  2008/12/04 16:56:49  cranshaw
## multithreading fixes
##
## Revision 1.3  2008/12/02 22:00:35  cranshaw
## Removing archive scripts and changing defaultRelease=AtlasPOOL,takeFromEnv
##
## Revision 1.2  2008/11/13 14:26:04  cranshaw
## Merging branches for 14.4 and 14.5
##
## Revision 1.1.2.2  2008/11/11 15:53:12  cranshaw
## add flexibility
##
## Revision 1.1.2.1  2008/11/05 14:14:19  cranshaw
## help and consolidation
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
## Revision 1.4  2008/08/05 16:12:55  cranshaw
## Fix tests
##
## Revision 1.3  2008/08/02 22:59:37  cranshaw
## Tests all exit with TRFOK, now, in devval
##
## Revision 1.2  2008/08/02 00:27:53  cranshaw
## Fix problems for 64 bit builds
## Move setups to python directory
## Declare python scripts
## First workable atn tests
##
## Revision 1.1  2008/07/31 14:26:01  cranshaw
## Add merge script. Tests nearly ready.
##
## Revision 1.1  2008/07/30 15:23:08  cranshaw
## see ChangeLog
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
## @version $Revision: 1.7 $
##

#########################################################################
##
## Transformation for the Tier-0 merging of TAG files
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
##  Example1: File would be loaded into table fdr08_run1_MinBias
##      % python ../tagMergeTest.py fdr08_run1.0003053.MinBias.merge.TAG.o1_r6_t2._0001.1 
##  Example2: File would be loaded into table t0tag
##      % python ../tagMergeTest.py fdr08_run1.0003053.MinBias.merge.TAG.o1_r6_t2._0001.1 t0tag 
## 
## (C) Jack Cranshaw, Luc Goossens, Armin Nairz 
##     (September 2006, October 2007, July 2008)
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time, subprocess

from AtlasCollectionTransforms.tagMerge import *

defaultRelease='AtlasPOOL,takeFromEnv'
defaultAtts = "*"

def restart():
 print "restarting ..."
 subprocess.Popen("python tagMerge_trf.py", shell=True)

def fork(port):
 print "forking on " + port + " ..."
 os.environ["ATHENAEUM_PORT"] = port
 subprocess.Popen("python tagMerge_trf.py", shell=True)

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
     resultMsg += " MERGE SERVER HELP"
     resultMsg += "---------------------------------------------------------------------"
   resultMsg += "tagMerge_trf.py from AtlasCollectionTransforms"

   if server: 
     cmd = "cat $EXTRACT_RELEASE_AREA/Database/AthenaPOOL/AtlasCollectionTransforms/cmt/version.cmt"
     (s,o) = commands.getstatusoutput(cmd)
     if s==0:
       resultMsg += "Version:" + str(o) + "\n"
     else:
       resultMsg += "Version not available from $EXTRACT_RELEASE_AREA/Database/AthenaPOOL/AtlasCollectionTransforms/version.cmt\n"
   # PURPOSE
   resultMsg += "\n"
   resultMsg += "This transform merges two ROOT Collections. \n"
   #INTERFACE
   resultMsg += "tagMerge_trf.py <inputfile> <outputcoll> <releaseconfig>\n"
   resultMsg += "Inputs:\n"
   resultMsg += "  (0) PID: the actual value will be filled by the server (default=0)\n"
   resultMsg += "  (1) Input name: full file name of ROOT Collection being loaded (required not null)\n"
   resultMsg += "  (2) Output name: name for target Relational Collection (required not null)\n"
   resultMsg += "  (3) Release: comma separated info on which release to use (default='"+defaultRelease+"')\n"
   resultMsg += "---------------------------------------------------------------------\n"
   print resultMsg
   return resultMsg

def tagMerge(pid,infile, outdest, release=defaultRelease, query="", atts=defaultAtts) :

   resultMsg = "arguments = %s,%s,%s\n" % (infile, outdest, release)
   fileList = infile.split(",")
   for file in fileList:
      (retcode, acronym, txt, nevt, dttagdb) = mergeTagDB(file, outdest, release, query, atts)
      resultMsg += "TagDB return values (retcode,acronym,file,txt,nevt,dttagdb) = (%s,%s,%s,%s,%s,%s)\n" % (retcode, acronym, file, txt, nevt, dttagdb)

   resultMsg += "done for " + str(pid) +"\n"
   print resultMsg
   global answer
   answer = {}
   answer[pid] = resultMsg


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

  if sys.argv[1] == '-h' or sys.argv[1]=='--help':
    help()
    sys.exit(-1)

  # Pull files out of pickle file
  import pickle, sys

  fname=""
  if len(sys.argv) == 2 and sys.argv[1].startswith( '--argdict=') :
    print "Found parameters indicating using pickled input"
    fname=sys.argv[1][len( '--argdict='):]

    if (fname!=""):
      f = open(fname, 'r')
      map = pickle.load(f)
      f.close()
    else:
      print "pickle filename was empty"
      sys.exit(-1)

    tagfile = map['inputTagFiles']

    destination = map['outputTagFile']

    release = 'AtlasPOOL,takeFromEnv'
    try : release = map['releaseConfig']
    except : pass

    query=""
    try : query=map['query']
    except : pass

    tagMerge(tagfile, destination, release, query)   

  else:

    print "Using command line parsing"

    narg=2

    tagfile = sys.argv[1]

    destination = sys.argv[2]

    # Make default POOL utility from env
    release = 'AtlasPOOL,takeFromEnv'
    try : 
      release = sys.argv[3]
      narg=3
    except : pass

    query=""
    try : 
      query=sys.argv[4]
      narg=4
    except : pass

    if narg==2 :
      tagMerge("0", tagfile, destination)   
    elif narg==3 :
      tagMerge("0", tagfile, destination, release)   
    elif narg==4 :
      tagMerge("0", tagfile, destination, release, query)   

