include.block("TrigBphysHypo/TriggerConfig_Bphysics_flags.py")

lowLumiMenu=(HLTconfigVersion.find("900GeV")!=-1)
    
triggerMenu.addSignatures(["BJpsiMU6mu"]) 
triggerMenu.enableSignatures(["BJpsiMU6mu"])

triggerMenu.addSignatures(["Bmu6mu6"])
triggerMenu.enableSignatures(["Bmu6mu6"])

triggerMenu.addSignatures(["Bmu6mu6X"])
triggerMenu.addSignatures(["Bmu0mu0X"])
triggerMenu.disableSignatures(["Bmu6mu6X", "Bmu0mu0X"])

triggerMenu.addSignatures(["BJpsiMU0mu"]) 
triggerMenu.addSignatures(["Bmu0mu0"])

triggerMenu.addSignatures(["BUpsiMU6mu"]) 
triggerMenu.enableSignatures(["BUpsiMU6mu"])

if not lowLumiMenu:
    triggerMenu.disableSignatures(["BJpsiMU0mu", "Bmu0mu0",  "Bmu0mu0X"])
    
#using Jet RoI
triggerMenu.addSignatures(["BsDsPhiPi"])
triggerMenu.enableSignatures(["BsDsPhiPi"])

#using EM RoI
triggerMenu.addSignatures(["BJpsiee"])
triggerMenu.disableSignatures(["BJpsiee"])

triggerMenu.addSignatures(["BGammaX"])
triggerMenu.disableSignatures(["BGammaX"])

#FullScan
triggerMenu.addSignatures(["BsDsPhiPiFullScan"])

if not lowLumiMenu:
    triggerMenu.disableSignatures(["BsDsPhiPiFullScan"])
                          
TriggerFlags.BphysicsSlice.addFlags(["doSiTrack"], False)
TriggerFlags.BphysicsSlice.addFlags(["doIDSCAN"], True)
