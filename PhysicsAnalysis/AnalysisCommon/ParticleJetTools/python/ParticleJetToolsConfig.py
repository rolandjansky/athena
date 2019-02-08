# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# ParticleJetToolsConfig: A helper module for configuring tools for    #
# truth jet reconstruction and classification                          #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
jrtlog = Logging.logging.getLogger('ParticleJetToolsConfig')

from ParticleJetTools import ParticleJetToolsConf
from MCTruthClassifier import MCTruthClassifierConf

# Putting MCTruthClassifier here as we needn't stick jet configs in really foreign packages
def getMCTruthClassifier():
    # Assume mc15 value
    firstSimCreatedBarcode = 200000
    truthclassif = MCTruthClassifierConf.MCTruthClassifier(
        "JetMCTruthClassifier",
        barcodeG4Shift=firstSimCreatedBarcode,
        xAODTruthLinkVector=""
        )
    # Config neessary only for Athena releases
    import os
    if "AtlasProject" in os.environ.keys():
        if os.environ["AtlasProject"] in ["Athena","AthDerivation"]:
            truthclassif.ParticleCaloExtensionTool=""
    return truthclassif

# Generates truth particle containers for truth labeling
truthpartoptions = {
    "Partons":{"ToolType":ParticleJetToolsConf.CopyTruthPartons,"ptmin":5000},
    "BosonTop":{"ToolType":ParticleJetToolsConf.CopyBosonTopLabelTruthParticles,"ptmin":100000},
    "FlavourLabel":{"ToolType":ParticleJetToolsConf.CopyFlavorLabelTruthParticles,"ptmin":5000},
}
def getCopyTruthLabelParticles(truthtype):
    truthcategory = ""
    if truthtype == "Partons":
        truthcategory = "Partons"
    if truthtype in ["WBosons", "ZBosons", "HBosons", "TQuarksFinal"]:
        truthcategory = "BosonTop"
    else:
        truthcategory = "FlavourLabel"
    tooltype = truthpartoptions[truthcategory]["ToolType"]
    ptmin = truthpartoptions[truthcategory]["ptmin"]
    ctp = tooltype("truthpartcopy_{0}".format(truthtype),
                   PtMin = ptmin,
                   OutputName = "TruthLabel{0}".format(truthtype))
    return ctp

# Generates input truth particle containers for truth jets
def getCopyTruthJetParticles(modspec):
    truthclassif = getMCTruthClassifier()

    # Commented for now -- we ideally need a dual-use input file peeker
    # # Input file is EVNT
    # if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
    #    barCodeFromMetadata=0

    truthpartcopy = ParticleJetToolsConf.CopyTruthJetParticles(
        "truthpartcopy"+modspec,
        OutputName="JetInputTruthParticles"+modspec,
        MCTruthClassifier=truthclassif,
        BarCodeFromMetadata=2)
    if modspec=="NoWZ":
        truthpartcopy.IncludePromptLeptons=False
        # truthpartcopy.IncludePromptPhotons=False # Needs cherry-pick from 21.2
        truthpartcopy.IncludeMuons=True
        truthpartcopy.IncludeNeutrinos=True
    return truthpartcopy

def getJetQuarkLabel():
    jetquarklabel = ParticleJetToolsConf.Analysis__JetQuarkLabel(
        "jetquarklabel",
        McEventCollection = "TruthEvents"
        )
    return jetquarklabel

def getJetConeLabeling():
    jetquarklabel = getJetQuarkLabel()
    truthpartonlabel = ParticleJetToolsConf.Analysis__JetConeLabeling(
        "truthpartondr",
        JetTruthMatchTool = jetquarklabel
        )
    return truthpartonlabel

  # Cone matching for B, C and tau truth for all but track jets.
def getJetDeltaRLabelTool(modspec):
    jetptmin = float(modspec)
    jetdrlabeler = ParticleJetToolsConf.ParticleJetDeltaRLabelTool(
        "jetdrlabeler_jetpt{0}GeV".format(int(jetptmin/1000)),
        LabelName = "HadronConeExclTruthLabelID",
        DoubleLabelName = "HadronConeExclExtendedTruthLabelID",
        BLabelName = "ConeExclBHadronsFinal",
        CLabelName = "ConeExclCHadronsFinal",
        TauLabelName = "ConeExclTausFinal",
        BParticleCollection = "TruthLabelBHadronsFinal",
        CParticleCollection = "TruthLabelCHadronsFinal",
        TauParticleCollection = "TruthLabelTausFinal",
        PartPtMin = 5000.,
        DRMax = 0.3,
        MatchMode = "MinDR"
        )
    jetdrlabeler.JetPtMin = jetptmin
    return jetdrlabeler

