#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys,string,commands
from datetime import datetime
import pickle
import shutil

def makePickle(argdict,outpickle):
  file = open(outpickle,'w')
  pickle.dump(argdict,file)
  file.close()
  return

def searchLogfiles(logArr):

  ## strings to search for in logfile(s)
  successArr = ['leaving with code 0','ErrorCode=0 (OK)','successful run']
  failureArr = ['FATAL','Core dump','stack trace','Shortened traceback','traceback']

  print "Searching for success/failure strings: "
  #print successArr ## printing this gives fake errors when scanning logfiles!
  #print failureArr

  ## default success is False
  ## if no search strings found, status stays False
  myOk = True

  for log in logArr:
    allOk = False
    ## logical OR of success search strings
    for search in successArr:
      cmd = 'grep "%s" %s' % (search,log)
      s,o = commands.getstatusoutput(cmd)
      if len(o)>0:
        #print o
        allOk = True
        break
    ## logical AND of failure search strings. These overwrite possible earlier success.
    for search in failureArr:
      cmd = 'grep "%s" %s' % (search,log)
      s,o = commands.getstatusoutput(cmd)
      if len(o)>0:
        print ">> Found ERROR string : %s" % o
        allOk = False
        break
    myOk = (allOk and myOk)
    print "> Scanned through log file: %s ... Ok ? %s" % (log,allOk)

  return myOk


def printTimestamp(timestampfile,outpickle,tocpfile,
		   kitbasedir="/afs/cern.ch/atlas/software/builds/kitrel/nightlies/",
		   webbasedir="http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/kitrel/nightlies/"
                  ):

  ## info we need for our timestampfile
  paccmd = "pacman -allow trust-all-caches tar-overwrite -get "
  timenow = datetime.today().strftime("%Y%m%d") #("%Y%m%d.%H%M")
  print "timenow=", timenow
  version = os.environ["AtlasVersion"]
  print "version=", version
  area = os.environ["AtlasArea"].replace("/afs/cern.ch/atlas/software/builds/nightlies/","")
  print "area=", area
  areaArr = area.split("/")
  print "areaArr=", areaArr
  release = areaArr[0]
  print "release=", release
  project = areaArr[1]
  print "project=", project
  setupstr= "-tag=%s,%s,%s" % (release,project,version)
  print "setupstr=", setupstr 
  newcache= ""
  suffix  = ""
  if project=="AtlasTier0": suffix="-T0/"
  if project=="AtlasProduction": suffix="-Prod/"
  print "project=", project
  cmtconfig = os.environ["CMTCONFIG"].replace("-","_")
  print "cmtconfig", cmtconfig
  
  kitdir = kitbasedir + release + suffix + "i686-slc5-gcc43-opt/" + version + "/cache/"
  print "kitdir=", kitdir
  pacman = webbasedir + release + suffix + "i686-slc5-gcc43-opt/" + version + "/cache/"
  print "pacman=", pacman
  tardir = kitbasedir + release + suffix +"i686-slc5-gcc43-opt/" + version + "/kits/"
  print "tardir=", tardir

  #webdir = webbasedir + release + suffix + version + "/kits/"
  webdir = "http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0/rtt/OnlineRecoTests/"
  print "webdir=", webdir
  rttdir = "/afs/cern.ch/atlas/offline/external/FullChainTest/tier0/rtt/OnlineRecoTests/"
  print "rttdir=", rttdir

  tarfiles = []
  files=os.listdir(tardir)
  print "List of tardir =", files
  for tgzfile in files:
    if tgzfile.find(project)==0 and \
       tgzfile.endswith(".tar.gz"):
       shutil.copy(tardir+tgzfile,rttdir)
       tarfiles.append(webdir+tgzfile)
  print "tarfiles=", tarfiles
  
  files=os.listdir(kitdir)
  print "List of kitdir =", files  
  pacfileFound=False
  print "Is pacfileFound? ", pacfileFound
  print "Looking for %s*%s*pacman " % (project, cmtconfig)
  for pacfile in files:  
    print "pacfile=", pacfile
    if pacfile.find(project)==0 and \
       pacfile.endswith("_opt.pacman"):
       print "found pacfile = ", pacfile 
       #newcache = pacfile.replace(".pacman","")
       #print "newcache=", newcache
       pacfileTokens=pacfile.split("_")
       newcache=pacfileTokens[1]+"."+pacfileTokens[2]+"."+pacfileTokens[3]+"."+pacfileTokens[4]
       pacman+=pacfile
       print "pacman",pacman
       pacfileFound=True
       print "Is pacfileFound? ", pacfileFound
       break

  if not pacfileFound: 
    print "ERROR : Could not find pacman file for : %s %s %s." % (release,project,version)
    print "        Pacman basedir used : %s" % pacman
    print "        Timestamp file not updated. Exit."
    sys.exit(1)

  #newcache = newcache.replace("_"+cmtconfig,"")
  #print "newcache after replacing in cmtconfig:", newcache
  #newcache = newcache.replace(project+"_","")
  #print "newcache after replacing project:", newcache
  #newcache = newcache.replace("_",".")
  #print "final newcache:", newcache

  

  argdict = {}
  argdict['pacman'] = pacman 
  argdict['paccmd'] = paccmd+pacman
  argdict['setupstr'] = setupstr
  argdict['version'] = version
  argdict['area'] = area
  argdict['timenow'] = timenow
  argdict['project'] = project
  argdict['release'] = newcache
  argdict['projectrelease'] = project+"-"+release

  ## first line: timestamp
  ## following lines: files to copy over to p1
  file1 = open(timestampfile,'w')
  file1.write(timenow+"\n")
  for copyfile in tocpfile:
    file1.write(copyfile+"\n")
  for tgzfile in tarfiles:
    file1.write(tgzfile+"\n")
  file1.close()

  print "argdict: ", argdict 
  return argdict


