
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


(idtrigChainlist, tidaAnalysischains) = fullScanChains(rID)

def resetSigs():
  TriggerFlags.doHypo=False
  TriggerFlags.Slices_all_setOff()
  TriggerFlags.BeamspotSlice.setAll();
  TriggerFlags.METSlice.setAll();
  TriggerFlags.JetSlice.setAll();
  TriggerFlags.BeamspotSlice.signatures = idtrigChainlist



include("TrigInDetValidation/TrigInDetValidation_RTT_Common.py")


