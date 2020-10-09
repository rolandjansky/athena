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
#second set of files can be activated by the if statement below 

#if athenaCommonFlags.FilesInput()==[]:
#  athenaCommonFlags.FilesInput=[
#    "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.RDO.e3601_s2665_s2183_r7191/RDO.06753544._000001.pool.root.1"
#    ]

include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

(idtrigChainlist, tidaAnalysischains) = electronChains(rID)


def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.EgammaSlice.setAll()
  TriggerFlags.EgammaSlice.signatures = idtrigChainlist


PdgId=11

include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")

