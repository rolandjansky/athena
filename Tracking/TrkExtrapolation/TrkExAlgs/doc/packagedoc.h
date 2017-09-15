/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkExAlgs_page TrkExAlgs Package

The TrkExAlgs package is designed for hosting test and validation algorithms
that are matter of the track extrapolation.

@author Andreas Salzburger <Andreas.Salzburger@cern.ch>

@section TrkExAlgs_TrkExAlgsIntro Introduction

The validaiton algorithms that can be found in this package are:

   * ExtrapolationValidation : a self-consistency check of extrapolation processes
     (including navigation, material effects integration)
   * RiddersAlgorithm : a numerical check of the transport jacobians provided
     by ther various propagation engines

The algorithms can be steered through jobOptions files that are located in the
TrkExExample package.



*/
