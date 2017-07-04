# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################################
# Common code used for the HIGG4 jet building and calibration   #
# Z.Zinonos                                     		#
# zenon@cern.ch                                 		#
# Nov 2015                                      		#
#################################################################

from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

def setup(HIGG4DxName, HIGG4DxSequence, HIGG4DxSlimmingHelper):
    
    jet_collection = HIGG4DxName+"Jets"
    if not jet_collection in OutputJets:
        # AntiKt10*PtFrac5Rclus20 jets
        if HIGG4DxName in ['HIGG4D2', 'HIGG4D3']:
            
            # in rel 21, AntiKt4PV0TrackJetsContainer has to be re-created on the fly
            # this no longer works. The implementation below is needed.
#            addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=HIGG4DxSequence, outputGroup=HIGG4DxName)
#            addStandardJets("AntiKt", 0.2, "PV0Track", 2000, mods="track_ungroomed", algseq=HIGG4DxSequence, outputGroup=HIGG4DxName)
            
            reducedJetList = ["AntiKt4PV0TrackJets", 
                              "AntiKt2PV0TrackJets",
                              "AntiKt4TruthJets",
                              "AntiKt4TruthWZJets"]
                              
            replaceAODReducedJets(reducedJetList,HIGG4DxSequence,HIGG4DxName)
            
            # default trimmed jets. Does this still work?
            addDefaultTrimmedJets(HIGG4DxSequence, jet_collection, True)

         # I think this is not needed
#        if HIGG4DxName in ['HIGG4D2', 'HIGG4D3']:
#            OutputJets[jet_collection].append("AntiKt2PV0TrackJets")

