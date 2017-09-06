/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkVertexAnalysisUtils_page The TrkVertexAnalysisUtilsInterfaces package

@author Evelina.Bouhova@cern.ch

@section TrkVertexAnalysisUtils_introductionTrkVertexAnalysisUtils Introduction

This package contains methods to calculate a number of properties of the vertices returned by the TrkV0Fitter.

@section TrkVertexAnalysisUtils_designTrkVertexAnalysisUtils Design
<br>
<b></i>V0Tools</i></b>
Methods specific to V0 reconstructed objects. V0 vertices contain two tracks, the first of which is always the positive one.
All methods take Trk::ExtendedVxCandidate* or Trk::V0Hypothesis* as an argument.

  - Methods, returning pointers to the different V0 Hypotheses : <b></i>v0Unconstr</i></b>, <b></i>v0Kshort</i></b>, 
<b></i>v0Lambda</i></b>, <b></i>v0LambdaBar</i></b>, <b></i>v0Gamma</i></b>. 

  - Methods, returning the invariant mass, error on the invariant mass and  
Chi2 probability of the invariant mass of a V0Hypothesis
for a given hypothesis for the masses (doubles) of the input tracks and the V0 mass (double) : <b></i>invariantMass</i></b>, 
<b></i>invariantMassError</i></b>, <b></i>invariantMassProbability</i></b>.

  - Methods, returning the refitted 3-momenta of the positive and negative tracks and the V0 : Trk::GlobalMomentum 
<b></i>positiveTrackMomentum</i></b>, <b></i>negativeTrackMomentum</i></b>, <b></i>V0Momentum</i></b>

  - Methods, returning the refitted 4-momenta of the positive and negative tracks
and the V0 for a given hypothesis for the masses of the input tracks and the V0 mass :
CLHEP::HepLorentzVector <b></i>positiveTrack4Momentum</i></b>, <b></i>negativeTrack4Momentum</i></b>, <b></i>V04Momentum</i></b>

  - NDoF of the vertex fit : int <b></i>ndof</i></b>

  - Chisq of the vertex fit : double <b></i>chisq</i></b>

  - Chisq probability of the vertex fit : double <b></i>vertexProbability</i></b>

  - vertex position : CLHEP::Hep3Vector <b></i>vtx</i></b>

  - Rxy of the vertex : double <b></i>rxy</i></b>, can be calculated with respect to 0. (if no vertex specified) or 
Trk::RecVertex, Trk::Vertex and CLHEP::Hep3Vector

  - pointers to original Trk::Track : <b></i>positiveOrigTrack</i></b>, <b></i>negativeOrigTrack</i></b>

<br><br>



*/
