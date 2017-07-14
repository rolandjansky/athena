/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigBjetHypo_page TrigBjetHypo Package

@author Andrea.Coccaro@ge.infn.it

@section TrigBjetHypo_Introduction 

This package contains the HLT b-tagging selection based on the likelihood ratio method and on a track-based chi2 probability method.
It is running both at LVL2 and EF and it basically contains a feature extraction algorithm to compute the b-tagging weight of all the different taggers and an hypothesis algorithm which implements the selection.

The track-based chi2 probability tagger (CHI2) computes the probability for a jet to originate from the primary vertex using the signed transverse impact parameter significance of tracks pointing to the jet.

The likelihood taggers implemented are based on the signed impact parameter significance of tracks and on the different properties of secondary vertices; in particular:
- significance of longitudinal impact parameter (IP1D);
- significance of transverese impact parameter (IP2D);
- 2D combination of the track-based methods (IP3D);
- invariant mass of tracks linked to the secondary vertex (MVTX);
- energy fraction of the secondary vertex (EVTX);
- number of tracks lined to the secondary vertex (NVTX);
- 3D combination of the vertex-based methods (SVTX);
- combination of the two likelihoods based on tracks and secondary vertex informations (COMB).

Hypothesis algorithm can perform the selection using these different taggers: CHI2, IP2D, IP3D or COMB.

In order to optimize b-tagging performance keeping the reconstruction efficiency as high as possible, in the feature extraction algorithm a dedicated track selection has been added.



*/
