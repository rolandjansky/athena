/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigLongLivedParticlesHypo_page 
@author Stefano Giagu, Antonio Policicchio

@section TrigLongLivedParticlesHypo_TrigLongLivedParticlesHypoOverview Overview
This package contains LVL2 Hypotesis  algorithms for Long-Lived 
neutral particle triggers: MuonClusterHypo, TrigL2HVJetHypo and TrigMuonJetHypo.
The MuonClusterHypo applies cuts on the TrigMuonClusterFeature written by the MuonCluster Fex.
The TrigMuonJetHypo is a simply pass=true algorithm for TrigMuonJetFex.
The TrigL2HVJetHypo(Trk) is a modified version of the standard LVL2 jet hypothesis algorithm. 
The LVL2 jet has to be a jet without connecting  tracks to the interaction point. 
In order to accomplish this request the
algorithm computes the number 
of tracks with a Pt > 1 GeV in a region etaxphi 0.2x0.2 
around the jet direction. Moreover the jet has to be the non-standard value for Log(Ehad/Eem)>1.  



*/
