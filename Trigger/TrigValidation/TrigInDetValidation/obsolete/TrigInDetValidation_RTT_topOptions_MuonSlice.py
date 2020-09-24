#####################################################################################################
#
# top level jobOptions to run Muon chains in the RTT or standalone
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
#      "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.RDO.e3601_s2576_s2132_r7143/RDO.06718162._000013.pool.root.1"
#    ]

###XMLDataSet='TrigInDetValidation_mu_single_mu_100' # <-- RTT jobID


include("TrigInDetValidation/TrigInDetValidation_RTT_Chains.py")

rID=False
if 'doIDNewTracking' in dir() and doIDNewTracking==True:
  rID = True

(idtrigChainlist, tidaAnalysischains) = muonChains(rID)

def resetSigs():
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.MuonSlice.setAll();
  TriggerFlags.MuonSlice.signatures = idtrigChainlist


PdgId=13


include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")



