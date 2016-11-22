include.block("TrigJetHypo/TriggerConfig_Jet.py")


TriggerFlags.JetSlice.printFlags("Jet Slice Flags")
print " ###############################################################"
print " ##                           JetSliceFlags                   ##"
print " ##               L2Muon  L2Calo   L2ID    EFMuon  EFCalo  EFID   ##"
print " ## Jet Slice   : ", TriggerFlags.JetSlice.printL2Muon()," ", TriggerFlags.JetSlice.printL2Calo(), " ", TriggerFlags.JetSlice.printL2ID(), "\ ", TriggerFlags.JetSlice.printEFMuon(), " ", TriggerFlags.JetSlice.printEFCalo(), " ", TriggerFlags.JetSlice.printEFMuon(), " ##"




include("TrigJetHypo/TriggerConfig_Jet_Level1.py")

from TrigJetHypo.TriggerConfig_JetHypoMaker import configChain


if triggerMenu.signatureIsEnabled("J5"):
     configChain(TriggerFlags, triggerPythonConfig,'J5')

if triggerMenu.signatureIsEnabled("J10"):
     configChain(TriggerFlags, triggerPythonConfig,'J10')

#if triggerMenu.signatureIsEnabled("jet10fr"):
#     configChain(TriggerFlags, triggerPythonConfig,'jet10fr')

#if triggerMenu.signatureIsEnabled("jet10fl"):
#     configChain(TriggerFlags, triggerPythonConfig,'jet10fl')
