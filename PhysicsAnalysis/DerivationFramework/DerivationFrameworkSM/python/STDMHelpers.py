# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

def STDMRecalcJets(sequence, derivationName, isMC):
    STDMJetCollections = {} 
    STDMJetCollections["STDM2"] = ["AntiKt4TruthJets", "AntiKt4TruthWZJets"]
    STDMJetCollections["STDM3"] = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets"]
    STDMJetCollections["STDM4"] = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets"]
    STDMJetCollections["STDM5"] = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets"]
    STDMJetCollections["STDM6"] = []
    STDMJetCollections["STDM9"] = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets", "AntiKt4TruthJets", "AntiKt4TruthWZJets"]

    myOutputGroup = derivationName + "Jets"

    # dict for output jet stream
    jetsToWrite = []

    # find settings here:
    # https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Jet/JetRec/trunk/share/JetRec_jobOptions.py#L44

    # build AntiKt2PV0TrackJets
    if "AntiKt2PV0TrackJets" in STDMJetCollections[derivationName]:
        addStandardJets("AntiKt", 0.2, "PV0Track", 2000, mods="track_ungroomed", algseq=sequence, outputGroup=myOutputGroup)
        jetsToWrite.append("AntiKt2PV0TrackJets")

    # build AntiKt4PV0TrackJets
    if "AntiKt4PV0TrackJets" in STDMJetCollections[derivationName]:
        addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=sequence, outputGroup=myOutputGroup)
        jetsToWrite.append("AntiKt4PV0TrackJets")

    # build AntiKt10LCTopoJets
    if "AntiKt10LCTopoJets" in STDMJetCollections[derivationName]:
        addStandardJets("AntiKt", 1.0, "LCTopo", mods="lctopo_ungroomed", ptmin=40000, ptminFilter=50000, calibOpt="none", algseq=sequence, outputGroup=myOutputGroup)
        jetsToWrite.append("AntiKt10LCTopoJets")

    # truth jets
    if isMC:
        # build AntiKt4TruthJets
        if "AntiKt4TruthJets" in STDMJetCollections[derivationName]:
            addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=sequence, outputGroup=myOutputGroup)
            jetsToWrite.append("AntiKt4TruthJets")

        # build AntiKt4TruthWZJets
        if "AntiKt4TruthWZJets" in STDMJetCollections[derivationName]:
            addStandardJets("AntiKt", 0.4, "TruthWZ", 5000, mods="truth_ungroomed", algseq=sequence, outputGroup=myOutputGroup)
            jetsToWrite.append("AntiKt4TruthWZJets")

        # build AntiKt10TruthJets
        if "AntiKt10TruthJets" in STDMJetCollections[derivationName]:
            addStandardJets("AntiKt", 1.0, "Truth", 40000, mods="truth_ungroomed", algseq=sequence, outputGroup=myOutputGroup)
            jetsToWrite.append("AntiKt10TruthJets")

        # build AntiKt10TruthWZJets
        if "AntiKt10TruthWZJets" in STDMJetCollections[derivationName]:
            addStandardJets("AntiKt", 1.0, "TruthWZ", 40000, mods="truth_ungroomed", algseq=sequence, outputGroup=myOutputGroup)
            jetsToWrite.append("AntiKt10TruthWZJets")
    return jetsToWrite


