/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkV0Vertex_page TrkV0Vertex Package

@author Evelina.Bouhova@cern.ch
@author Kirill.Prokofiev@cern.ch

@section TrkV0Vertex_IntroductionTrkV0Vertex Introduction
This is and Event Data Model package, designed to 
store the information about reconstructed V0 candidates, 
such as photon conversions, kaons, lambdas and so.

@section TrkV0Vertex_designTrkV0Vertex Design
<b><i>Trk::V0Hypothesis</i></b> Inherits from <i>Trk::ExtendedVxCandidate</i>. 
In addition it contains the assumed PDG ID 
of the positive and negative tracks, making the V0 and the assumed PDG ID of the V0. 
Possible V0 hypotheses are: Unconstrained (i.e. no mass or pointing constraints applied in the fit) 
and the mass constrained hypotheses: Kshort (K_S0 -> pi_plus pi_minus), Lambda (Lambda0 -> p_plus pi_minus), 
LambdaBar (anti_Lambda0 -> pi_plus anti_p_minus) and Gamma conversion (gamma -> e_plus e_minus).
<br><br>
<b><i>Trk::V0Candidate</i></b> Always contains the Unconstrained hypothesis 
plus one or more mass constrained hypotheses (reconstructed from the same pair of oppositely charged tracks).
<br><br>
<b><i>V0Container</i></b> This is a top level object of the <i>TrkV0Vertex</i> package.
It represents a collection of <i>Trk::V0Candidate<i>'s reconstructed by a V0 finding
algorithm and written to the StoreGate for further use.


*/
