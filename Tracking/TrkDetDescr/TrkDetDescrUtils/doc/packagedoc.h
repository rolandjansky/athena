/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkDetDescrUtils_page TrkDetDescrUtils Package

This Package contains useful utilities for creating the TrackingGeometry.

@author Andreas.Salzburger@cern.ch

@section TrkDetDescrUtils_TrkDetDescrIntro Introduction

The classes in this package are purely inline for speed optimisation or
for the fact that they are class templates.
The package contains a Trk::BinnedArray template class that allows to order 
geometrical objects into an array of 1-dim or 2-dim binning. The binning is done by Trk::BinUtility
classes of different implementations.

In addition it contains geometry statics and a reference counting class.




*/
