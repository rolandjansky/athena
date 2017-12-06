#####################################################################################################
#
# top level jobOptions to run Electron chains in the RTT or standalone
# sets some global variables that adjust the execution of TrigInDetValidation_RTT_Common.py
#
# Jiri.Masik@manchester.ac.uk
#
#####################################################################################################

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#set athenaCommonFlags.FilesInput to be able to use this job options standalone without RTT

#if athenaCommonFlags.FilesInput()==[]:
#  athenaCommonFlags.FilesInput=[
#    "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7193/RDO.06752771._000001.pool.root.1",
#  ]


rMC = False
rID = False
rFTK=False
rBperf=False
if 'runMergedChain' in dir() and runMergedChain==True:
  rMC = True
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True
if 'doBperf' in dir() and doBperf==True:
  rBperf = True

tidaAnalysischains = []

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

(electronChainlist, electronAnalysischains) = electronChains(rID)
tidaAnalysischains += electronAnalysischains

(muonChainlist, muonAnalysischains) = muonChains(rMC, rID, rFTK)
tidaAnalysischains += muonAnalysischains

(tauChainlist, tauAnalysischains) = tauChains(rMC, rID, rFTK)
tidaAnalysischains += tauAnalysischains

(bjetChainlist, bjetAnalysischains) = bjetChains(rMC, rID, rFTK, rBperf)
tidaAnalysischains += bjetAnalysischains

(minBiasChainlist, minBiasAnalysischains) = minBiasChains(rMC, rID)
tidaAnalysischains += minBiasAnalysischains

def resetSigs():
  TriggerFlags.Slices_all_setOff()

  TriggerFlags.EgammaSlice.setAll()
  TriggerFlags.EgammaSlice.signatures = electronChainlist

  TriggerFlags.MuonSlice.setAll();
  TriggerFlags.MuonSlice.signatures = muonChainlist

  TriggerFlags.TauSlice.setAll()
  TriggerFlags.TauSlice.signatures = tauChainlist

  TriggerFlags.BjetSlice.setAll();
  TriggerFlags.BjetSlice.signatures = bjetChainlist

#  TriggerFlags.MinBiasSlice.setAll();
#  TriggerFlags.MinBiasSlice.signatures = minBiasChainlist

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")
