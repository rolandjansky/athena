# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#====================================================================
# Common file used for TOPQ slimming
# Call with:
#     import DerivationFrameworkTop.TOPQCommonSlimming
#     DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQX', TOPQStream)
#====================================================================

#================================
# IMPORTS
#================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from AthenaCommon.GlobalFlags import globalflags
from DerivationFrameworkTop.TOPQCommonExtraContent import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addOriginCorrectedClusters

import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

def setup(TOPQname, stream):
  DFisMC = (globalflags.DataSource()=='geant4')

  TOPQSlimmingHelper = SlimmingHelper(TOPQname + "SlimmingHelper")

  #=====================================================
  # ADD BTagging_AntiKt4EMPFlow COLLECTION TO DICTIONARY
  #=====================================================
  TOPQSlimmingHelper.AppendToDictionary = {"BTagging_AntiKt4EMPFlow" : "xAOD::BTaggingContainer", "BTagging_AntiKt4EMPFlowAux" : "xAOD::BTaggingAuxContainer"}

  #================================
  # SMART SLIMMING
  #================================
  TOPQSlimmingHelper.SmartCollections =  []
  TOPQSlimmingHelper.SmartCollections += TOPQSmartSlimmingCollections

  print "TOPQSlimmingHelper.SmartCollections: " , TOPQSlimmingHelper.SmartCollections

  #=================================================================
  # EXTRA VARIABLES FROM DerivationFrameworkTop.TOPQCommonExtraContent
  #=================================================================
  TOPQSlimmingHelper.ExtraVariables = []
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesAntiKt4EMTopoJets
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesAntiKt4EMPFlowJets
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesBTagging_AntiKt4EMPFlow
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesBTagging_AntiKt4EMTopo
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesPhotons
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesElectrons
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesMuons
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesTaus
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesTrackJets
  TOPQSlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
  TOPQSlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()
  #  TOPQSlimmingHelper.ExtraVariables += ["CaloCalTopoClusters.calE.calEta.calPhi.calM.rawM.rawE.rawEta.rawPhi.e_sampl.eta_sampl.etaCalo.phiCalo"]
  TOPQSlimmingHelper.ExtraVariables += ["CaloCalTopoClusters.calPt.calEta.calPhi.calM.calE.CENTER_MAG"]

  # Also write the origin-corrected clusters for both EMTopo (small-R-jets) and
  # LCTopo (large-R-jets).
  addOriginCorrectedClusters(TOPQSlimmingHelper, writeLC=True, writeEM=True)

  if DFisMC:
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesPhotonsTruth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesElectronsTruth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesMuonsTruth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariablesTausTruth
# add these variables to all MC and data (TOPQ4 only)
  if DFisMC or TOPQname == 'TOPQ4':
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVarsBTag_HLT_Container
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVarsJet_EF_Container
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVarsJet_Split_Container

  print "TOPQSlimmingHelper.ExtraVariables: " , TOPQSlimmingHelper.ExtraVariables

  #================================
  # EXTRA COLLECTIONS - user added
  #================================
  TOPQSlimmingHelper.AllVariables = []
  TOPQSlimmingHelper.AllVariables += TOPQExtraContainersStandard
  TOPQSlimmingHelper.AllVariables += TOPQExtraContainersTrigger

  if DFisMC:
    TOPQSlimmingHelper.AllVariables += TOPQExtraContainersTruth

  print "TOPQSlimmingHelper.AllVariables: " , TOPQSlimmingHelper.AllVariables

  #================================
  # CREATED ON-THE-FLY COLLECTIONS
  #================================
  TOPQSlimmingHelper.StaticContent = []
  TOPQSlimmingHelper.StaticContent += TOPQStaticContent

  if DFisMC:
    TOPQSlimmingHelper.StaticContent += TOPQStaticContentTruth

  print "TOPQSlimmingHelper.StaticContent: " , TOPQSlimmingHelper.StaticContent

  #================================
  # TRIGGER CONTENT
  #================================
  TOPQSlimmingHelper.IncludeMuonTriggerContent   = True
  TOPQSlimmingHelper.IncludeEGammaTriggerContent = True
  TOPQSlimmingHelper.IncludeJetTriggerContent    = True
  TOPQSlimmingHelper.IncludeTauTriggerContent    = True
  TOPQSlimmingHelper.IncludeEtMissTriggerContent = True
  #TOPQSlimmingHelper.IncludeJetTauEtMissTriggerContent = True
  #TOPQSlimmingHelper.IncludeBJetTriggerContent         = True
  #TOPQSlimmingHelper.IncludeBPhysTriggerContent        = True
  #TOPQSlimmingHelper.IncludeMinBiasTriggerContent      = True

  #====================================
  # ADD AntiKT4EMPFlowJets MET OUTPUT
  #====================================
  # needs to be after where AllVaraibles[] and StaticContent[] are created
  # adds METAssoc_XXXX, MET_Core_XXXX, and MET_Reference_XXXX
  # uses DerivationFrameworkJetEtMiss.METCommon.py
  addMETOutputs(TOPQSlimmingHelper,["AntiKt4EMPFlow"])

  #====================================
  # ADD CUSTOM JET OUTPUT (new rel 21 method)
  #====================================
  addJetOutputs(TOPQSlimmingHelper,["SmallR", "LargeR", TOPQname],
                [], # smart list
                ["AntiKt3PV0TrackJets",
                 "AntiKt4PV0TrackJets",
                 "AntiKt4LCTopoJets",
                 "AntiKt10LCTopoJets",
                 "AntiKt10TruthWZJets",
                 ]# veto list
  )

  #================================
  # THIS NEEDS TO BE THE LAST LINE
  #================================
  TOPQSlimmingHelper.AppendContentToStream(stream)
# end setup(TOPQname, stream)
