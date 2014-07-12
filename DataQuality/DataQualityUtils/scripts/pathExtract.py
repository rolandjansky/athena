#!/usr/bin env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# arguments : list of run

import os, sys  
import string
import subprocess as sp

from ROOT import *
from ROOT import gROOT, gDirectory, gPad


# Return the path of the output of tier0 monitoring
def returnTier0HistPath(run,stream,amiTag,tag="data12_8TeV"):
   os.putenv("STAGE_SVCCLASS","atlcal")
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_'}
   path="/castor/cern.ch/grid/atlas/tzero/prod1/perm/"+tag+"/"+prefix[stream]+stream+"/"
   P = sp.Popen(['nsls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      listOfRuns = p[0].split('\n')
      for iRun in listOfRuns:
         if (str(run) in iRun):
            path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/"+tag+"/"+prefix[stream]+stream+"/"+iRun
            P = sp.Popen(['nsls',path],stdout=sp.PIPE,stderr=sp.PIPE)
            p = P.communicate()
            listOfDirs = p[0].split('\n')
            for iDir in listOfDirs:
               dotAmiTag=".%s"%(amiTag)
               if ("HIST" in iDir and dotAmiTag in iDir):
                  path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/"+tag+"/"+prefix[stream]+stream+"/"+iRun+"/"+iDir
                  P = sp.Popen(['nsls',path],stdout=sp.PIPE,stderr=sp.PIPE)
                  p = P.communicate()
                  listOfFiles = p[0].split('\n')
                  path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/"+tag+"/"+prefix[stream]+stream+"/"+iRun+"/"+iDir+"/"+listOfFiles[0]
                  P = sp.Popen(['stager_get','-M',path],stdout=sp.PIPE,stderr=sp.PIPE)
                  p = P.communicate()
                  if p[1]=='':
                     if ("Insufficient" in p[0]):
                        print "File NOT STAGED : missing %s"%(path)
                        return "NOFILE"
                     else:
                        print "I found %s"%(path)
                        return path
   print "Failed -  Aborted at %s level"%(path)
   return "NOFILE"

# Return the list of TAGs files on castor
def returnTAGPath(run,stream,amiTag="f",tag ="data12_8TeV"):
   os.putenv("STAGE_SVCCLASS","atlcal")
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_'}
   found = False
   list = []
   path = '/castor/cern.ch/grid/atlas/tzero/prod1/perm/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
   P = sp.Popen(['nsls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      files = p[0]
      files = files.split('\n')
      for f in files:
         dotAmiTag=".%s"%(amiTag)
         if ('TAG' in f and dotAmiTag in f): 
            path += f+'/' 
            found=True
            break
   if not found:
      print 'no TAG directory found in %s'%(path)
      return 

   P = sp.Popen(['nsls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      files = p[0].split('\n')
      for iFile in files:
         if (len(iFile)>0):
            # Has found a file- Check that it is staged
            pathFile = path+iFile
            P = sp.Popen(['stager_get','-M',pathFile],stdout=sp.PIPE,stderr=sp.PIPE)
            p = P.communicate()
            if p[1]=='':
               if ("Insufficient" in p[0]):
                  print "File NOT STAGED=== Skipping file %s"%(pathFile)
               else:
                  print "I found %s"%(pathFile)
                  list.append(pathFile)
            else:
               return 'no TAG file'
   return list

# Add new run to noise bursts tchain - 2011 data stored on castor
def addRun(newRun,chain):
   path='/castor/cern.ch/grid/atlas/atlasgroupdisk/det-larg/data11_7TeV/NTUP_LARNOISE/'
   P = sp.Popen(['nsls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   print p
   if p[1]=='':
      tags = p[0]
      tags = tags.split('\n')
      print tags
      for t in tags:
         path='/castor/cern.ch/grid/atlas/atlasgroupdisk/det-larg/data11_7TeV/NTUP_LARNOISE/'+t
         P = sp.Popen(['nsls',path],stdout=sp.PIPE,stderr=sp.PIPE)
         p = P.communicate()
         if p[1]=='':
            runs = p[0]
            runs = runs.split('\n')
            for iRun in runs:
               if (str(newRun) in iRun) and ('CosmicCalo' in iRun):
                  path='/castor/cern.ch/grid/atlas/atlasgroupdisk/det-larg/data11_7TeV/NTUP_LARNOISE/'+t+'/'+iRun
                  P = sp.Popen(['nsls',path],stdout=sp.PIPE,stderr=sp.PIPE)
                  p = P.communicate()
                  print path
                  if p[1]=='':
                     files = p[0]
                     files = files.split('\n')
                     for iFile in files:
                        if 'root' in iFile:
                           path='rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/det-larg/data11_7TeV/NTUP_LARNOISE/'+t+'/'+iRun+'/'+iFile
                           print "Adding :",path
                           chain.Add(path)

# Add new run to noise bursts TChain - 2012 data stored on EOS
def eosChainInclude(newRun,stream,chain):
   path="/eos/atlas/atlasgroupdisk/det-larg/data12_8TeV/NTUP_LARNOISE/"
   P = sp.Popen(["/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select","ls",path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      tags = p[0]
      tags = tags.split('\n')
      for t in tags:
         path='/eos/atlas/atlasgroupdisk/det-larg/data12_8TeV/NTUP_LARNOISE/'+t
         P = sp.Popen(['/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
         p = P.communicate()
         if p[1]=='':
            runs = p[0]
            runs = runs.split('\n')
            for iRun in runs:
               if (str(newRun) in iRun) and (stream in iRun):
                  path='/eos/atlas/atlasgroupdisk/det-larg/data12_8TeV/NTUP_LARNOISE/'+t+'/'+iRun
                  P = sp.Popen(['/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
                  p = P.communicate()
                  if p[1]=='':
                     files = p[0]
                     files = files.split('\n')
                     for iFile in files:
                        if 'root' in iFile:
                           path='root://eosatlas//eos/atlas/atlasgroupdisk/det-larg/data12_8TeV/NTUP_LARNOISE/'+t+'/'+iRun+'/'+iFile
                           print "Adding %s"%(path)
                           chain.Add(path)
