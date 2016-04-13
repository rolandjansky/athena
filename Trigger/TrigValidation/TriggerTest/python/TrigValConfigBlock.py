# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

#
# Block added temporarily to test new vs. old Python configuration 
#
class TrigValConfigBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
      TrigValJobOptionBlock.__init__(self, "Modern",['Modern','OldConfig']) 
      
      self.name = "Modern"
      if(current  == "" ) : current = "Modern"
      self.setCurrent(current)  # this calls initializeBlocks()


   def jobNameBlock(self) :
      return self.current()
   
   def  initializeBlocks(self) :

      if( self.current() == 'Modern' ) :
         self.jobProperties     += """

TriggerModernConfig=True

"""
      else :
         self.jobProperties     += """

TriggerModernConfig=False

"""


