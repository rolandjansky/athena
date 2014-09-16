# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValValidationBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, 'None', ['None','NODEBUG'])

     self.name = "Validation"
     if(current  == "" ) : self.setCurrent('None')  # this calls initializeBlocks()


   def jobNameBlock(self) :
      return ""

   def  initializeBlocks(self) :

     ####################         ###############################
     if(    self.current() == 'NODEBUG' ) :
         self.jobFlags  = """
TriggerFlags.enableMonitoring = [ 'Validation', 'Time' ]
"""
     else :        
         self.jobFlags  = """
TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]
"""

         self.extraTopOptions  = """
"""

