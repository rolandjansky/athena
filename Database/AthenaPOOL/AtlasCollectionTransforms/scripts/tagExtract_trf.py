#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Transformation for the Tier-0 TAG extraction form RDB to file
##
##  - input:
##    a. Colon separated list of input collections
##    b. Output file name
##       - assumed to be PFN, so don't drop the .root
##    c. Query to be executed on all input collections
##    d. Lumi string to be added to output file metadata
##    e. Atlas release (optional), e.g. 14.2.10,
##       - "takeFromEnv" will cause it to assume setup was done outside script
##    f. TAG DB connect string (optional)
##       - default is oracle://int8r/atlas_tags_loading_test
##
##  Example1: File would include events from collections fdr08_run1_MinBias
##      and fdr08_run1_Muon with run number > 0.
##      % ExtractAtlas.py fdr08_run1_MinBias:fdr08_run1_Muon mydir/myevents.root 'RunNumber>0'
##
## Uses Atlas (version = 13.0.0 + patches)
##
## (C) Jack Cranshaw (Sep 2008)
## (U) Julius Hrivnac (Dec 2008) WWW: http://cern.ch/hrivnac - Blog: http://hrivnac.free.fr/wordpress
##
#########################################################################

import sys, string, commands, os.path, os, pickle, time, urllib, shutil

from socket import getfqdn

from AtlasCollectionTransforms.tagExtract_ATN import *

defaultLumi=''
defaultRelease='AtlasPOOL,takeFromEnv'
defaultConnection='oracle://atlas_tags/atlas_tags_csc_fdr'
defaultLumiencoded = False
defaultTarget = ''
defaultAtts = "*"
defaultXML = ''

try:
  port = os.environ["ATHENAEUM_PORT"]
except:
  port = 0

script = os.path.basename(sys.argv[0])

logFile      = "/tmp/Athenaeum/" + str(script) + ":" + str(port) + ".log"
progressFile = "/tmp/Athenaeum/" + str(script) + ":" + str(port) + ".progress"

# Write the status of the running job into a progress file.
###########################################################
def setProgress(percentage, message):
  f = open(progressFile, "w")
  f.write(str(percentage) + ";" + message)
  f.close()

# Retunr the callimg help.
##########################
def help():

   # First test whether this is running on the web service
   #  if so, then grab VERSION from release area
   server = True
   try: 
      borg = os.environ['TAGEXSRV']
   except:
      server=False

   resultMsg = "---------------------------------------------------------------------\n"
   if server:
     resultMsg += " EXTRACT SERVER HELP\n"
     resultMsg += "---------------------------------------------------------------------\n"
   print "tagExtract_trf.py from AtlasCollectionTransforms"

   if server: 
     cmd = "cat $EXTRACT_RELEASE_AREA/Database/AthenaPOOL/AtlasCollectionTransforms/cmt/version.cmt\n"
     (s,o) = commands.getstatusoutput(cmd)
     if s==0:
       resultMsg += "Version: " + str(o) + "\n"
     else:
       resultMsg += "Version not available from $EXTRACT_RELEASE_AREA/Database/AthenaPOOL/AtlasCollectionTransforms/version.cmt\n"
   # PURPOSE
   resultMsg += "\n"
   resultMsg += "This transform extracts a ROOT Collection with all references and all metadata "
   "  from a relational TAG database for use as input to an analysis job.\n"
   #INTERFACE
   resultMsg += "Inputs:\n"
   resultMsg += "  (0) pid: the actual value will be filled in by the server\n"
   resultMsg += "  (1) Output name: full file name for a ROOT Collection (required not null)\n"
   resultMsg += "  (2) Query: string in sql syntax to be used in where clause (required)\n"
   resultMsg += "  (3) Input name: name of Relational Collection being queried (required not null)\n"
   resultMsg += "  (4) Lumi info: string containg metadata used for luminosity calculation (default='" + str(defaultLumi) + "')\n"
   resultMsg += "  (5) Release: comma separated info on which release to use (default='" + str(defaultRelease) + "')\n"
   resultMsg += "  (6) Connection: coral connect string for relational db (default='" + str(defaultConnection) + "')\n"
   resultMsg += "  (7) Lumiencoded: Denote whether the Lumi info argument is urlencoded (default=" + str(defaultLumiencoded) + ")\n"
   resultMsg += "  (8) Target directory: directory for the created Pool/Root file (default='" + str(defaultTarget) + "')\n"
   resultMsg += "  (9) Attributes: comma separated list of attributes to include in output (default=*)\n"
   resultMsg += "---------------------------------------------------------------------\n"
   print resultMsg
   sys.stdout.flush()
   return resultMsg

