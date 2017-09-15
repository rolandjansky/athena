/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkDeterministicAnnealingFilter_page The TrkDeterministicAnnealingFilter package
The Deterministic Annealing Filter (DAF) implementation.

@author Sebastian.Fleischmann@cern.ch

@section TrkDeterministicAnnealingFilter_introductionTrkDeterministicAnnealingFilter Introduction

The TrkDeterministicAnnealingFilter package implements the deterministic annealing filtering
technique for track fitting, as proposed by Fr&uuml;hwirth and Strandlie in 1999 [1].

This package contains the high-level steering algorithms of the DAF. The actual calculations
are performed by several other packages which depend in many cases on detector-dependend knowledge.
@image html CantGetEnough.jpg

@section TrkDeterministicAnnealingFilter_TrkDeterministicAnnealingFilterOverview Class Overview
  The TrkDeterministicAnnealingFilter package contains the following classes:

  - Trk::DeterministicAnnealingFilter : DAF steering algorithm, prepares the fit and performs the annealing iterations
  - Trk::DAF_WeightCalculator : Tool for calculating the assignment probabilities according to the DAF formulae
  - Trk::DAF_SimpleWeightCalculator : Tool for calculating the assignment probabilities according to the DAF formulae with simplified normalization (modification to formulas in [1])
  
@section TrkDeterministicAnnealingFilter_TrkDeterministicAnnealingFilterDocReferences References
[1] R. Fr&uuml;hwirth, A. Strandlie, <i>Track fitting with ambiguities and noise:
a study of elastic tracking and non-linear filters</i>,  Computer Physics Communications (1999) vol. 120, no. 2-3, pp. 197-214.
    
@section TrkDeterministicAnnealingFilter_ExtrasTrkDeterministicAnnealingFilter Extra Pages

*/
