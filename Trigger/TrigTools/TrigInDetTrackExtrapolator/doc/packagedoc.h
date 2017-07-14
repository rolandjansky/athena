/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigInDetTrackExtrapolator_page TrigInDetTrackExtrapolator Package

This package contains an implementation of the LVL2 ID track extrapolator tool.
This tool is based on an abstract interface ITrigInDetTrackExtrapolator.  

@author Dmitry.Emeliyanov@cern.ch

@section TrigInDetTrackExtrapolator_TrigInDetTrackFitOverview Overview

The main functonality of this tool is to provide extrapolation of TrigInDetTrack track
parameters from perigee point to the calorimeter surface. Only track parameters are extrapolated, 
i.e. no extrapolated covariance is provided. 

The tool has the following public methods

  - TrigInDetTrackFitPar* extrapolateToBarrel(const TrigInDetTrack*, double R) - extrapolates to a 
barrel surface with radius R, returns a new track parameters on the surface

  - TrigInDetTrackFitPar* extrapolateToEndcap(const TrigInDetTrack*, double Z) - extrapolates to a 
endcap surface with coordinate Z, returns a new track parameters on the surface

  - StatusCode extrapolateToCalo(const TrigInDetTrack*,double Rcalo, double Zcalo, 
double& phiC, double& etaC) - extrapolates to the calorimeter surface assumed to be a cylinder with 
radius Rcalo and half-length of Zcalo, returns phiC, etaC - coordinates of the track impact point on 
the calo surface.

*/  