# Perform the extraction job.
# Return HTML fragment with the status.
######################################################
def Extract(pid,outdest,query,collRead,lumi=defaultLumi,release=defaultRelease,srcconnect=defaultConnection,lumiencoded=defaultLumiencoded,target=defaultTarget,atts=defaultAtts) :

  tagexsrv = True
  try:
    borg = os.environ['TAGEXSRV']
  except:
    tagexsrv = False

  if tagexsrv: 
     setProgress(50, "extracting ...")

  print "Extract(\"" + str(pid) + "\", \"" + str(outdest) + "\", \"" + str(query) + "\", \"" + str(collRead) + "\", \"" + str(lumi)  + "\", \"" + str(release) + "\", \"" + str(srcconnect) + "\", \"" + str(lumiencoded) + "\", \"" + str(target) + "\", \"" + str(atts) + "\")"
  start = time.time()

  collList = collRead.split(":")
  print collList

  f = open('report_'+str(pid)+'.pickle', 'w')

  if (lumiencoded): lumi = urllib.unquote_plus(lumi)

  #for collname in collList:
    #(retcode, acronym, nevt, dttagdb) = TAGExtract(collname, outdest, query, lumi, release, srcconnect)

  (retcode, acronym, nevt, dttagdb) = TAGExtract(collRead, outdest, query, lumi, release, srcconnect,atts)
  print "TagDB return values (retcode,acronym,nevt,dttagdb) = (%s,%s,%s,%s)" % (retcode, acronym, nevt, dttagdb)

  # info for report pickle file
  map = {'tagwebs': {'trfcode':retcode,
                     'trfacronym':acronym,
                     'trfcode':retcode,   
                     'metadata': {'dtime':dttagdb, 'Nevt':nevt, 'input':collRead, 'outfiles':outdest}
                    }
        }
  pickle.dump(map, f)

  f.close()

  if tagexsrv:
    if target != "":
      setProgress(90, "moving created file ...")
      shutil.move(outdest, target + "/" + outdest)
      outdest = target + "/" + outdest

  resultMsg = "Extraction has finished for client " + str(pid) + "\n"
  resultMsg += "return values (retcode, acronym, nevt, dttagdb) = (%s,%s,%s,%s)" % (retcode, acronym, nevt, dttagdb) + "\n"
  resultMsg += str(nevt) + " events written into " + str(outdest)
  print resultMsg
  if tagexsrv: setProgress(100, "finished")
  return resultMsg

# Start the extraction job asynchronously.
# Return HTML fragment with the link to the status page.
########################################################
def ExtractAsync(pid,outdest,query,collRead,lumi=defaultLumi,release=defaultRelease,srcconnect=defaultConnection,lumiencoded=defaultLumiencoded,target=defaultTarget,xmlout=defaultXML) :

  tagexsrv = True
  setProgress(50, "extracting ...")

  print "ExtractAsync(\"" + str(pid) + "\", \"" + str(outdest) + "\", \"" + str(query) + "\", \"" + str(collRead) + "\", \"" + str(lumi)  + "\", \"" + str(release) + "\", \"" + str(srcconnect) + "\", \"" + str(lumiencoded) + "\", \"" + str(target) + "\")"
  start = time.time()

  if (lumiencoded): lumi = urllib.unquote_plus(lumi)

  resultMsg = ""
  (retcode, acronym, nevt, dttagdb) = TAGExtractAsync(collRead, outdest, query, lumi, release, srcconnect)
  resultMsg += "Extraction has started for client " + str(pid) + "<br>"
  if target == "":
    resultMsg += "events will be written into " + outdest + "<br>"
  else:
    resultMsg += "events will be written into " + target + "/" + outdest + "<br>"
  resultMsg += "the result file will be available <a href='http://cern.ch/Athenaeum/GetFile.jsp?url=http://" + getfqdn() + ":" + port + "&key=insider&filename=" + outdest + "&copyto=" + target + "' target='_blank'>here</a><br>"

  print resultMsg
  setProgress(100, "working in background")
  return resultMsg

