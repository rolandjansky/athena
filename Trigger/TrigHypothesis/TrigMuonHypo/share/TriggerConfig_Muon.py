include.block("TrigMuonHypo/TriggerConfig_Muon.py")
include("TrigMuonHypo/TriggerConfig_singleMuon.py")

TriggerFlags.MuonSlice.printFlags("Muon Slice Flags")

if TriggerFlags.doLVL1:
    include ("TrigMuonHypo/TriggerConfig_Muon_LVL1.py")

if triggerMenu.signatureIsEnabled("mu6l"):
    TriggerConfig_singleMuon("MU06" , "mu6l", "L1_MU06", "Muon" )

if triggerMenu.signatureIsEnabled("mu6"):
    TriggerConfig_singleMuon("MU06" , "mu6" , "L1_MU06", "Muon" )


if triggerMenu.signatureIsEnabled("mu20i"):
    TriggerConfig_singleMuon("MU20" , "mu20i" , "L1_MU20", "Muon" ) 
       
if triggerMenu.signatureIsEnabled("mu5"):
    TriggerConfig_singleMuon("MU05" , "mu5" , "L1_MU05", "900GeV" )

if triggerMenu.signatureIsEnabled("mu0"):
    TriggerConfig_singleMuon("MU00" , "mu0" , "L1_MU00", "900GeV" )


if triggerMenu.signatureIsEnabled("stau"):
    TriggerConfig_stauMuon("MU20" , "L1_MU20", "Muon" )


if triggerMenu.signatureIsEnabled("tileROD"):
#    TriggerConfig_tileROD("MU6" , "L1_MU6", "Muon" )
    TriggerConfig_tileROD("EM03" , "L1_EM5", "Muon" )
    

if triggerMenu.signatureIsEnabled("tileMU"):
#    TriggerConfig_tileMU("MU6" , "L1_MU6", "Muon" )
    TriggerConfig_tileMU("EM03" , "L1_EM5", "Muon" )


if triggerMenu.signatureIsEnabled("Zmumu"):
    include("TrigMuonHypo/TriggerConfig_Zmumu.py")


