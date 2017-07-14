/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkAlgebraUtils_page TrkAlgebraUtils
@author roberth@bu.edu
@author Daniel Kollar <daniel.kollar@cern.ch>

@section TrkAlgebraUtils_TrkAlgebraUtilsIntro Introduction

This package was copied directly from SiGlobalChi2AlgebraUtils.  It contains 
the classes used to store and manipulate large matrices and vectors used for 
alignment of detector modules.  

@section TrkAlgebraUtils_TrkAlgebraUtilsOverview Class Overview
  The package contains the following classes:

  - AlMat:  handles general NxM matrices

  - AlSymMatBase:  inherits from AlMat, contains base implementation for 
    handling symmetric matrices
  - AlSymMat:  inherits from AlSymMatBase, contains implementation for 
    handling symmetric matrices
  - AlSpaMat:  inherits from AlSymMatBase, contains implementation for 
    handling sparse symmetric matrices

  - IPCMat:  contains methods for storing and retrieving matrices in 
    binary format, useful for parallel processing

  - AlVec:  handles vector of doubles
  - IntVec:  handles vector of integers



*/
