#!/usr/bin env python
# arguments : list of run

import os, sys  
import string
import subprocess as sp

from ROOT import *

# Return the path of the output of tier0 monitoring
def returnEosHistPath(run,stream,amiTag,tag="data16_13TeV"):
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_','ZeroBias':'physics_','MinBias':'physics_'}
   path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
   P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   listOfFiles = p[0].split('\n')
   for iFile in listOfFiles:
      if ("HIST.%s"%(amiTag) in iFile):
         path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'+iFile
         P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
         p = P.communicate()
         path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'+iFile+'/'+p[0]
         return path

   return "FILE NOT FOUND"

# OBSOLETE - See below
# Return the path of the output of tier0 monitoring for the single LB (available only a couple of days after processing)
#def returnEosHistPathLB(run,lb,stream,amiTag,tag="data16_13TeV"):
#   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_','ZeroBias':'physics_'}
#   path = '/eos/atlas/atlastier0/tzero/prod/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
#   P = sp.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
#   p = P.communicate()
#   listOfFiles = p[0].split('\n')
#
#   for iFile in listOfFiles:
#      if ("recon.HIST.%s"%(amiTag) in iFile and "LOG" not in iFile):
#         path = '/eos/atlas/atlastier0/tzero/prod/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'+iFile
#         P = sp.Popen(['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
#         p = P.communicate()
#         listOfFiles2 = p[0].split('\n')
#         for iFile2 in listOfFiles2:            
#            print iFile2
#            ilb = int((iFile2.split("_lb")[1]).split("._SFO")[0])
#            if (lb == ilb):
#               path = '/eos/atlas/atlastier0/tzero/prod/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'+iFile+'/'+iFile2
#               return path
#
#   return "FILE NOT FOUND"

# Return the path of the output of tier0 monitoring for a range of single LB (available only a couple of days after processing)
def returnEosHistPathLB(run,lb0,lb1,stream,amiTag,tag="data16_13TeV"):
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_','ZeroBias':'physics_'}
   path = '/eos/atlas/atlastier0/tzero/prod/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
   P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   listOfFiles = p[0].split('\n')

   pathList = []
   for iFile in listOfFiles:
      if ("recon.HIST.%s"%(amiTag) in iFile and "LOG" not in iFile):
         path = '/eos/atlas/atlastier0/tzero/prod/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'+iFile
         P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
         p = P.communicate()
         listOfFiles2 = p[0].split('\n')
         for iFile2 in listOfFiles2:            
            if ("data" in iFile2):
               ilb = int((iFile2.split("_lb")[1]).split("._")[0])
#               print iFile2,ilb
               if (lb0<=ilb and ilb<=lb1):
                  path = '/eos/atlas/atlastier0/tzero/prod/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'+iFile+'/'+iFile2
                  pathList.append(path)
   
   if len(pathList)>0:
      return pathList
   else:
      return "FILE NOT FOUND"

# Return the list of TAGs files on EOS
def returnEosTagPath(run,stream,amiTag="f",tag ="data16_13TeV"):
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_','ZeroBias':'physics_'}
   found = False
   listOfFiles = []
   path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
   P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
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

   P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      files = p[0].split('\n')
      for iFile in files:
         if (len(iFile)>0):
            pathFile = path+iFile
            listOfFiles.append(pathFile)
   return listOfFiles

# Return the list of LArNoise ntuple files on EOS
def returnEosLArNoisePath(run,stream,amiTag="f",tag ="data16_13TeV"):
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_'}
   found = False
   listOfFiles = []
   path = '/eos/atlas/atlascerngroupdisk/det-larg/Tier0/perm/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
   P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      files = p[0]
      files = files.split('\n')
      for f in files:
         dotAmiTag=".%s"%(amiTag)
         if ('LARNOISE' in f and dotAmiTag in f): 
            path += f+'/' 
            found=True
            break
   if not found:
      print 'no LARNOISE directory found in %s'%(path)
      return 

   P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      files = p[0].split('\n')
      for iFile in files:
         if (len(iFile)>0):
            pathFile = path+iFile
            listOfFiles.append(pathFile)
   return listOfFiles

# Return the list of ESDs files on EOS
def returnEosEsdPath(run,stream,amiTag="f",tag ="data16_13TeV"):
   prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_'}
   found = False
   listOfFiles = []
   path = '/eos/atlas/atlastier0/rucio/'+tag+'/'+prefix[stream]+stream+'/00'+str(run)+'/'
   P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      files = p[0]
      files = files.split('\n')
      for f in files:
         dotAmiTag=".%s"%(amiTag)
         if ('.ESD' in f and dotAmiTag in f): 
            path += f+'/' 
            found=True
            break
   if not found:
      print 'no ESD directory found in %s'%(path)
      return 

   P = sp.Popen(['/usr/bin/eos','ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   if p[1]=='':
      files = p[0].split('\n')
      for iFile in files:
         if (len(iFile)>0):
            pathFile = path+iFile
            listOfFiles.append(pathFile)
   return listOfFiles

# Return the list of all files stored on a local user directory
def returnFilesPath(directory=".",filterName = ""):
   listOfFiles = []
   path = directory
   P = sp.Popen(['ls',path],stdout=sp.PIPE,stderr=sp.PIPE)
   p = P.communicate()
   found = false
   if p[1]=='':
      files = p[0]
      files = files.split('\n')
      for f in files:
         if filterName in f:
            pathFile = path  + f
            listOfFiles.append(pathFile)
            found = true

   if not found:
      print 'no file containing %f found in %s'%(filterName,path)
      return 

   return listOfFiles
