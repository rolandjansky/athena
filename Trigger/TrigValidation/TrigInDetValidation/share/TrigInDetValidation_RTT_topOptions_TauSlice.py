###############################################################
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
###############################################################
#
# Set Flags for Running the Trigger in TransientBS Mode
#==============================================================

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if 'd0Max' in dir():
  from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
  InDetTrigSliceSettings[('d0SeedMax','tauCore')] = d0Max;
  InDetTrigSliceSettings[('d0SeedMax','tauIso')] = d0Max;
  InDetTrigSliceSettings[('d0SeedPPSMax','tauCore')] = d0Max;
  InDetTrigSliceSettings[('d0SeedPPSMax','tauIso')] = d0Max;

#if athenaCommonFlags.FilesInput()==[]:
#  athenaCommonFlags.FilesInput=[
#    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.RDO.e3601_s2757_r7245/RDO.06885566._001556.pool.root.1"    
#       ]

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

#from AthenaCommon.AppMgr import release_metadata
#d = release_metadata()
##TestMonTool.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']
#print d['nightly name']
#if d['nightly name']=='20.1.X.Y.Z-VAL-TrigMC' or d['nightly name']=='20.X.Y-VAL' or d['nightly name']=='21.X.Y' or d['nightly name']=='20.7.X-VAL' or '20.7.3.Y-VAL' in d['nightly name'] or '20.7.4.Y-VAL' in d['nightly name'] :
#  print '***JK This is a realease with FTK, will include chains '
#else:
#  print '***JK This release does not include FTK, will set doFTK=False'
#  doFTK=False

rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True
rFTK=False
if 'doFTK' in dir() and doFTK==True:
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  TriggerFlags.doFTK=True
  rFTK=True

(idtrigChainlist, tidaAnalysischains) = tauChains(rID,rFTK)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.TauSlice.setAll()
  #TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [ 'L1_TAU5', ]
  TriggerFlags.TauSlice.signatures = idtrigChainlist

PdgId=15



from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doHypo.set_Value_and_Lock(True)


include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

if 'mlExtensions' in dir() and mlExtensions==True:
  FTF = topSequence.TrigSteer_HLT.TrigFastTrackFinder_TauCore
  FTF.doSeedRedundancyCheck = True
  FTF.UseTrigSeedML  = 1 #can be 0, 1, 2, or 3, 0 means the ML-based seed filtering is off
  FTF.TrigSeedML_LUT = 'trigseed_ML_medium.lut' #can be _loose, _medium, or _strict