# Check the existence of filename.status file, which is created when
# the extraction job has finished.
# Analyse the log files (.log, .out, .err).
# Return appropriate answer as an HTML fragment.
#####################################################################
def getFile(filename, copyto):
  if os.path.exists(filename + ".status"):
    (status, log) =  commands.getstatusoutput("cat " + filename + ".log")
    retcode1 = 0
    (retcode2, msg) = commands.getstatusoutput("grep -i 'error:' " + filename + ".log")
    retcode = 0
    nevt = -1
    dttagdb = 0
    acronym = 'OK'
    bad1 = (retcode1 != 0)
    bad2 = (retcode2 != 256 and retcode2 != 0)
    bad3 = msg != ''
    print bad1," ",bad2," ",bad3
    bad = bad1 or bad2 or bad3
    if bad:
      print "ERROR: Setup or job execution problem!" + str(bad1) + str(bad2) + str(bad3)
      retcode = 62600
      acronym = 'EXEPROBLEM'
    else:
      cmd = "grep \"total events appended to each output\" " + filename + ".log"
      (status, output) = commands.getstatusoutput(cmd)
      if status == 0 : nevt = int(output.split()[1])
      if copyto != "":
        shutil.move(filename, copyto + "/" + filename)
        filename = copyto + "/" + filename
    resultMsg  = "<b>TagDB return values (retcode,acronym,nevt,dttagdb) = (%s,%s,%s,%s)</b><br>" % (retcode, acronym, nevt, dttagdb)
    resultMsg += "created file: " + filename + "<br>"
    resultMsg += "<u>log:</u><br><em>" + log + "</em>"
    return resultMsg
  else:
    return "File " + filename + " doesn't exist yet - <a href='http://cern.ch/Athenaeum/GetFile.jsp?url=http://" + getfqdn() + ":" + port + "&key=insider&filename=" + filename + "&copyto=" + copyto + "'>try later</a>"

# Return the most recent log file.
##################################
def log():
  f = open(logFile, "r")
  text = logFile + ":\n---------------------------------------------------\n" + f.read()
  return text

# Return the progressFile, containing the status of the running job.
####################################################################
def progress():
  f = open(progressFile)
  text = f.read()
  return text

tagexsrv = True
try:
  borg = os.environ['TAGEXSRV']
except:
  tagexsrv = False

if tagexsrv:
  argumentInteractiveServerNoAlgTools = True
  argumentInteractiveServerServerPort = port
  argumentInteractiveServerServerKey = "insider"
  execfile ("InteractiveServer.py")
else:

  # set variables based on cli arguments

  if sys.argv[1] == '-h' or sys.argv[1]=='--help':
    help()
    sys.exit(0)

  collRead = sys.argv[1]

  outdest = sys.argv[2]

  query = sys.argv[3]

  narg=len(sys.argv)-1
  print "narg = ",narg

  lumi = ''
  try : 
    lumi = sys.argv[4]
  except : pass

  release = defaultRelease
  try : release = sys.argv[5]
  except : pass

  srcconnect = 'oracle://int8r_lb/atlas_tags_csc_fdr'
  try : srcconnect = sys.argv[6]
  except : pass

  encoded = defaultLumiencoded
  try : encoded = sys.argv[7]
  except : pass

  target = "/tmp"
  try : target = sys.argv[8]
  except : pass

  ratts = "*"
  try : ratts = sys.argv[9]
  except : pass

  global answer
  answer = {}

  if narg==3:
    Extract("0",outdest,query,collRead)
  if narg==4:
    Extract("0",outdest,query,collRead,lumi)
  if narg==5:
    Extract("0",outdest,query,collRead,lumi,release)
  if narg==6:
    Extract("0",outdest,query,collRead,lumi,release,srcconnect)
  if narg==7:
    Extract("0",outdest,query,collRead,lumi,release,srcconnect,encoded)
  if narg==8:
    Extract("0",outdest,query,collRead,lumi,release,srcconnect,encoded,target)
  if narg==9:
    Extract("0",outdest,query,collRead,lumi,release,srcconnect,encoded,target,ratts)

  #print answer["0"]

