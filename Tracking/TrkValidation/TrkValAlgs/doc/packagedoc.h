/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkValAlgs_page The TrkValAlgs package
General validation algorithms for common tracking.

@author Andreas.Salzburger@cern.ch, Sebastian.Fleischmann@cern.ch

@section TrkValAlgs_introductionTrkValAlgs Introduction

This package contains algorithms to validate the common tracking. This includes an algorithm to fill validation ntuples with track data. The algorithm is based on the tools in the TrkValTools package, see the documentation of this package for more information about the ntuple variables, etc.

@section TrkValAlgs_TrkValAlgsOverview Class Overview
  The TrkValAlgs package contains the following classes:

  - Trk::TrackValidationNtupleWriter : Alg for filling a validation ntuple with Trk::Track information using Trk::IValidationNtupleTool. It retrieves a track collection from StoreGate and loops over all contained tracks. 
  - Trk::SingleTrackDiffAlg : Alg for a simple track diff. The Trk::ITrackDiff tool is used to perform the comparison. This alg can be used
  for single track events, because only the first track of the reference collection will be compared with the best matching track of the comparison collection.
      
@section TrkValAlgs_HowToTrkValAlgs How to use the algorithms
The Trk::TrackValidationNtupleWriter needs a configured instance of the Trk::BasicValidationNtupleTool. This tool can be configured by the Python setup class <tt>ConfiguredBasicValidationNtupleTool</tt> which is included in the TrkValTools package. An example can be found in the <tt><a href="TrkTrackValidationNtupleWriter_8py-source.html">TrkTrackValidationNtupleWriter.py</a></tt> jobOption fragment.

@subsection TrackValidationNtupleWriterJobOptions jobOptions of the Trk::TrackValidationNtupleWriter
<DL>
  <DT>TrackCollection
  <DD>Name of the track collection. No default.
  
  <DT>ValidationNtupleTool
  <DD>Tool and instance name of the Trk::IValidationNtupleTool. Default: <tt>Trk::BasicValidationNtupleTool/BasicValidationNtupleTool</tt>
</DL>

@section TrkValAlgs_ExtrasTrkValAlgs Extra Pages

*/
