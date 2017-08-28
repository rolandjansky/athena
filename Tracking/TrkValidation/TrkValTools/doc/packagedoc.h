/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkValTools_page The TrkValTools package
General validation tools for common tracking.

@author Andreas.Salzburger -at- cern.ch, Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch

@section TrkValTools_introductionTrkValTools Introduction

This package contains tools to validate track reconstruction on a common Inner Detector -
Muons code basis. It includes tools to fill validation ntuples e.g. with track data.
The scheme for filling the ntuple is based on track-wise entries. Every instance of
the steering tool (Trk::BasicValidationNtupleTool) can fill an ntuple tree of its own
(also in the same ntuple file). This allows to produce one tree e.g. with track data
from pattern recognition or simplified simulation and one tree with refitted tracks
to compare them with ROOT afterwards.

The ntuples created with the tools of this package can also be used by the stand-alone ROOT applications distributed with this package, namely the <nop>TrackPlotter and <nop>ResidualPulls. The <nop>TrackPlotter allows to plot the track and measurement positions
in the InnerDetector to check the performance of track fitters "by hand".
<nop>ResidualPulls analyses local position residuals and pulls at each measurement
surface, providing a general tool to validate the measurement error calibration
and material effect corrections.

@section TrkValTools_valNtupleSchemeTrkValTools Validation ntuple scheme
The main steering of the ntuple entries is performed by the Trk::BasicValidationNtupleTool, which can book the ntuple, create the tree directory and add basic entries. This tool is also able to write the current record to the ntuple file and start a new entry. One can also use an existing ntuple tree (which has to have the same track-wise structure) and just use the Trk::BasicValidationNtupleTool to add some ntuple variables.

Every entry (track) contains the event number, a track id (numbering the tracks within one event) and an optional iteration index, which can be used if different intermediate results of a track are produced in the refit procedure (see Trk::DeterministicAnnealingFilter for an example). The track id is the same for all tracks generated within one iteration process and reset to 0 every time a new event is started. Perigee parameters and @f$ \chi^2 @f$ are filled for every track if available.

For fitter validation especially information about the measurements on the track and the related track states are needed. Therefore the total number of measurements on the track and the number of measurements within a certain sub-detector type are stored. The tool writes array variables with columns for every measurement into the ntuple.

One can give a list of Trk::IValidationNtupleHelperTool for each detector type by joboptions which are called by the Trk::BasicValidationNtupleTool if a measurement of the related detector is found on the track. Those Trk::IValidationNtupleHelperTool can add items to the ntuple and fill special data needed for validation. The Trk::IValidationNtupleHelperTool may also reside in a detector-specific namespace (e.g. InDetFitterValidation) to be able to add information which need a dynamic cast of the Trk::MeasurementBase to the implementations.

Each detector-specific helper tool list may contain several tools and some tools may even be contained in more than one list, if they fill data for different detector types. There exists also a general list, which tools are called for every type of measurement.

@image html ValidationNtupleToolScheme.png

@section TrkValTools_TrkValToolsOverview Class Overview
The TrkValTools package contains the following classes:

 - Trk::DirectTrackNtupleWriterTool : Tool to write an ntuple record about a track on the fly to a tree, using the same set of Trk::IValidationNtupleHelperTool s like the Trk::TrackValidationNtupleWriter has.
 - Trk::PerigeeParametersNtupleTool : Tool to fill a track/trackparticle's parameters and error matrix at the origin, includes possibility to write corresponding truth parameters and pulls.
 - Trk::TrackInformationNtupleTool : Tool to fill a track/trackparticle's basic information
 - Trk::MeasurementVectorNtupleTool : Tool to fill a track/trackparticle's hit pattern and parameter trajectory.
 - Trk::TrackSummaryNtupleTool : Tool to fill a track/trackparticle's track summary information, including particle ID.
 - Trk::ResidualValidationNtupleHelper : Helper tool for Trk::BasicValidationNtupleTool
   which writes residual and pull information for Pixels, SCT and TRT using the
   Trk::ResidualPullCalculator.
 - Trk::HitPositionNtupleHelper : Helper tool for Trk::BasicValidationNtupleTool 
   which writes statistics about measurement and surface positions.
 - Trk::TrackPositionNtupleHelper : Helper tool for Trk::BasicValidationNtupleTool 
   which writes info about track positions.
 - Trk::BremPositionNtupleHelper : Helper tool for Trk::BasicValidationNtupleTool 
   which writes additional info about track states with brem-like 
   material effects.
 - Trk::DAF_ValidationNtupleHelper : Helper tool for Trk::BasicValidationNtupleTool 
   which writes data from Trk::CompetingRIOsOnTrack.
 - Trk::TruthNtupleTool : Tool to fill basic information about the truth record.
        It is called directly by the ntuple writer but provides information also
        for particles which have failed to be reconstructed. Furthermore it manages
        indices to link to ntuple entries of the associated reconstructed particles.
 - Trk::JetTruthNtupleTool : Tool to fill basic information about truth jets.
        It has link indices to the truth ntuple tree.
 - Trk::PrimaryTruthClassifier : modular concept to define certain categories of
        truth particles, this one labels primary, secondary and truncated tracks.
 - Trk::InDetReconstructableSelector : modular concept of selecting truth particles
        for the truth tree and efficiency calculation. This one selects generated
        particles which leave a detectable trace in the Inner Detector.
 - Trk::GenParticleJetFinder : track-based Durham jet finder, this one acting on truth
        particles.
 - Trk::TrackDiff : Tool to compare two tracks (e.g. simulated and reconstructed
        track). Differences in the Trk::TrackStateOnSurface are listed in the
        messages and a summary for each track is written to a ntuple.

