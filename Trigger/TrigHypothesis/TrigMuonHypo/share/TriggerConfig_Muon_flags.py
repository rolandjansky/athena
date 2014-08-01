include.block("TrigEgammaHypo/TriggerConfig_Muon_flags.py")


triggerMenu.addSignatures(["mu6l", "mu6", "mu20i", "mu5", "mu0", "Zmumu", "stau","tileROD","tileMU"])

triggerMenu.disableSignature("Zmumu")
triggerMenu.disableSignature("stau")
triggerMenu.disableSignature("tileROD")
triggerMenu.disableSignature("tileMU")

if HLTconfigVersion=="CSC-02-900GeV":
    triggerMenu.disableSignature("mu6l")

else:
    triggerMenu.disableSignature("mu5")
    triggerMenu.disableSignature("mu0")

TriggerFlags.MuonSlice.addFlags(["doSITRACK"],True)
