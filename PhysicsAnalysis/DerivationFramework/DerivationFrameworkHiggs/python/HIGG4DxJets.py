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
    
    if not HIGG4DxName in OutputJets:
        reducedJetList = ["AntiKt4TruthJets",
                          "AntiKt4TruthWZJets",
                          ]
      
        if HIGG4DxName in ['HIGG4D2', 'HIGG4D3', 'HIGG4D6']:
            
            reducedJetList += ["AntiKt4PV0TrackJets", 
                               "AntiKt2PV0TrackJets",
                               "AntiKt10LCTopoJets"
                               ]
                              
        replaceAODReducedJets(reducedJetList,HIGG4DxSequence,HIGG4DxName)
            
        if HIGG4DxName in ['HIGG4D2', 'HIGG4D3', 'HIGG4D6']:
            # default trimmed jets.
            addDefaultTrimmedJets(HIGG4DxSequence, HIGG4DxName, True)