@section TrkValTools_HowToTrkValTools How to use the tools
The most convenient way to use the validation ntuple tools is to run the Trk::TrackValidationNtupleWriter algorithm from the TrkValAlgs package, which retrieves a track collection from StoreGate and loops over all contained tracks. It needs a configured instance of the Trk::BasicValidationNtupleTool.

You can also run the Trk::BasicValidationNtupleTool directly by calling it from your own algorithm or tool to write data of complete tracks or add data of measurements successively. An example for this use case is the Trk::DeterministicAnnealingFilter.

For all purposes the python Configurables should be used, since the validation ntuple is
aimed at a broad variety of applications and would easily not allow meaningful default
configuration.

@subsection BasicValidationNtupleToolJobOptions jobOptions of the Trk::BasicValidationNtupleTool
<DL>
  <DT>BookNewNtuple
  <DD>shall a new ntuple directory be created? If set to <tt>False</tt> no event data (event no. etc.) will be written and the calling Alg takes care of the Tree. Default: <tt>False</tt>
  
  <DT>NtupleFileName
  <DD>filename handler of the ntuple, only applicable if BookNewNtupl is <tt>True</tt>. Default: <tt>/NTUPLES/FILE1</tt>
  
  <DT>NtupleDirectoryName
  <DD>directory to be created or used in the ntuple file, only applicable if BookNewNtupl is <tt>Tru\
e</tt>. Default: <tt>FitterValidation</tt>
  
  <DT>NtupleTreeName
  <DD>ntuple tree, only applicable if BookNewNtupl is <tt>Tru\
e</tt>. Default: <tt>Validation</tt>
  
  <DT>UseROTwithMaxAssgnProb
  <DD>in the case of Trk::CompetingRIOsOnTrack: shall the included RIO_OnTrack with the maximum assignment probability be used for calculations? Default: <tt>True</tt>
  
  <DT>IgnoreMissingTrackCovarianceForPulls
  <DD>shall the pull be calculated when no covariance matrix for the track state is available? Default: <tt>False</tt>
  
  <DT>UpdatorTool
  <DD>tool and instance name of the IUpdator tool to be used to get unbiased track states. If no tool is given (<tt>none</tt>) the normal track states will be used (and residuals and pulls might be too small). Default: <tt>Trk::KalmanUpdator/TrkKalmanUpdator</tt>
  
  <DT>ExtrapolatorTool
  <DD>tool and instance name of the IExtrapolator, aimed at getting Perigee parameters for tracks which do not have it. Default: <tt>Trk::Extrapolator/InDetExtrapolator</tt>

  <DT>HoleSearchTool
  <DD>tool and instance name of the ITrackHoleSearchTool for analysis of track holes and assignment efficiencies. Default: <tt>InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool</tt>

  <DT>DoHoleSearch
  <DD>flag toggling the search for holes and filling of associated variables. Default: true

  <DT>PixelNtupleHelperTools
  <DD>list of Trk::IValidationNtupleHelperTool to be called if measurement is identified to be in the Pixels. Default: empty
  
  <DT>SCTNtupleHelperTools
  <DD>list of Trk::IValidationNtupleHelperTool to be called if measurement is identified to be in the SCT. Default: empty
  
  <DT>TRTNtupleHelperTools
  <DD>list of Trk::IValidationNtupleHelperTool to be called if measurement is identified to be in the TRT. Default: empty
  
  <DT>MDTNtupleHelperTools
  <DD>list of Trk::IValidationNtupleHelperTool to be called if measurement is identified to be in the MDT. Default: empty
  
  <DT>CSCNtupleHelperTools
  <DD>list of Trk::IValidationNtupleHelperTool to be called if measurement is identified to be in the CSC. Default: empty
  
  <DT>RPCNtupleHelperTools
  <DD>list of Trk::IValidationNtupleHelperTool to be called if measurement is identified to be in the RPC. Default: empty
  
  <DT>TGCNtupleHelperTools
  <DD>list of Trk::IValidationNtupleHelperTool to be called if measurement is identified to be in the TGC. Default: empty
  
  <DT>GeneralNtupleHelperTools
  <DD>list of Trk::IValidationNtupleHelperTool to be called for every measurement independent from detector technology. Default: empty

  <DT>DoTruth
  <DD>flag to toggle truth access, allowing validation of tracks on real data with the same
      ntuple framework. Default: true
