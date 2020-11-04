# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
import DerivationFrameworkTop.TOPQCommonExtraContent as tec
from DerivationFrameworkJetEtMiss.METCommon import addMETOutputs
from DerivationFrameworkJetEtMiss.JetCommon import addJetOutputs

import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as Config

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
  TOPQSlimmingHelper.SmartCollections += tec.TOPQSmartSlimmingCollections

  print ("TOPQSlimmingHelper.SmartCollections: " , TOPQSlimmingHelper.SmartCollections)

  #=================================================================
  # EXTRA VARIABLES FROM DerivationFrameworkTop.TOPQCommonExtraContent
  #=================================================================
  TOPQSlimmingHelper.ExtraVariables = []
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesAntiKt4EMTopoJets
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesAntiKt4EMPFlowJets
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesBTagging_AntiKt4EMPFlow
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesBTagging_AntiKt4EMTopo
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesPhotons
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesElectrons
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesMuons
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesTaus
  TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesTrackJets
  TOPQSlimmingHelper.ExtraVariables += Config.GetExtraPromptVariablesForDxAOD()
  #  TOPQSlimmingHelper.ExtraVariables += ["CaloCalTopoClusters.calE.calEta.calPhi.calM.rawM.rawE.rawEta.rawPhi.e_sampl.eta_sampl.etaCalo.phiCalo"]
  TOPQSlimmingHelper.ExtraVariables += ["CaloCalTopoClusters.calPt.calEta.calPhi.calM.calE.CENTER_MAG"]

  if DFisMC:
    TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesPhotonsTruth
    TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesElectronsTruth
    TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesMuonsTruth
    TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVariablesTausTruth
    TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVarsBTag_HLT_Container
    TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVarsJet_EF_Container
    TOPQSlimmingHelper.ExtraVariables += tec.TOPQExtraVarsJet_Split_Container

  print ("TOPQSlimmingHelper.ExtraVariables: " , TOPQSlimmingHelper.ExtraVariables)

  #================================
  # EXTRA COLLECTIONS - user added
  #================================
  TOPQSlimmingHelper.AllVariables = []
  TOPQSlimmingHelper.AllVariables += tec.TOPQExtraContainersStandard
  TOPQSlimmingHelper.AllVariables += tec.TOPQExtraContainersTrigger

  if DFisMC:
    TOPQSlimmingHelper.AllVariables += tec.TOPQExtraContainersTruth

  print ("TOPQSlimmingHelper.AllVariables: " , TOPQSlimmingHelper.AllVariables)

  #================================
  # CREATED ON-THE-FLY COLLECTIONS
  #================================
  TOPQSlimmingHelper.StaticContent = []
  TOPQSlimmingHelper.StaticContent += tec.TOPQStaticContent

  if DFisMC:
    TOPQSlimmingHelper.StaticContent += tec.TOPQStaticContentTruth

  print ("TOPQSlimmingHelper.StaticContent: " , TOPQSlimmingHelper.StaticContent)

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
