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

#    if not DAOD_StreamID+"Jets" in OutputJets:
    
        # add a small-R collection
#        OutputJets[DAOD_StreamID+"Jets"] = ["AntiKt2PV0TrackJets"]

    	# schedule reconstruction of AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets 
#        addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, algseq=DerivationFrameworkJob)
#        OutputJets[DAOD_StreamID+"Jets"].append("AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets")
#        print "HELLO HERE", OutputJets[DAOD_StreamID+"Jets"]
    
    jet_collection = HIGG4DxName+"Jets"
    if not jet_collection in OutputJets:
        # AntiKt10*PtFrac5Rclus20 jets
        if HIGG4DxName in ['HIGG4D2', 'HIGG4D3']:
            addDefaultTrimmedJets(HIGG4DxSequence, jet_collection, False)

        if HIGG4DxName in ['HIGG4D2', 'HIGG4D3']:
            OutputJets[jet_collection].append("AntiKt2PV0TrackJets")