</DL>

@subsection ResidualValidationNtupleHelperJobOptions jobOptions of the Trk::ResidualValidationNtupleHelper
<DL>  
  <DT>IgnoreMissingTrackCovariance
  <DD>shall the pull be calculated when no covariance matrix for the track state is available? Default: <tt>False</tt>
</DL>
@subsection DAF_ValidationNtupleHelperJobOptions jobOptions of the Trk::DAF_ValidationNtupleHelper
<DL>  
  <DT>IgnoreMissingTrackCovarianceForPulls
  <DD>shall the pull be calculated when no covariance matrix for the track state is available? Default: <tt>False</tt>
</DL>
@subsection TrackDiffJobOptions jobOptions of Trk::TrackDiff
<DL>
  <DT>NtupleFileName
  <DD>filename handler of the ntuple. Default: <tt>/NTUPLES</tt>
  
  <DT>NtupleDirectoryName
  <DD>directory to be created or used in the ntuple file. Default: <tt>TrackValidation</tt>
  
  <DT>NtupleTreeName
  <DD>ntuple tree. Default: <tt>TrackDiff</tt>
  
  <DT>CompareOutliers
  <DD>Shall Trk::TrackStateOnSurface of type Outlier be used in the comparison? Default: <tt>True</tt>
  
  <DT>CompareAllTSoS
  <DD>Shall Trk::TrackStateOnSurface of all types be used in the comparison?  Default: <tt>False</tt> <br>
      If CompareOutliers and CompareAllTSoS are both set to <tt>False</tt>
      only Trk::TrackStateOnSurface which are marked as Measurement are used.
  
  <DT>WriteNtuple
  <DD>Shall data be written into a ntuple?  Default: <tt>True</tt>
  
  <DT>WriteCompetingROTdata
  <DD>Shall data about Trk::TrkCompetingRIOsOnTrack be written?  Default: <tt>False</tt>
</DL>

