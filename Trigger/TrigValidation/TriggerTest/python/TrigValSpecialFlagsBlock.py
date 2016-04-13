# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValSpecialFlagsBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, 'None',['None','doTruthFalse','memTrack', 'rerunHYPO','hackRPC'])

     self.name = "SpecialFlags"
     if(current  == "" ) : current = "None"
     self.setCurrent(current)  # this calls initializeBlocks()


   def jobNameBlock(self) :
      if( self.current() == 'None' ) :
          return ""
      else:
          return "_"+self.current()


   def  initializeBlocks(self) :

     ####################     doTruthFalse    ###############################
     if(    self.current() == 'doTruthFalse' ) :
        self.jobProperties     = """
rec.doTruth=False
"""
     else :
        self.jobProperties     = """
rec.doTruth=True
"""

     ####################     check mem leaks    ###############################
     if(    self.current() == 'memTrack' ) :
        self.jobProperties     += """
# Run the memory leak checker
# This is equivalent to athena --leak-check-execute
import Hephaestus.MemoryTracker as memtrack
memtrack.trace("Algorithm::sysExecute")
memtrack.ignore("StoreGateSvc::setupProxy")
memtrack.configure(memtrack.LEAK_CHECK | memtrack.PROFILE)
"""



     ####################     rerunHYPO    ###############################
     if(    self.current() == 'rerunHYPO' ) :
        self.jobProperties     += """
"""
        self.jobFlags          += """
#jobOptions to re-run HYPO only        
TriggerFlags.doHypo=True
TriggerFlags.doFEX=False
"""
        self.extraTopOptions   += ""



     ####################     RPC hack for L=10^31    ###############################
     if(    self.current() == 'hackRPC' ) :
        self.jobProperties     += """
"""
        self.jobFlags          += """
if hasattr(svcMgr,'THistSvc'):
    svcMgr.THistSvc.OutputLevel = WARNING

# hack for L=10^31 (repeated in special flags)
if hasattr(ServiceMgr,'RPCcblingSvc')
    ServiceMgr.RPCcablingSvc.HackFor1031 = True
"""
        self.extraTopOptions   += ""
