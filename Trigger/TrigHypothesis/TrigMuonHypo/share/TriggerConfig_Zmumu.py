include.block("TrigMuonHypo/TriggerConfig_Zmumu.py")

TE="MU06"

if TriggerFlags.MuonSlice.doL2Muon():
    # add an include for the algorithms used
    include ("TrigmuFast/TriggerConfig_TrigmuFast.py")

    # get an appropriate algorithm instance  
    t2mufast=TriggerConfig_TrigmuFast("L2","muon")

    # add algorithm instance to sequence starting from EMROI and producing TEout="T2CaloEgOut"
    TE = sequenceFile.addSequence("L2", TE, t2mufast.classAndInstanceName(), "T2muFastOut_Zmumu")

    include ("TrigMuonHypo/TriggerConfig_MufastHypo.py")
    t2mufastHypo20 = TriggerConfig_MufastHypo("L2", "muon", "20GeV")

    TE = sequenceFile.addSequence("L2", TE, t2mufastHypo6.classAndInstanceName(), "T2muFastMuon_Zmumu")

if TriggerFlags.MuonSlice.doL2ID():
    include ("TrigIDSCAN/TriggerConfig_TrigIDSCAN.py")

    # get appropriate algorithm instances
    idscanMuon=TriggerConfig_TrigIDSCAN("L2","muon")

    # add algorithm instance to sequence starting from TE and producing TE="L2electron"
    TE = sequenceFile.addSequence ("L2", TE, idscanMuon.classAndInstanceName(), "L2IDmuon_Zmumu")

if TriggerFlags.MuonSlice.doL2Muon() and TriggerFlags.MuonSlice.doL2ID():
    include ("TrigmuComb/TriggerConfig_TrigmuComb.py")

    # get appropriate algorithm instances
    trigmuComb=TriggerConfig_TrigmuComb("L2","muon")

    # add algorithm instance to sequence starting from TE and producing TE="L2electron"
    TE = sequenceFile.addSequence ("L2", TE, trigmuComb.classAndInstanceName(), "L2Combmuon_Zmumu")

    include ("TrigMuonHypo/TriggerConfig_TrigL2DimuonMassHypo.py")
    trigL2zmumuhypo=TriggerConfig_TrigL2DimuonMassHypo("L2","Z") 
    TE = sequenceFile.addSequence ("L2", TE+" "+TE, trigL2zmumuhypo.classAndInstanceName(), "L2zmumu")
   
    
# Add signatures here
signatureFile.addSignature(signame="Zmumu", prescale="1", forcedAccept="0",TElist=[TE])
