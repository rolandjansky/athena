/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetExtensionProcessor_page The InDetExtensionProcessor package

The InDetExtensionProcessor package provides algorithms which
form part of the Inner Detector track reconstruction sequence [1].

Currently there is one algorithm, InDet::InDetExtensionProcessor,
which is run at the end of the inside-out track finding to
evaluate by means of a full track fit if a proposed extension
of TRT hits matches with the associated silicon track.
Three possible situations are treated in this algorithm:
 - For a given Si track no extension is proposed. The original Si track is kept unmodified.
 - For a given Si track an extension is proposed, but the track fit failed or gives a worse score than the initial Si track. A new track object is then made with the additional TRT hits added as outlier states and the original Si track fit result.
 - For a given Si track an extension is proposed, and the additional hits have improved the track score. The new fit result is kept and a new Track made.

The algorithm is coded in a general way and can work with any combination
of original tracks and associated extensions given through a
Trk::TrackExtensionMap and with any type of track fitter.

@author Markus Elsing

@section InDetExtensionProcessor_BiblioInDetExtensionProcessor References
[1] T. Cornelissen et al., <i>Concepts, Design and Implementation of the ATLAS New Tracking</i>, ATLAS int.comm com-soft-2007_002<br>

@section InDetExtensionProcessor_JoboptionsInDetExtensionProcessor Job options for InDet::InDetExtensionProcessor
<DL>
  <DT>TrackName
  <DD>Name of the input Trackcollection in StoreGate.
 Default: <tt>Tracks</tt>

  <DT>ExtensionMap
  <DD>Name of the input extension map in StoreGate.
 Default: <tt>TrackExtensionMap</tt>

  <DT>NewTrackName
  <DD>Name of the output Trackcollection under which it shall be written to StoreGate.
 Default: <tt>ExtendedTrack</tt>

  <DT>TrackFitter
  <DD>Toolhandle for the track fitter.
 Default: <tt>Trk::KalmanFitter</tt>

  <DT>ScoringTool
  <DD>Toolhandle for the track scorer.
 Default: <tt>Trk::TrackScoringTool</tt>

  <DT>runOutlier
  <DD>switch whether to run outlier logics or not.
 Default: <tt>True</tt>

  <DT>RefitPrds
  <DD>switch whether to have the fit re-calibrated the clusters and drift circles
     (true) or not. Default: <tt>False</tt>

  <DT>matEffects
  <DD>particle hypothesis to assume for material effects in track fit.
  Default: <tt>4 (pion)</tt>

  <DT>keepFailedExtension
  <DD>switch whether to keep failed extension as outlier hits on the new track.
 Default: <tt>True</tt>

  <DT>etaBounds
  <DD>eta intervals for internal monitoring.
  Default: <tt>0.8, 1.6, 2.1</tt>
</DL>

@section InDetExtensionProcessor_ExtrasInDetExtensionProcessor Extra Pages

*/
