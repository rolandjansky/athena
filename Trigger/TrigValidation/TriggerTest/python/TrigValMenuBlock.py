# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValMenuBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, 'Default', ['Default','Physics','CSC-06'])

     self.name = "Menu"
     if(current  == "" ) : current = "Default"
     self.setCurrent(current)  # this calls initializeBlocks()


   def jobNameBlock(self) :
      if( self.current() == 'Default' ) :
          return ""
      else:
          return self.current()+"Menu"


   def  initializeBlocks(self) :

     ####################    All     ###############################
     if(    self.current() == 'Physics' ) :
         self.jobFlags     = """
triggerMenu.disableAll()

#### WARNING First attempt of Physics menu. The list below needs to be revised!!
triggerMenu.enableSignatures(["2e15i","e25i","e60"])
triggerMenu.enableSignatures(["2g20i","g60"])
triggerMenu.enableSignatures(["mu20"])
triggerMenu.enableSignatures(["tau25i"])
triggerMenu.enableSignatures(["jet160","2jet120","3jet65"])

# following menus are not available yet
#triggerMenu.enableSignatures(["4jet55","mu10"])

"""
     ####################    Calo     ###############################
     elif(  self.current() == 'CSC-06' ) :
         self.jobFlags     = """
triggerMenu.disableAll()

#### 
triggerMenu.enableSignatures(["e10","e10TRTxK", "2e15i","e25i","e60"])
triggerMenu.enableSignatures(["g10","2g20i","g60"])
triggerMenu.enableSignatures(["mu6l","mu6","mu20"])
triggerMenu.enableSignatures(["tau10","tau10i","tau15","tau15i","tau20i","tau25i","tau35i","tauNoCut"])
triggerMenu.enableSignatures(["jet20a","jet20b","jet20c","jet20d","jet20kt","jet160","2jet120","3jet65","4jet50])
triggerMenu.enableSignatures(["met10"])
triggerMenu.enableSignatures(["frjet10","fljet10", "b35"])
triggerMenu.enableSignatures(["L2_Zee"])
"""