def compileDict(argdict,inpickleArr,outpickle):

  for inpickle in inpickleArr:
    f = open(inpickle, 'r')
    map = pickle.load(f)
    f.close()
    keys = map.keys()
    for key in keys:
      argdict[key] = map[key]

  print "Saving pickled dictionary: %s" % outpickle
  makePickle(argdict,outpickle)

  return


## main
if __name__ == "__main__":

  usageline = "usage: " + sys.argv[0] + " <logfile1,logfile2,etc> <timestampfile> [<indic1.pickle,indic2.pickle,etc>] [<outdic.pickle>] [<tocpfile1,tocpfile2,etc>] [<forceAllOk>] [<kitbasedir>] [<webbasedir>]"
  if (len(sys.argv)<3):
    print usageline
    sys.exit(1)

  ## getopts
  logList = sys.argv[1]
  timestampfile = sys.argv[2] # 14.5.2.Y-VAL/AtlasProduction/latest_copied_release
  logArr = logList.split(",")
 
  inpickleArr = []
  if len(sys.argv)>3:
    inpickleStr = sys.argv[3]
    inpickleArr = inpickleStr.split(",")

  outpickle = "outdict.pickle"
  if len(sys.argv)>4:
    outpickle = sys.argv[4]

  tocpfile = []
  if len(sys.argv)>5:
    tocpfile = sys.argv[5].split(",")

  ## following dirs are preset
  outdir     = "/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/ort/"
  kitbasedir = "/afs/cern.ch/atlas/software/builds/kitrel/nightlies/" # /14.5.X.Y-T0/rel_3/cache/AtlasTier0_14_5_0_6_noarch.pacman
  webbasedir = "http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/kitrel/nightlies/"
  ## eg: http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/kitrel/nightlies/14.5.X.Y-T0/rel_0/cache:AtlasTier0_14_5_0_6_i686_slc4_gcc34_opt.pacman

  forceAllOk = False
  if len(sys.argv)>6:
    forceAllOk = ("True"==sys.argv[6])

  if len(sys.argv)>7:
    kitbasedir = sys.argv[7]
  if ( not kitbasedir.endswith("/") ): kitbasedir+="/"
  if len(sys.argv)>8:
    webbasedir = sys.argv[8]
  if ( not webbasedir.endswith("/") ): webbasedir+="/"

  ## process logfiles and write timestamp
  allOk = searchLogfiles(logArr) or forceAllOk
  if allOk:
    ## force okay?
    print "\nForce all okay ? %s" % forceAllOk
    print "\nAll logfile(s) okay.\nSaving updated timestamp file: %s" % timestampfile
    argdict = printTimestamp(timestampfile,outpickle,tocpfile,kitbasedir,webbasedir)
    compileDict(argdict,inpickleArr,outpickle)
  else:
    print "\nProblem found in logfile(s). Timestamp file not updated. Exit."

  print "\nErrorCode=0 (OK)"

