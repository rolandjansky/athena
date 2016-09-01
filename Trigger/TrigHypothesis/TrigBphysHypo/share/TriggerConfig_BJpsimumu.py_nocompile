include.block("TrigMuonHypo/TriggerConfig_BJpsimumu.py")

def TriggerConfig_BJpsimumu(TE,sliceID):

    if TriggerFlags.BphysicsSlice.doL2Muon() and TriggerFlags.BphysicsSlice.doL2ID():

        include ("TrigMuonHypo/TriggerConfig_TrigL2DimuonMassHypo.py")
        trigL2Jpsimumuhypo=TriggerConfig_TrigL2DimuonMassHypo("L2","Jpsi") 
        TE = sequenceFile.addSequence ("L2", TE+" "+TE, trigL2Jpsimumuhypo.classAndInstanceName(), "L2_"+sliceID)

    signatureFile.addSignature(signame=sliceID, prescale="1", forcedAccept="0",TElist=[TE])
