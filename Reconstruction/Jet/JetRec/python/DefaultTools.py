# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from JetRecConf import JetPseudojetRetriever
from JetRecConf import JetConstituentsRetriever
from JetRecConf import JetRecTool
from JetRecConf import PseudoJetGetter
from JetRecConf import MuonSegmentPseudoJetGetter
from JetRecConf import JetFromPseudojet
from JetRecConf import JetConstitRemover
from JetRecConf import JetSorter

def declareDefaultTools():
  from JetRecConfig.JetRecFlags import jetFlags
  from JetRecConfig.JetRecStandardToolManager import jtm

  ghostScaleFactor = 1e-40

  #--------------------------------------------------------------
  # Jet reco infrastructure.
  #--------------------------------------------------------------

  # Jet pseudojet retriever.
  jtm += JetPseudojetRetriever("jpjretriever")

  # Jet constituent retriever.
  labs = []
  if jetFlags.useTracks():
    labs += ["Track"]
    labs += ["AntiKt2TrackJet", "AntiKt2TrackJet"]
  if jetFlags.useMuonSegments():
    labs += ["MuonSegment",]
  if jetFlags.useTruth():
    labs += ["Truth"]
    for lab in jetFlags.truthFlavorTags():
      labs += [lab]
  jtm += JetConstituentsRetriever(
    "jconretriever",
    UsePseudojet = True,
    UseJetConstituents = True,
    PseudojetRetriever = jtm.jpjretriever,
    GhostLabels = labs,
    GhostScale = ghostScaleFactor
  )

  #--------------------------------------------------------------
  # Pseudojet builders.
  #--------------------------------------------------------------

  # Clusters.
  jtm += PseudoJetGetter(
    "lcget",
    InputContainer = "CaloCalTopoClusters",
    Label = "LCTopo",
    OutputContainer = "PseudoJetLCTopo",
    SkipNegativeEnergy = True,
    GhostScale = 0.0
  )

  # EM clusters.
  jtm += PseudoJetGetter(
    "emget",
    InputContainer = "CaloCalTopoClusters",
    Label = "EMTopo",
    OutputContainer = "PseudoJetEMTopo",
    SkipNegativeEnergy = True,
    GhostScale = 0.0
  )


  # Muon segments
  jtm += MuonSegmentPseudoJetGetter(
    "gmusegget",
    InputContainer = "MuonSegments",
    Label = "GhostMuonSegment",
    OutputContainer = "PseudoJetGhostMuonSegment",
    Pt = 1.e-20
  )

  # AntiKt2 track jets.
  jtm += PseudoJetGetter(
    "gakt2trackget", # give a unique name
    InputContainer = jetFlags.containerNamePrefix() + "AntiKt2PV0TrackJets", # SG key
    Label = "GhostAntiKt2TrackJet",   # this is the name you'll use to retrieve associated ghosts
    OutputContainer = "PseudoJetGhostAntiKt2TrackJet",
    SkipNegativeEnergy = True,
    GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
  )

#  # AntiKt3 track jets.
#  jtm += PseudoJetGetter(
#    "gakt3trackget", # give a unique name
#    InputContainer = jetFlags.containerNamePrefix() + "AntiKt3PV0TrackJets", # SG key
#    Label = "GhostAntiKt3TrackJet",   # this is the name you'll use to retrieve associated ghosts
#    OutputContainer = "PseudoJetGhostAntiKt3TrackJet",
#    SkipNegativeEnergy = True,
#    GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
#  )

  # AntiKt4 track jets.
  jtm += PseudoJetGetter(
    "gakt4trackget", # give a unique name
    InputContainer = jetFlags.containerNamePrefix() + "AntiKt4PV0TrackJets", # SG key
    Label = "GhostAntiKt4TrackJet",   # this is the name you'll use to retrieve associated ghosts
    OutputContainer = "PseudoJetGhostAntiKt4TrackJet",
    SkipNegativeEnergy = True,
    GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
  )
  
  # TCCs.
  jtm += PseudoJetGetter(
    "tccget",
    InputContainer = "TrackCaloClustersCombinedAndNeutral",
    Label = "TrackCaloCluster",
    OutputContainer = "PseudoJetTCC",
    SkipNegativeEnergy = True,
    GhostScale = 0.0
  )

  #--------------------------------------------------------------
  # Jet builder.
  # The tool manager must have one jet builder.
  #--------------------------------------------------------------

  jtm.addJetBuilderWithArea(JetFromPseudojet(
    "jblda",
    Attributes = ["ActiveArea", "ActiveArea4vec"]
  ))

  jtm.addJetBuilderWithoutArea(JetFromPseudojet(
    "jbldna",
    Attributes = []
  ))

  # Number of associated muon segments.
  #jtm += JetMuonSegmentMomentsTool("muonsegs")

  # Remove constituents (useful for truth jets in evgen pile-up file)
  jtm += JetConstitRemover("removeconstit")

  # Sort jets by pT
  # May be deisred after calibration or grooming.
  jtm += JetSorter("jetsorter")
