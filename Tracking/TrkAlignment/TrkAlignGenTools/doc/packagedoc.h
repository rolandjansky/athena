/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkAlignGenTools_page TrkAlignGenTools
@author roberth@bu.edu
@author Daniel Kollar <daniel.kollar@cern.ch>

@section TrkAlignGenTools_TrkAlignGenToolsIntro Introduction

This package was copied directly from tools in InDetAlignTools and 
SiGlobalChi2AlignTools.  It contains some tools used to perform alignment 
of generic detector modules, and virtual interfaces to be inherited by 
detector-specific tools.

@section TrkAlignGenTools_TrkAlignGenToolsOverview Class Overview
  The package contains the following classes:
  - AlignModuleTool: inherits from IAlignModuleTool, used to handle the
    list of AlignModules (the alignment geometry) and provide functionality
    for basic operations/relations between AlignModules, AlignPars, etc. 
  - AlignTrackPreProcessor: inherits from IAlignTrackPreProcessor, used
    to create a collection of AlignTracks from a collection of Tracks,
    performs track selection and allows to refit the tracks
  - AlignTrackCreator: inherits from IAlignTrackCreator, used to process
    the Track and add hit information to AlignTrack (residuals, etc.)
  - AlignTrackDresser: inherits from IAlignTrackDresser, used to add extra
    information to AlignTrack, needed by the alignment global/local chi2
    algorithm, calls derivative calculator tool
  - AnalyticalDerivCalcTool: inherits from IDerivCalcTool, used to calculate
    derivatives of residuals wrt. alignment parameters needed for global/local
    chi2 algorithm in analytical way by using information from the track
    fitter
  - ShiftingDerivCalcTool: inherits from IDerivCalcTool, used to calculate
    derivatives of residuals wrt. alignment parameters needed for global/local
    chi2 algorithm by "shifting" the hit along the DoF and refitting the track
  - MatrixTool:  inherits from IMatrixTool, used to manipulate matrix and 
    vector entries and to solve for alignment parameters.
  - IPCMatrixTool:  inherits from IMatrixTool, used to manipulate matrix and 
    vector entries in binary format for parallel processing, and to solve for 
    alignment parameters.
  - TrkAlignDBTool:  contains tools for reading information needed for 
    performing alignment and for writing final alignment parameters to the 
    database 



*/
