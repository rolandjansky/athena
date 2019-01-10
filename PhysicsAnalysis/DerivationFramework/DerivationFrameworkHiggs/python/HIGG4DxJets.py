# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################################
# Common code used for the HIGG4 jet building and calibration   #
# Z.Zinonos                                     		#
# zenon@cern.ch                                 		#
# Nov 2015                                      		#
#################################################################

from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit

def setup(HIGG4DxName, HIGG4DxSequence, HIGG4DxSlimmingHelper):
    
    if not HIGG4DxName in OutputJets:
        reducedJetList = ["AntiKt4TruthJets",
                          "AntiKt4TruthWZJets",
                          ]
      
        if HIGG4DxName in ['HIGG4D2', 'HIGG4D3', 'HIGG4D6', 'HDBS1']:
            
            reducedJetList += ["AntiKt4PV0TrackJets", 
                               "AntiKt2PV0TrackJets",
                               "AntiKt10LCTopoJets"
                               ]
                              
        replaceAODReducedJets(reducedJetList,HIGG4DxSequence,HIGG4DxName)
        
        # AntiKt4EMPFlow is not tagged by default. Need to re-tag:
        FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = HIGG4DxSequence)
            
        if HIGG4DxName in ['HIGG4D2', 'HIGG4D3', 'HIGG4D6']:
            # default trimmed jets.
            addDefaultTrimmedJets(HIGG4DxSequence, HIGG4DxName, True)
    if HIGG4DxName in ['HDBS1']:
        import DiTauRec.DiTauAlgorithmsHolder as DiTauAlgs
        ditauTools = []
        ditauTools.append(DiTauAlgs.getSeedJetBuilder("AntiKt10LCTopoJets"))
        ditauTools.append(DiTauAlgs.getElMuFinder())
        ditauTools.append(DiTauAlgs.getSubjetBuilder())
        ditauTools.append(DiTauAlgs.getVertexFinder())
        ditauTools.append(DiTauAlgs.getDiTauTrackFinder())
        ditauTools.append(DiTauAlgs.getIDVarCalculator(False))

        from DiTauRec.DiTauRecConf import DiTauBuilder
        DiTauBuilder = DiTauBuilder(
            name="DiTauBuilderLowPt",
            DiTauContainer="DiTauJetsLowPt",
            DiTauAuxContainer="DiTauJetsLowPtAux.",
            Tools=ditauTools,
            SeedJetName="AntiKt10LCTopoJets",
            minPt=50000,
            maxEta=2.5,
            OutputLevel=2,
            Rjet=1.0,
            Rsubjet=0.2,
            Rcore=0.1)

        HIGG4DxSequence += DiTauBuilder


