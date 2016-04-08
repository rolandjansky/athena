#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# module: PostProcessRecoRTTTests.py

from Logger import Logger

class PostProcessRecoRTTTests:
   def __init__(self,argDict={}):
      
      self.logger    = Logger()
      descriptor = argDict.get('JobDescriptor', None)
      
      self.RttName = descriptor.name
      self.DisName = descriptor.jobDisplayName
      
      self.NightlyType  = descriptor.paths.branch     # ==> e.g. dev, devval, 15.4.X.Y, etc.
      self.ProjectName  = descriptor.paths.topProject # ==> AtlasProduction, AtlasTier0, etc.
      # self.cmtConfigStr = descriptor.paths.cmtConfig  # ==> e.g. i686-slc4-gcc34-opt
      self.PackageTag   = descriptor.paths.packageTag # ==> e.g. PackageTag-00-01-20
      self.resPath      = descriptor.resPath
      self.logFile      = descriptor.log

   def run(self):
      
      from subprocess import Popen, PIPE
      import os
      
      releaseName = os.environ['AtlasVersion']
      atlasArea   = os.environ['AtlasArea']
      cmtConfig   = os.environ['CMTCONFIG']
      
      # post processing the logfiles
      self.logger.info("PostProcessRecoRTTTests run()")
      
      dp=os.environ['DATAPATH']
      self.logger.info(" echo DATAPATH : %s"%dp)
      self.logger.info(" get_files slurpRecoRTTTests.pl")
      try:
         carg=[ "get_files", "slurpRecoRTTTests.pl" ]

         p = Popen(carg, stdout=PIPE)
         out=p.communicate()[0]
         self.logger.info(out)
         
      except IOError:
         self.logger.info( 'not good. something is wrong with get_files slurpRecoRTTTests.pl' )
         return -1
      
      self.logger.info(" proccessing logfile %s"%self.logFile)
      try:
         carg=[ "./slurpRecoRTTTests.pl", self.logFile ]

         p = Popen(carg, stdout=PIPE)
         out=p.communicate()[0]
         self.logger.info(out)
         
      except IOError:
         self.logger.info( 'not good. something is wrong with slurpRecoRTTTests.pl' )
         return -1
      
      return 0
