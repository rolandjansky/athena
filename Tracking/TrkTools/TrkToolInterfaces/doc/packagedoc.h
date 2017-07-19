/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkToolInterfaces_page TrkToolInterfaces Package

This package contains abstract interfaces for common tracking tools,
in particular those under the TrkTools container package.
Concrete implementations can be found in each leaf package.
Interfaces for specific tasks (such as extrapolation, fitting,
detector description) are grouped under their associated cvs containers
instead of this package.

@author Common Tracking Software Group

@section TrkToolInterfaces_TrkToolInterfacesList List of Defined Interfaces

Abstract interfaces for the following tracking tasks are defined:

@htmlinclude annotated.html

   - Trk::IAmbiTrackSelectionTool : forms new tracks using information about shared and already associated hits
   - Trk::ICompetingRIOsOnTrackTool : creates Trk::CompetingRIOsOnTrack objects from a list of mutually exclusive PrepRawData and a track hypothesis
   - Trk::IDetailedTrackTruthBuilder : creates full truth information at track level
   - Trk::IDetailedTrackTruthSimilarity : Interface for track-truth match quality estimator tools
   - Trk::IPatternParametersUpdator : Interface for updating Trk::PatternTrackParameters with measurements
   - Trk::IPRD_AssociationTool : manages lists of hits and their association to reconstructed tracks
   - Trk::IResidualPullCalculator : calculates residuals and pulls between a track state and a measurement
   - Trk::IRIO_OnTrackCreator : performs additional hit calibration at pattern reco level and transforms PRDs to Trk::RIO_OnTrack objects
   - Trk::IRIO_OnTrackErrorScalingTool : applies an error inflation function during the ROT creator process
   - Trk::ISegmentConverterTool : converts Trk::Segment objects to Trk::Track.
   - Trk::ITrackAmbiguityProcessorTool : solves hit assoication ambiguities in a given list of tracks
   - Trk::ITrackHoleSearchTool : searches, counts and adds holes (missed sensitive detectors) on the track
   - Trk::ITrackMagneticFieldIntegrator : calculates field integral of track, separated in indet and ms parts
   - Trk::ITrackParticleCreatorTool : tool to generate the AOD representation of Tracks, Rec::TrackParticle
   - Trk::ITrackScoringTool : scores tracks according to their properties, giving a sophisticated track quality criterium
   - Trk::ITrackSelectorTool : judges the track quality with respect to a given analysis/reconstruction procedure
   - Trk::ITrackSlimmingTool : strips a Trk::Track down to the bare information needed for storage or refit
   - Trk::ITrackSummaryTool : condenses Trk::Track properties and associated hits to a (non-fittable) foot print, the Trk::TrackSummary object
   - Trk::ITrackSummaryHelperTool : sub-structure of the above tool to allow detetector-specific access to information
   - Trk::ITrackTimingTool : interface for having time-space points calculated from various subdetector time measurements on a track
   - Trk::ITRT_ElectronPidTool : performs electron identification on given (Indet) track
   - Trk::IPixelToTPIDTool : performs particle identification using pixel charge information
   - Trk::ITruthToTrack : converts MC-generator particle parameters into EDM representation for validation studies
   - Trk::ITruthTrajectoryBuilder : collects MC-generator information to build full (truth) trajectory across the detector
   - Trk::IUpdator : collection of methods operating on track states, mainly for Kalman filtering
   - Trk::IWeightCalculator : encapsulates the calculations of assignmentProbabilities of competing measurements to a track

This package is not built as any library, it is a simple include package.   

@section TrkToolInterfaces_ExtrasTrkToolInterfaces Extra Pages

*/