@section TrkValTools_WhichVariablesTrkValTools List of the Ntuple variables
This paragraph gives a short overview of the ntuple variables provided by the different tools
@subsection BasicValidationNtupleToolVariables Ntuple variables of the Trk::BasicValidationNtupleTool
<table>
<tr><td>RunNumber</td> <td>run number the track belongs to</td> <td><tt>long</tt></td></tr>
<tr><td>EventNumber</td> <td>event number the track belongs to</td> <td><tt>long</tt></td></tr>
<tr><td>TrackID</td> <td>number of the track within the current event</td> <td><tt>long</tt></td></tr>
<tr><td>IterationIndex</td> <td>iteration index of the track</td> <td><tt>long</tt></td></tr>
<tr><td>nHits</td> <td>number of measurements on the track (including outliers)</td> <td><tt>long</tt></td></tr>
<tr><td>RecD0</td> <td>@f$ d_0 @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecZ0</td> <td>@f$ z_0 @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecPhi0</td> <td>@f$ \varphi_0 @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecTheta</td> <td>@f$ \theta_0 @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecEta</td> <td>@f$ \eta @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecQoverP</td> <td>@f$ \frac{q}{p} @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecErrD0</td> <td>@f$ \sigma_{d_0} @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecErrZ0</td> <td>@f$ \sigma_{z_0} @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecErrPhi0</td> <td>@f$ \sigma_{\varphi_0} @f$ of the Perigee</td> <td><tt>float</tt></td></tr>
<tr><td>RecErrTheta</td> <td>@f$ \sigma_{\theta_0} @f$ of the Perigee</td><td><tt>float</tt></td></tr>
<tr><td>RecErrQoverP</td> <td>@f$ \sigma_{\frac{q}{p}} @f$ of the Perigee</td><td><tt>float</tt></td></tr>
<tr><td>Chi2overNdof</td> <td>@f$ \frac{chi^2}{n_{DoF}} @f$</td> <td><tt>float</tt></td></tr>
<tr><td>Ndof</td> <td>@f$ n_{DoF} @f$</td> <td><tt>float</tt></td></tr>
<tr><td>pullLocX[nHits]</td> <td>pull in Trk::locX (pay attention: this might be wrong for some detector types),<br>value is set to <tt>-1000.</tt> for outliers as they usually contain no track parameters</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>pullLocY[nHits]</td> <td>pull in Trk::locY (pay attention: this might be wrong for some detector types),<br>value is set to <tt>-1000.</tt> if no second local coordinate exists for the measurement</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>residualLocX[nHits]</td> <td>residual in Trk::locX (pay attention: this might be wrong for some detector types),<br>value is set to <tt>-1000.</tt> for outliers</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>residualLocY[nHits]</td> <td>residual in Trk::locX (pay attention: this might be wrong for some detector types),<br>value is set to <tt>-1000.</tt> if no second local coordinate exists for the measurement and for outliers</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>DetectorType[nHits]</td> <td>enum of the detector type</td> <td><tt>Array&lt;long&gt;</tt></td></tr>
<tr><td>outlierFlag[nHits]</td> <td>flag is set to 1, if the measurement was marked as an outlier</td> <td><tt>Array&lt;long&gt;</tt></td></tr>
<tr><td>nPixelHits</td> <td>number of measurements in the Pixels</td> <td><tt>long</tt></td></tr>
<tr><td>nSCTHits</td> <td>number of measurements in the SCT (without outliers)</td> <td><tt>long</tt></td></tr>
<tr><td>nTRTHits</td> <td>number of measurements in the TRT (without outliers)</td> <td><tt>long</tt></td></tr>
<tr><td>nMDTHits</td> <td>number of measurements in the MDT (without outliers)</td> <td><tt>long</tt></td></tr>
<tr><td>nCSCHits</td> <td>number of measurements in the SCS (without outliers)</td> <td><tt>long</tt></td></tr>
<tr><td>nRPCHits</td> <td>number of measurements in the RPC (without outliers)</td> <td><tt>long</tt></td></tr>
<tr><td>nTGCHits</td> <td>number of measurements in the TGC (without outliers)</td> <td><tt>long</tt></td></tr>
</table>

@subsection ResidualValidationNtupleHelperVariables Ntuple variables of the Trk::ResidualValidationNtupleHelper
<table>
<tr><td>PixPullPhi[nPixelHits]</td> <td>pull in @f$ \varphi @f$ for Pixel measurements</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>PixPullEta[nPixelHits]</td> <td>pull in @f$ \eta @f$ for Pixel measurements</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>PixResPhi[nPixelHits]</td> <td>residual in @f$ \varphi @f$ for Pixel measurements</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>PixResEta[nPixelHits]</td> <td>residual in @f$ \eta @f$ for Pixel measurements</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>SCTPull[nSCTHits]</td> <td>pull for SCT measurements (perpendicular to strip direction, therefore just one-dimensional)</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>SCTRes[nSCTHits]</td> <td>residual for SCT measurements (perpendicular to strip direction)</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>TRTPull[nTRTHits]</td> <td>pull for TRT measurements</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>TRTRes[nTRTHits]</td> <td>residual for TRT measurements</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
</table>
These variables contain only measurements which are not marked as outliers.
<!-- If you need the residuals for outliers as well, you can use the general residual
variables (<tt>residualLocX[nHits]</tt>) and do a cut on the <tt>outlierFlag[nHits]</tt>
and on the <tt>DetectorType[nHits]</tt>, e.g. <tt>tree->Draw("residualLocX","DetectorType==3 && outlierFlag==1")</tt>
to plot the residual distribution for all outliers in the TRT. But: In the current setup
of newTracking outliers usually contain no track parameters so the residuals can not be calculated!
-->

