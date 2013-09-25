/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrkMultiComponentStateOnSurface Package

Generalisation of the TrackStateOnSurface to allow for a number of components to describe the state rather than a single set of Trk::TrackParameters. In this case the normal Trk::TrackParameters pointer is replaced with a Trk::MultiComponentState pointer.

@author Tom.Atkinson@cern.ch

@section TrkMultiCoSoSIntro Introduction

The TrkMultiComponentStateOnSurface package is designed for the implementation of the GaussianSumFilter. The Gaussian Sum Filter requires that a number of sets of Trk::TrackParameters objects describe the state at every surface. In this regard the Gaussian Sum Filter behaves like a number of Kalman Filters running in parallel. The TrkMultiComponentStateOnSurface package provides this functionality.

@section TrkMultiCoSoSOverview Class Overview

   There are several classes in this package that not only provide the functionality for the Trk::MultiComponentStateOnSurface but are also responsible for all the memory management of the class. This issue will also be covered in this document.

   The Trk::ComponentParameters class is a std::pair containing a pointer to a Trk::TrackParameters object and a double representing the weighting of the state. The combination of many of these components comprise a Trk::MultiComponentState object which is a list of ComponentParameters.

   The Trk::ComponentParameters object, when deleted does NOT delete the data pointed to by the Trk::TrackParameters pointer; it simply removes the pair. This decision was made because the Trk::ComponentParameters object is normally (though not always) passed to become part of a Trk::MultiComponentState container. Thus the Trk::MultiComponentState is responsible for the deletion of the Trk::TrackParameters information.

   The Trk::MultiComponentState also includes methods to print the state information, renormalise the state weightings to 1, rescale the covariance matrix (useful in Gaussian Sum Smoothing, and checks to see if the MultiComponentState has components with associated error matricies.

   The Trk::MultiComponentStateOnSurface class is the equivalent to the Trk::TrackStateOnSurface class, with the exception that it holds a Trk::MultiComponentState pointer, rather than a Trk::TrackParameters pointer. Because it has to call explicitly the Trk::TrackStateOnSurface constructor, and that constructor requires a pointer to a Trk::TrackParameters object, a design decision was taken to pass the term with leading weight in the Trk::MultiComponentState as this parameter. From a conceptual perspective it would be preferable to pass the combined state (a weighted average of all components), but as this would involve the use of the Trk::MultiComponentStateCombiner AlgTool, this was not possible (it is a design requirement of EDM classes to avoid the use of AlgTools in the data classes!)

  */  
