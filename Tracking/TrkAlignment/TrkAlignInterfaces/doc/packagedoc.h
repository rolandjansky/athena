/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkAlignInterfaces_page TrkAlignInterfaces
@author roberth@bu.edu
@author Daniel Kollar <daniel.kollar@cern.ch>

@section TrkAlignInterfaces_TrkAlignInterfacesIntro Introduction

This package contains virtual interfaces for tools to be used for track-based alignment.

@section TrkAlignInterfaces_TrkAlignInterfacesOverview Class Overview
  The package contains the following classes:

  - IAlignTrackPreProcessor: processes the collection of Tracks and creates a vector of AlignTracks
  - IAlignTrackCreator: processes AlignTrack and assigns a collection AlignTSOS to it   
  - IAlignTrackDresser: dresses AlignTrack with derivatives, residuals, etc.
  - IAlignTool: accumulates information from tracks and solves for alignment parameters
  - IAlignModuleTool: provides methods to handle AlignModules and AlignPars
  - IFillNtupleTool: can be inherited by detector-specific tool to write output ntuple  
  - IGeometryManagerTool: inherited by detector-specific tool for building geometry of AlignModules to be aligned  
  - IMatrixTool: inherited by MatrixTool and IPCMatrixTool for dealing with large matrices
  - IDerivCalcTool: inherited by tools that calculate derivatives
  - IJacobianManagerTool: not yet used
  - ITrkAlignDBTool: used to read constants from the database and write constants (to ASCII file for now)



*/
