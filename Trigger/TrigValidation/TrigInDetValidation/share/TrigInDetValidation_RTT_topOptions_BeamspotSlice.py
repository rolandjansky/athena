###############################################################
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
###############################################################

#####################################################################################################
#
# top level jobOptions to run b-jet chains in the RTT or standalone
# sets some global variables that adjust the execution of TrigInDetValidation_RTT_Common.py
#
# Jiri.Masik@manchester.ac.uk
#
#####################################################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#set athenaCommonFlags.FilesInput to be able to use this job options standalone without RTT
#secondSet of files can be activated by the if statement below 

#if athenaCommonFlags.FilesInput()==[]:
#  athenaCommonFlags.FilesInput=[
#    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7193/RDO.06752771._000001.pool.root.1",
#    ]

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

rFTK=False
if 'doFTK' in dir() and doFTK==True:
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  TriggerFlags.doFTK=True
  rFTK=True

(idtrigChainlist, tidaAnalysischains) = beamspotChains(rID,rFTK)

def resetSigs():
  TriggerFlags.doHypo=False
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.BeamspotSlice.setAll();
  TriggerFlags.BeamspotSlice.signatures = idtrigChainlist



include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")
topSequence.TrigSteer_HLT.terminateAlgo.Prescale=1.

FTF = topSequence.TrigSteer_HLT.TrigFastTrackFinder_BeamSpot_IDTrig
if 'mlExtensions' in dir() and mlExtensions==True:
  FTF.doSeedRedundancyCheck = True
  FTF.UseTrigSeedML  = 1 #can be 0, 1, 2, or 3, 0 means the ML-based seed filtering is off
  FTF.TrigSeedML_LUT = 'trigseed_ML_medium.lut' #can be _loose, _medium, or _strict


if 'fastZFinder' in dir() and fastZFinder==True:
  from AthenaCommon.AppMgr import ToolSvc
  zfinder = ToolSvc.TrigZFinder

  zfinder.NumberOfPeaks = 4
  zfinder.TripletMode = 1
  zfinder.TripletDZ = 1
  zfinder.PhiBinSize = 0.1
  zfinder.MaxLayer = 3
  zfinder.MinVtxSignificance = 10
  zfinder.Percentile = 0.95

  print 'zfinder settings modified by TrigInDetValidation_RTT_topOptions_BeamspotSlice.py'
  print zfinder


else:
  FTF.doZFinder = False

  
# good options for any slice - leave here commented while for use in 
# tests in the near future  
#  FTF.UseTrigSeedML = 3 #all pixel clusters in PPP seeds will passed through the filter
#  FTF.UseSCT_MiddleSP = False #effectively switch to PPP mode as PPS are switched off by default
#  FTF.maxEC_Pixel_cluster_length = 0.65 #will be replaced by a trained endcap LUT when it's available
  
