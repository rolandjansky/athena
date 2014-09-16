# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValSliceBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, 'All', ['All','Electron', 'Photon', 'Muon','MuonOldConfig', 'Tau',
                                                  'Jet', 'MET', 'Bphysics','BphysicsOldConfig','Bjet','Cosmics'])

     self.name = "Slice"
     if(current  == "" ) : current = "All"
     self.setCurrent(current)  # this calls initializeBlocks()



   def jobNameBlock(self) :
      # all exclude Cosmics
      if( self.current() == 'All' ) :
          return ""
      else:
          # temporary fix for Bphys and Muon: should
          # be removed when OldConfig disappears-RG23/8
          if ( self.current() == "BphysicsOldConfig" ) :
             return "BphysicsSlice"
          elif ( self.current() == "MuonOldConfig" ) :
             return "MuonSlice"          
          else :
             return self.current()+"Slice"


   def  initializeBlocks(self) :
     if(  self.current() != 'All' ) :
        self.jobFlags     = """
####  First switch all slices OFF
TriggerFlags.Slices_all_setOff()
"""

     ####################   Electron      ###############################
     if(    self.current() == 'Electron' ) :
         self.jobFlags     += """
###### This is a temporary fix ################
DetFlags.simulateLVL1.Calo_setOn()

# Enable electron slice
TriggerFlags.EgammaSlice.setAll()
triggerMenu.disableAll()
triggerMenu.enableSignatures(["e10", "e25i", "e60", "2e15i", "e10TRTxK", "Zee", "Jpsiee"])
"""

     ####################   Photon      ###############################
     if(    self.current() == 'Photon' ) :
         self.jobFlags     += """
###### This is a temporary fix ################
DetFlags.simulateLVL1.Calo_setOn()

# Enable Photon slice
TriggerFlags.EgammaSlice.unsetAll()
triggerMenu.disableAll()
triggerMenu.enableSignature("g10")
triggerMenu.enableSignature("g60")
triggerMenu.enableSignature("2g20i")

#enable only calorimeter parts for photon standalone
TriggerFlags.EgammaSlice.setL2Calo()
TriggerFlags.EgammaSlice.setEFCalo()
#TriggerFlags.EgammaSlice.setL2ID()
#TriggerFlags.EgammaSlice.setEFID()
"""

     ####################   Muon  #########################################
     if(    self.current() == 'Muon' ) :
         self.jobFlags     += """
# Enable Muon slice
DetFlags.digitize.Muon_setOff()

TriggerFlags.MuonSlice.doL2Muon()
TriggerFlags.MuonSlice.doL2ID()
#TriggerFlags.MuonSlice.setL2Calo()

# Muon EF part
TriggerFlags.MuonSlice.doEFMuon()
TriggerFlags.MuonSlice.doEFID()

"""

     if(    self.current() == 'MuonOldConfig' ) :
         self.jobFlags     += """         
if readMuonHits: 
    DetFlags.digitize.Muon_setOn()
    DetFlags.readRIOBS.Muon_setOff()
    DetFlags.readRIOPool.LVL1_setOff()
    TriggerFlags.doLVL1=True
#    ###### This is a temporary fix ################
    DetFlags.simulateLVL1.Muon_setOn()
else:
    DetFlags.digitize.Muon_setOff()

TriggerFlags.MuonSlice.setL2Muon()
TriggerFlags.MuonSlice.setL2ID()

# Muon EF part
TriggerFlags.MuonSlice.setEFMuon()
TriggerFlags.MuonSlice.setEFID()

"""

         self.extraTopOptions   += """
muFast = TrigSteer_L2.allConfigurables.get('muFast_Muon')

muFast.OutputLevel = DEBUG

muFast.MUlvl1INFO  = True
muFast.MUtrackINFO = True
muFast.MUroadsINFO = True
muFast.MUdecoINFO  = True
muFast.MUcontINFO  = True
muFast.MUfitINFO   = True
muFast.MUsagINFO   = True
muFast.MUptINFO    = True

muFast.TestString = "muFast_Muon_L2 REGTEST "

#TrigSteer_L2.OutputLevel=DEBUG
#TrigSteer_L2.allConfigurables.get('TrigIDSCAN_Muon').OutputLevel=DEBUG
#TrigSteer_L2.allConfigurables.get("muComb_Muon").OutputLevel=DEBUG
#TrigSteer_L2.allConfigurables.get("MucombHypo_Muon_6GeV").OutputLevel=DEBUG
TrigSteer_EF.allConfigurables.get("MooHLTAlgo_CB_Muon").IDFromTrigger = True
"""


     ####################   Tau      ###############################
     if(    self.current() == 'Tau' ) :
         self.jobFlags     += """
###### This is a temporary fix ################
DetFlags.simulateLVL1.Calo_setOn()

# Enable Tau slice
TriggerFlags.TauSlice.setAll()
#triggerMenu.disableAll()
#triggerMenu.enableSignatures(["tau10","tau10i","tau15","tau15i","tau20i","tau25i","tau35i","tauNoCut"])
"""

     ####################   Jet      ###############################
     if(    self.current() == 'Jet' ) :
         self.jobFlags     += """
###### This is a temporary fix ################
DetFlags.simulateLVL1.Calo_setOn()

# Enable Jet slice
TriggerFlags.JetSlice.setAll()
#triggerMenu.disableAll()
#triggerMenu.enableSignatures(["jet160"])
"""

     ####################   MET      ###############################
     if(    self.current() == 'MET' ) :
         self.jobFlags     += """

# add muons for MET ... !
if readMuonHits:
    DetFlags.digitize.Muon_setOn()
    DetFlags.readRIOBS.Muon_setOff()
    DetFlags.readRIOPool.LVL1_setOff()
    TriggerFlags.doLVL1=True
    ###### This is a temporary fix ################
    DetFlags.simulateLVL1.Muon_setOn()
else:
    DetFlags.digitize.Muon_setOff()

###### This is a temporary fix ################
DetFlags.simulateLVL1.Calo_setOn()

# Enable MET slice
#TriggerFlags.METSlice.unsetAll()
TriggerFlags.METSlice.setL2Calo()
TriggerFlags.METSlice.setEFCalo()

"""

     ####################   Bjet      ###############################
     if(    self.current() == 'Bjet' ) :
         self.jobFlags     += """

###### This is a temporary fix ################
DetFlags.simulateLVL1.Muon_setOn()

# Enable Bjet slice
TriggerFlags.BjetSlice.setAll()
triggerMenu.disableAll()
triggerMenu.enableSignature("b35")
"""

     ####################   Bphysics      ###############################
     if(    self.current() == 'Bphysics' ) :
         self.jobFlags     += """
###### This is a temporary fix ################
DetFlags.simulateLVL1.Muon_setOn()

# Enable Bphysics slice: needs muon slice
TriggerFlags.MuonSlice.setAll()
TriggerFlags.BphysicsSlice.setAll()

# hack for L=10^31 (repeated in special flags)
if hasattr(ServiceMgr,'RPCcblingSvc')
    ServiceMgr.RPCcablingSvc.HackFor1031 = True
"""

     if(    self.current() == 'BphysicsOldConfig' ) :
         self.jobFlags     += """
###### This is a temporary fix ################
DetFlags.simulateLVL1.Muon_setOn()

# Enable Bphysics slice: needs muon slice
TriggerFlags.BphysicsSlice.unsetAll()
TriggerFlags.BphysicsSlice.setL2Muon()
TriggerFlags.BphysicsSlice.setL2ID()
TriggerFlags.BphysicsSlice.setEFID()

# Enable muon slice
TriggerFlags.MuonSlice.setL2Muon()
TriggerFlags.MuonSlice.setL2ID()
TriggerFlags.MuonSlice.unsetEF()

triggerMenu.disableAll()
triggerMenu.enableSignatures(["BJpsiMU6mu", "Bmu6mu6" ,"BsDsPhiPi","BsDsPhiPiFullScan"])
triggerMenu.enableSignatures(["BJpsiee"])
triggerMenu.enableSignatures(["mu6", "mu6l"])

"""

     ####################   Cosmics      ###############################
     if(    self.current() == 'Cosmics' ) :
        self.jobProperties     = """
if not 'FakeLVL1' in dir():
    FakeLVL1 = True

if not 'doSiTrack' in dir():
    doSiTrack = False
        
if not 'PoolRDOInput' in dir():
    acf.PoolRDOInput = ['/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/cosmic.10evts.rdo.root']
    DetDescrVersion='ATLAS-CSC-00-00-00'

# make sure Truth is OFF, even if it was set to ON before
rec.doTruth=False

"""
        self.jobFlags          = """
TriggerFlags.doEF=False
#TriggerFlags.doID=True
#TriggerFlags.doMuon=False

TriggerFlags.CosmicSlice.setL2()

triggerMenu.disableAll()

if doSiTrack:
    triggerMenu.enableSignature("cosmic_SiTrack")
else:
    triggerMenu.enableSignature("cosmic_IDSCAN")
    
theApp.Dlls   += [ "TrigmuFast" ] # needed for MUON::ROBmapper::ROBmapper(std::basic_string<char, std::char_traits<char>, std::allocator<char> >)
#theApp.Dlls += ['TrigL2CosmicMuonHypo']
theApp.Dlls += [ "TrigOnlineSpacePointTool","TrigOfflineSpacePointTool" ]

theApp.Dlls += [ "TrkExTools" ]


    

"""
        
        self.extraTopOptions   = """
if FakeLVL1:

    from TrigFake.TrigFakeConf import FakeLvl1RoIatFixedEtaPhi
#    from TrigFake.TrigFakeConf import FakeLvl1MultipleRoIsatFixedEtaPhi

#    fake = FakeLvl1MultipleRoIsatFixedEtaPhi()
#    fake.FakeMuonRoiPhi=[1.5708]#,-1.5708]
#    fake.FakeMuonRoiEta=[0.1]#,0.2]
#    fake.FakeMuonRoiLabel=["MU06"]#,"MU06"]
    fake = FakeLvl1RoIatFixedEtaPhi()
    fake.FakeMuonRoiPhi=3.1
    fake.FakeMuonRoiEta=0.1
    fake.FakeMuonRoiLabel="MU06"

    # fake.OutputLevel=VERBOSE

    job.TrigSteer_L2.LvlConverterTool =  fake

"""

