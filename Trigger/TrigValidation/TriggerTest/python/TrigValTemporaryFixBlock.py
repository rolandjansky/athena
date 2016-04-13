# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValTemporaryFixBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, 'Fix',['None','Fix'])

     self.name = "TemporaryFix"
     if(current  == "" ) : current = "Fix"
     self.setCurrent(current)  # this calls initializeBlocks()


   def jobNameBlock(self) :
      if( self.current() == 'None' ) :
          return "NoFix"
      else:
          return ""


   def  initializeBlocks(self) :

     ####################         ###############################
     if(    self.current() == 'Fix' ) :
         self.jobProperties     = ""
         self.jobFlags          = """
#------------ This is a temporary fix ---------------
TriggerFlags.abortOnConfigurationError=True
#-------------end of temporary fix-------------------
"""
         self.extraTopOptions   = """
# disable printing of histograms in finalize
if hasattr(svcMgr,'THistSvc'):
    svcMgr.THistSvc.OutputLevel = WARNING

# THistSvc = Service ( \"THistSvc\" )
# THistSvc.OutputLevel=WARNING
"""
