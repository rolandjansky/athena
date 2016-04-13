# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValDetectorBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, 'All', ['All','ID', 'Calo', 'Muon'])

     self.name = "Detector"
     if( current == "" ) : current = "All"
     self.setCurrent(current)  # this calls initializeBlocks()


   def jobNameBlock(self) :
      if( self.current() == 'All' ) :
          return ""
      else:
          return self.current()


   def  initializeBlocks(self) :

     ####################    ID     ###############################
     if(    self.current() == 'ID' ) :
         self.jobFlags     = """
# switch OFF other detectors
DetFlags.Muon_setOff()
TriggerFlags.doMuon=False

# has to allow Calo for now!
#DetFlags.Calo_setOff()
#TriggerFlags.doCalo=False

#### for now include only electron and tau slice
TriggerFlags.Slices_all_setOff()

TriggerFlags.EgammaSlice.setL2ID()
TriggerFlags.EgammaSlice.setEFID()
triggerMenu.disableAll()
triggerMenu.enableSignature("e10")
TriggerFlags.EgammaSlice.setFlag("doSiTrack",True)
TriggerFlags.EgammaSlice.setFlag("doTRTxK", False)

TriggerFlags.TauSlice.setL2ID()
#TriggerFlags.TauSlice.setEFID()
triggerMenu.enableSignature("tau10")

"""
     ####################    Calo     ###############################
     elif(  self.current() == 'Calo' ) :
         self.jobFlags     = """
# switch OFF other detectors
DetFlags.Muon_setOff()
DetFlags.ID_setOff()

TriggerFlags.doMuon=False
TriggerFlags.doID=False

TriggerFlags.TauSlice.unsetEF()
TriggerFlags.BphysicsSlice.unsetAll()
"""

     ####################    Muon     ###############################
     elif(  self.current() == 'Muon' ) :
         self.jobFlags     = """
# switch OFF other detectors
DetFlags.ID_setOff()
DetFlags.Calo_setOff()

TriggerFlags.doID=False
TriggerFlags.doCalo=False


TriggerFlags.BphysicsSlice.unsetAll()
TriggerFlags.MuonSlice.unsetEFID()

"""

