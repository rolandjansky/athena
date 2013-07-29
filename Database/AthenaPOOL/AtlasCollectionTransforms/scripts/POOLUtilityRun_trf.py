#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## $Log: not supported by cvs2svn $
## Revision 1.1  2008/11/21 15:09:26  cranshaw
## Add POOLUtilityRun transform
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
## @version $Revision: 1.2 $
##

#########################################################################
##
## Transformation for POOL Run
##
## (C) Jack Cranshaw 
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time, subprocess

from AtlasCollectionTransforms.POOLUtilityRun import *

defaultRelease='Atlas,takeFromEnv'

def restart():
 print "restarting ..."
 subprocess.Popen("python POOLUtilityRun_trf.py", shell=True)

def fork(port):
 print "forking on " + port + " ..."
 os.environ["ATHENAEUM_PORT"] = port
 subprocess.Popen("python POOLUtilityRun_trf.py", shell=True)

def help():

   # First test whether this is running on the web service
   #  if so, then grab VERSION from release area
   server = True
   try: 
      borg = os.environ['TAGEXSRV']
   except:
      server=False

   print "---------------------------------------------------------------------"
   if server:
     print " POOLUTILITYRUN SERVER HELP"
     print "---------------------------------------------------------------------"
   print "POOLUtilityRun_trf.py from AtlasCollectionTransforms"

   if server: 
     cmd = "cat $EXTRACT_RELEASE_AREA/Database/AthenaPOOL/AtlasCollectionTransforms/cmt/version.cmt"
     (s,o) = commands.getstatusoutput(cmd)
     if s==0:
       print "Version:",o
     else:
       print "Version not available from $EXTRACT_RELEASE_AREA/Database/AthenaPOOL/AtlasCollectionTransforms/version.cmt"
   # PURPOSE
   print ""
   print "This transform runs a POOL utility. "
   #INTERFACE
   print "POOLUtilityRun_trf.py <utilname> <xmlfile> <release>"
   print "Inputs:"
   print "  (1) Utility name (required not null)"
   print "  (2) XML File name: configuration file for utility (default=<utilname>+.exe.xml)"
   print "  (3) Release Config (default=", defaultRelease, ")"
   print "---------------------------------------------------------------------"

def POOLRun(utilname, xmlFName,release=defaultRelease) :

  print "arguments = ",utilname, xmlFName
  (retcode, acronym, dttagdb) = POOLUtilityRun(utilname, xmlFName, release)
  print "POOLUtilityRun return values (retcode,acronym,dttagdb) = (%s,%s,%s)" % (retcode, acronym, dttagdb)

  print "done"


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
  narg=3
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

    util = map['utilname']

    try:
      xml = map['xmlfile']
    except: 
      xml = util + ".exe.xml"

    try:
      rel = map['release']
    except: 
      narg -= 1

    if narg==3: POOLRun(util,xml,rel)   
    else: POOLRun(util,xml)

  else:

    print "Using command line parsing"

    util = sys.argv[1]

    try:
      xml = sys.argv[2]
    except: 
      xml = util + ".exe.xml"

    try:
      rel = sys.argv[3]
    except: 
      narg -= 1

    print "Final narg = ",narg

    if narg==3: 
      POOLRun(util,xml,rel)   
    else: 
      POOLRun(util,xml)

