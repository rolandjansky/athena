/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigL2LongLivedParticles_page 
@author Stefano Giagu, Antonio Policicchio

@section TrigL2LongLivedParticles_TrigL2LongLivedParticlesOverview Overview
This package contains LVL2 Fex algorithms for Long-Lived 
neutral particle triggers: MuonCluster and  TrigMuonJetFex.
The MuonCluster AllTE algorithm is consisting of a muon ROI cluster 
algorithm which computes the number of LVL1 muon  ROIs in a 
cone of DR = 0.4. Then it looks for jets associated in a 
cone of Dr=0.7 around the L1 ROI cluster and computes the number 
of tracks with a Pt > 2 GeV in a region etaxphi 0.2x0.2 
around the muon cluster.  
All relevant variable are saved in a TrigMuonClusterFeature abject attached to the  
event record for further analysis.

The TrigMuonJetFex is a Combo Fex algorithm which looks for angular matching between a LVL2 jet and a LVL1 muon.
The LVL2 jet has to be a jet without connecting  tracks to the interaction point. In order to accompish this request the
algorithm computes the number 
of tracks with a Pt > 1 GeV in a region etaxphi 0.2x0.2 
around the jet direction. No feature is written for this algorithm.



*/