@subsection HitPositionNtupleHelperVariables Ntuple variables of the Trk::HitPositionNtupleHelper
<table>
<tr><td>HitX[nHits]</td> <td>x coordinate of GlobalPosition of the measurement</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitY[nHits]</td> <td>y coordinate of GlobalPosition of the measurement</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitZ[nHits]</td> <td>z coordinate of GlobalPosition of the measurement</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitRadius[nHits]</td> <td>perp() of GlobalPosition of the measurement</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitPhi[nHits]</td> <td>phi() of GlobalPosition of the measurement</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitLocal1[nHits]</td> <td>Trk::locX of the local position of the measurement</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitLocal2[nHits]</td> <td>Trk::locY of the local position of the measurement,<br>value is set to <tt>-1000.</tt> if no second local coordinate exists for the measurement</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitSurfaceX[nHits]</td> <td>x coordinate (GlobalPosition) of the center of the measurement surface</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitSurfaceY[nHits]</td> <td>y coordinate (GlobalPosition) of the center of the measurement surface</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitSurfaceZ[nHits]</td> <td>z coordinate (GlobalPosition) of the center of the measurement surface</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitSurfaceRadius[nHits]</td> <td>perp() of the center of the measurement surface</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitSurfacePhi[nHits]</td> <td>phi() of the center of the measurement surface</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>HitSurfaceTheta[nHits]</td> <td>theta() of the center of the measurement surface</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
</table>
  
@subsection DAF_ValidationNtupleHelperVariables Ntuple variables of the Trk::DAF_ValidationNtupleHelper
<table>
<tr><td>CompROTnContainedROTs[nHits]</td> <td>number of contained ROTs in the Trk::CompetingRIOsOnTrack</td> <td><tt>Array&lt;long&gt;</tt></td></tr>
<tr><td>CompROTindexOfMaxAssgnProb[nHits]</td> <td>index of the ROT with maximum assignment probability in the Trk::CompetingRIOsOnTrack</td> <td><tt>Array&lt;long&gt;</tt></td></tr>
<tr><td>CompROTmaxAssgnProb[nHits]</td> <td>maximum assignment probability in the Trk::CompetingRIOsOnTrack</td> <td><tt>Array&lt;float&gt;</tt></td></tr>
<tr><td>CompROTcontainedROTsAssgnProbs[nHits][maxContainedROTs]</td> <td>assignment probabilities of the contained ROTs in the Trk::CompetingRIOsOnTrack</td> <td><tt>Matrix&lt;long&gt;</tt></td></tr>
<tr><td>CompROTcontainedROTsResLocX[nHits][maxContainedROTs]</td> <td>residuals in Trk::locX of the contained ROTs in the Trk::CompetingRIOsOnTrack</td> <td><tt>Matrix&lt;long&gt;</tt></td></tr>
<tr><td>CompROTcontainedROTsResLocY[nHits][maxContainedROTs]</td> <td>residuals in Trk::locY of the contained ROTs in the Trk::CompetingRIOsOnTrack,<br>value is set to <tt>-1000.</tt> if no second local coordinate exists for the measurement</td> <td><tt>Matrix&lt;long&gt;</tt></td></tr>
<tr><td>CompROTcontainedROTsPullLocX[nHits][maxContainedROTs]</td> <td>pulls in Trk::locX of the contained ROTs in the Trk::CompetingRIOsOnTrack</td> <td><tt>Matrix&lt;long&gt;</tt></td></tr>
<tr><td>CompROTcontainedROTsPullLocY[nHits][maxContainedROTs]</td> <td>pulls in Trk::locY of the contained ROTs in the Trk::CompetingRIOsOnTrack,<br>value is set to <tt>-1000.</tt> if no second local coordinate exists for the measurement</td> <td><tt>Matrix&lt;long&gt;</tt></td></tr>
</table>
All entries are zero for an index, if the measurement was not a Trk::CompetingRIOsOnTrack.

