
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

rMC = False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True

rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

rFTK=False
if 'doFTK' in dir() and doFTK==True:
  from TriggerJobOpts.TriggerFlags import TriggerFlags
  TriggerFlags.doFTK=True
  rFTK=True

(idtrigChainlist, tidaAnalysischains) = beamspotChains(rMC,rID,rFTK)

def resetSigs():
  TriggerFlags.doHypo=False
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.BeamspotSlice.setAll();
  TriggerFlags.BeamspotSlice.signatures = idtrigChainlist



include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")
topSequence.TrigSteer_HLT.terminateAlgo.Prescale=1.

if 'fastZFinder' in dir() and fastZFinder==True:
  FTF = topSequence.TrigSteer_HLT.TrigFastTrackFinder_BeamSpot_IDTrig

  # set fast ZFinder settings here
  # from AthenaCommon.ConfigurableDb import getConfigurable
  # zfinder = getConfigurable("TrigZFinder")
  zfinder = FTF.trigZFinder

  zfinder.NumberOfPeaks = 4
  zfinder.TripletMode = 1
  zfinder.TripletDZ = 1
  zfinder.PhiBinSize = 0.1
  zfinder.MaxLayer = 3
  zfinder.MinVtxSignificance = 10
  zfinder.Percentile = 0.95

  print zfinder


