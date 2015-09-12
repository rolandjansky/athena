#!/usr/bin env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# arguments : list of run

import os, sys  
import string
import subprocess as sp

from ROOT import *
from ROOT import gROOT, gDirectory, gPad

# Return the path of the output of tier0 monitoring
def returnEosHistPath(run,stream,amiTag,tag="data15_cos"):
   os.putenv("STAGE_SVCCLASS","atlcal")
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_','ZeroBias':'physics_'}
   path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
   P = sp.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   listOfFiles = p[0].split('\n')
   for iFile in listOfFiles:
      if ("HIST.%s"%(amiTag) in iFile):
         path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'+iFile
         P = sp.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
         p = P.communicate()
         path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'+iFile+'/'+p[0]
         return path

   return "FILE NOT FOUND"

# Return the list of TAGs files on EOS
def returnEosTagPath(run,stream,amiTag="f",tag ="data15_13TeV"):
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_'}
   found = False
   list = []
   path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
   P = sp.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
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

   P = sp.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      files = p[0].split('\n')
      for iFile in files:
         if (len(iFile)>0):
            pathFile = path+iFile
            list.append(pathFile)
   return list