@subsection TrackDiffVariables Ntuple variables of Trk::TrackDiff
<table>
<tr><td>EventNumber</td> <td>Number of the event the tracks belong to</td> <td><tt>int</tt></td></tr>
<tr><td>RefTrackPhi0</td> <td>Phi0 of the reference track</td> <td><tt>float</tt></td></tr>
<tr><td>RefTrackEta0</td> <td>Eta0 of the reference track</td> <td><tt>float</tt></td></tr>
<tr><td>nRefStates</td> <td>number of reference TSoS for each detector type</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nCompareStates</td> <td>number of TSoS of the compared track (for each detector type)</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nFakeStates</td> <td>number of TSoS of the compared track which are not included in the reference track (does not include wrong types), (for each detector type)</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nMissedStates</td> <td>number of TSoS of the reference track which are not included in the compared track (does not include wrong types, (for each detector type))</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nWrongType</td> <td>number of TSoS which have different types in the reference and the compared track (for each detector type)</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nPRD_Mismatches</td> <td>number of TSoS on the same detector surface which have different Trk::PrepRawData pointers (for each detector type)</td> <td><tt>int[detTypes]</tt></td></tr>
<tr><td>nDriftCircleSignFlips</td> <td>number of Trk::RIO_OnTrack in the TRT which have flipped drift radii signs</td> <td><tt>int</tt></td></tr>
<tr><td>nRefTypes</td> <td>number of reference TSoS for each TSoS type</td> <td><tt>int[tsosTypes]</tt></td></tr>
<tr><td>nCompareTypes</td> <td>number of compared TSoS for each TSoS type</td> <td><tt>int[tsosTypes]</tt></td></tr>
<tr><td>nMissedTypes</td> <td>number of TSoS which are not on the compared track (for each TSoS type)</td> <td><tt>int[tsosTypes]</tt></td></tr>
<tr><td>nFakeTypes</td> <td>number of TSoS which are not on the reference track (for each TSoS type)</td> <td><tt>int[tsosTypes]</tt></td></tr>
<tr><td>nDiffs</td> <td>number of differences found</td> <td><tt>int</tt></td></tr>
<tr><td>DetectorType</td> <td>detector type in which the difference was found</td> <td><tt>vector <int>[nDiffs]</tt></td></tr>
<tr><td>IsFake</td> <td>is the difference a fake state?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>IsMissing</td> <td>is the difference a missing state?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>IsPRD_Mismatch</td> <td>is the diff as PRD mismatch?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>IsFlippedSign</td> <td>has the diff flipped sign?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>IsNoDriftTime</td> <td>has the diff !NoDriftTime?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>RefIsMeasurement</td> <td>is reference state a measurement?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>RefIsOutlier</td> <td>is reference state an outlier?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>ComparedIsMeasurement</td> <td>is compared state a measurement?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>ComparedIsOutlier</td> <td>is compared state an outlier?</td> <td><tt>vector<int> [nDiffs]</tt></td></tr>
<tr><td>MaxAssgnProb</td> <td>just if WriteCompetingROTdata=True: maximum assignment probability of the compared state</td> <td><tt>vector<float> [nDiffs]</tt></td></tr>
<tr><td>SumAssgnProb</td> <td>just if WriteCompetingROTdata=True: sum of assignment probabilities of the compared state</td> <td><tt>vector<float> [nDiffs]</tt></td></tr>
</table>
All numbers contain just the Trk::TrackStateOnSurface which have the types selected
in the jobOptions, i.e. in the standard configuration only Measurements and Outliers.

@section TrkValTools_TrackPlotterTrkValTools TrackPlotter
Stand-alone ROOT application to plot tracks. The code can be found in the subdirectory
<tt>root/TrackPlotter</tt> of this package; it has to be compiled separately (see below).
This application runs on the ValidationNtuple written by the
Trk::BasicValidationNtupleTool (from Tracking/TrkValidation/TrkValTools)
resp. Trk::TrackValidationNtupleWriter (from Tracking/TrkValidation/TrkValAlgs).

compile this tool by:
<tt>cd root/TrackPlotter</tt><br>
<tt>make trackplotter</tt>

to run:
<tt>./trackplotter</tt>
or 
<tt>./trackplotter TrkValidation.root</tt>

This tool needs a correctly set up ROOT installation.
If you get the message "cannot run in batch mode", please make sure
you have a valid X11 connection.

As ROOT versions and installations may differ on some machines, you may need to recompile
the TrackPlotter application, when getting segfaults on a certain machine. You should delete
the files <tt>trackplotterDict.*</tt> (which are recreated by the compiler) and call <tt>make clean</tt> first.

The TrackPlotter has a config file (<tt>trackplotter.conf</tt>) which allows to
set the ntuple directories and trees. Depending on your runtime setup you may have
to change the config file. In the config file you can also activate the plotting
of PrepRawData, but in this case you also have to run the related algorithms from
the package InDetTrackValidation to
fill the RIO data into the ntuple.
@image html TrackPlotterInDetXY.png
@image html TrackPlotterInDetLarge.png

@section TrkValTools_ExtrasTrkValTools Extra Pages

*/
