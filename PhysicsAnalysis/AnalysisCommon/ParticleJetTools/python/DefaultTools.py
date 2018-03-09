# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def declareDefaultTools():

  from JetRecConfig.JetRecFlags import jetFlags
  from JetRecConfig.JetRecStandardToolManager import jtm
  from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
  from JetRec.JetRecConf import PseudoJetGetter

  try:
    from ParticleJetTools.ParticleJetToolsConf import Analysis__JetQuarkLabel
    jtm.haveParticleJetTools = True
  except:
    jtm.haveParticleJetTools = False
  if jtm.haveParticleJetTools:
    from ParticleJetTools.ParticleJetToolsConf import Analysis__JetConeLabeling
    from ParticleJetTools.ParticleJetToolsConf import Analysis__JetPartonTruthLabel
    from ParticleJetTools.ParticleJetToolsConf import CopyTruthJetParticles
    from ParticleJetTools.ParticleJetToolsConf import ParticleJetDeltaRLabelTool

  ghostScaleFactor = 1e-40

  #--------------------------------------------------------------
  # Truth selection.
  #--------------------------------------------------------------

  if jetFlags.useTruth:
      truthClassifier = MCTruthClassifier(name = "JetMCTruthClassifier",
                                         ParticleCaloExtensionTool="")
      jtm += truthClassifier

      jtm += CopyTruthJetParticles("truthpartcopy", OutputName="JetInputTruthParticles",
                                   MCTruthClassifier=truthClassifier)
      jtm += CopyTruthJetParticles("truthpartcopywz", OutputName="JetInputTruthParticlesNoWZ",
                                   MCTruthClassifier=truthClassifier,
                                   IncludePromptLeptons=False,
                                   IncludeMuons=True,IncludeNeutrinos=True)

  # Truth.
  if jetFlags.useTruth and jtm.haveParticleJetTools:
    # ParticleJetTools tools may be omitted in analysi releases.
    #ift jtm.haveParticleJetTools:
    # Delta-R truth parton label: truthpartondr.
    jtm += Analysis__JetQuarkLabel(
        "jetquarklabel",
        McEventCollection = "TruthEvents"
      )
    jtm += Analysis__JetConeLabeling(
        "truthpartondr",
        JetTruthMatchTool = jtm.jetquarklabel
        )

    # Parton truth label.
    jtm += Analysis__JetPartonTruthLabel("partontruthlabel")

    # Cone matching for B, C and tau truth for all but track jets.
    jtm += ParticleJetDeltaRLabelTool(
      "jetdrlabeler",
      LabelName = "HadronConeExclTruthLabelID",
      DoubleLabelName = "HadronConeExclExtendedTruthLabelID",
      BLabelName = "ConeExclBHadronsFinal",
      CLabelName = "ConeExclCHadronsFinal",
      TauLabelName = "ConeExclTausFinal",
      BParticleCollection = "TruthLabelBHadronsFinal",
      CParticleCollection = "TruthLabelCHadronsFinal",
      TauParticleCollection = "TruthLabelTausFinal",
      PartPtMin = 5000.0,
      JetPtMin =     0.0,
      DRMax = 0.3,
      MatchMode = "MinDR"
    )

    # Cone matching for B, C and tau truth for track jets.
    jtm += ParticleJetDeltaRLabelTool(
      "trackjetdrlabeler",
      LabelName = "HadronConeExclTruthLabelID",
      DoubleLabelName = "HadronConeExclExtendedTruthLabelID",
      BLabelName = "ConeExclBHadronsFinal",
      CLabelName = "ConeExclCHadronsFinal",
      TauLabelName = "ConeExclTausFinal",
      BParticleCollection = "TruthLabelBHadronsFinal",
      CParticleCollection = "TruthLabelCHadronsFinal",
      TauParticleCollection = "TruthLabelTausFinal",
      PartPtMin = 5000.0,
      JetPtMin = 4500.0,
      DRMax = 0.3,
      MatchMode = "MinDR"
    )

    jtm += PseudoJetGetter(
      "truthget",
      Label = "Truth",
      InputContainer = jtm.truthpartcopy.OutputName,
      OutputContainer = "PseudoJetTruth",
      GhostScale = 0.0,
      SkipNegativeEnergy = True,

    )
    jtm += PseudoJetGetter(
      "truthwzget",
      Label = "TruthWZ",
      InputContainer = jtm.truthpartcopywz.OutputName,
      OutputContainer = "PseudoJetTruthWZ",
      GhostScale = 0.0,
      SkipNegativeEnergy = True,

    )
    jtm += PseudoJetGetter(
      "gtruthget",
      Label = "GhostTruth",
      InputContainer = jtm.truthpartcopy.OutputName,
      OutputContainer = "PseudoJetGhostTruth",
      GhostScale = ghostScaleFactor,
      SkipNegativeEnergy = True,
    )

    # Truth flavor tags.
    for ptype in jetFlags.truthFlavorTags():
      jtm += PseudoJetGetter(
        "gtruthget_" + ptype,
        InputContainer = "TruthLabel" + ptype,
        Label = "Ghost" + ptype,
        OutputContainer = "PseudoJetGhost" + ptype,
        SkipNegativeEnergy = True,
        GhostScale = ghostScaleFactor,
      )


