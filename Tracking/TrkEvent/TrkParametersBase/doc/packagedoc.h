/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkParametersBase_page TrkParametersBase Package

@author Andreas.Salzburger@cern.ch

@section TrkParametersBase_TrkParametersBaseIntro Introduction

This package defines all track parameterisations in ATLAS by a set of 3 classes

   - template <int DIM, class T, class S> Trk::ParametersT
   - template <int DIM, class T, class S> Trk::CurvilinearParametersT
   - template <int DIM, class T> Trk::ParametersBase

The template arguments are 
   - int DIM : the dimension of the parameterisation 5 for standard, 6 for extended with mass
   - class T : the type can either by Charged or Neutral
   - class S : the surface from Trk::Surface to define the parameterisation frame

All local/global transformations are outsourced to the surfaces.

Note that CurvilinearParameters make best sense on planar surfaces.



*/
