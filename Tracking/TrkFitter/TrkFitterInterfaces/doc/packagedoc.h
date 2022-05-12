/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkFitterInterfaces_page TrkFitterInterfaces Package

This package contains interface classes to be used by the
ATLAS Track Fitters
Concrete implementations can be found in each fitter code

@author Wolfgang.Liebig <http://consult.cern.ch/xwho/people/485812>

@section TrkFitterInterfaces_TrkFitterInterfacesIntro Introduction

The following interface classes are defined

   - Trk::ITrackFitter               : interface for EDM track fitters
   - Trk::IGlobalTrackFitter         : extension for least-squares fitters which give out a deriv matrix

This package is not built as any library, it is a simple include package.   

*/
