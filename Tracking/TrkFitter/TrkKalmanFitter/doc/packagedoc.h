/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkKalmanFitter_page 

<!-- =============================================================== -->
@section TrkKalmanFitter_TrkKalmanFitterDocIntroduction Introduction
<!-- =============================================================== -->

The TrkKalmanFitter package implements in ATLAS the Kalman filtering
technique for track fitting, as proposed by Fr&uuml;hwirth et al.
in 1987 [1].<br>
It provides a forward filtering method, a backward smoothing and
an outlier logic to improve the fit quality.

This Kalman Filter Track Fitter ("KalmanFitter") is implemented
under the common interface for track fitters, ITrackFitter, and
makes direct use of the Tracking Event Data Model classes internally.
In practice this means that the Kalman Fitter operates independently
of the different kinds of measurement along the trajectory,
while it can be used for a wide variety of tasks from segment fitting
up to large-scale fits over the whole ATLAS detector as well as
precision refits.
Common tracking tools are used to provide extrapolation, material
and magnetic field update as well as calibration corrections and thus
maintain the fitter's independence of detector-specific conditions.

Further extensions of the basic Kalman Fitter are available as options:
Dynamic Noise Adjustment for correct fitting of electrons with energy
loss and access to alignable surfaces during use as alignment Kalman
Filter.

<!-- =============================================================== -->
@section TrkKalmanFitter_TrkKalmanFitterDocPrinciple The Kalman Filter Track Fitting Technique
<!-- =============================================================== -->

As opposed to global chi2 fits, the Kalman Filtering technique uses
a progressive algorithm to obtain the optimal track parameters at
the end of the process. Figure 1 illustrates how a single filtering
step in this progressive algorithm works: From previous steps
or (here) from an initial measurement the local parameters
(position and momentum direction) of the track are constraint
on a surface A. Each Kalman filter step involves two actions,
extrapolation (also called prediction) and updating. The extrapolation
propagates the track parameters and their uncertainties to the next
surface, B, with a measured hit. The predicted local
position received uncertainties from both the previous position
measurement(s) and from the extrapolation uncertainty. Through this
mechanism the filter becomes sensitive to track direction and momentum
already after a few steps. In the figure the extrapolated uncertainty
is therefore indicated to be larger than the usual hit error.
The second part, the Kalman update, integrates the local position
uncertainty of Hit B into the prediction, resulting in an updated
prediction B.

<center>
@image html KalmanFilterStep.png
<i>Fig. 1: A single filtering step in the Kalman Fitter</i>
</center>

The detailed mathematical background is presented in the references
and can also be found in the code.

This filtering step can be executed both in the forward and
in the opposite direction of the trajectory. Material effects
are added (or subtracted in the case of backward filtering)
during the extrapolation process. For this purpose the
Trk::ITrackFitter interface supports a parameter for the particle
type hypothesis which is passed through to the extrapolation.
This parameter is Trk::pion by default. It can be set to
Trk::nonInteracting in order to switch any material effects
calculation off.
Other settings (Trk::muon, Trk::electron ...) control how
the uncertainties from multiple scattering and energy loss
are added to the predicted state.

The KalmanFitter way of doing the measurement update 
(i.e. adding a measurement to the predicted state) is
externalised in a separate task under the IUpdator interface.
The default implementation is provided by the TrkMeasurementUpdator
package. The Kalman filter's mathematical basis is also used by
other algorithms e.g. to obtain the fitquality (chi2/ndf) and to
remove a hit from a track parameterisation, which in turn is a quick
way of knowing the unbiased residual of a fitted state.

<!-- =============================================================== -->
@section TrkKalmanFitter_TrkKalmanFitterDocImplement Implementation as an ATLAS Tracking Tool
<!-- =============================================================== -->

@subsection TrkKalmanFitterDocImple Interface
The KalmanFitter is an implementation of the Trk::ITrackFitter
interface and implements all its methods, differing by the format
how the input is given, such as
<ol>
<li> a list of measurements given as Trk::PrepRawData</li>
<li> an existing track and a list of additional measurements
     provided as Trk::PrepRawData objects</li>
<li> a list of measurements given as Trk::MeasurementBase</li>
<li> an existing track and a list of additional measurements
     provided as Trk::MeasurementBase objects</li>
<li> the hits from an existing track (i.e. to do a refit)</li>
<li> two separate tracks for making a combined fit.</li>
</ol>
To avoid duplication of code, the case (2) is integrated in case (1),
as well as (4) is integrated in (3).

In order to modularise the iterative strategies and the Kalman filtering
process further, the code is split up in a ForwardKalmanfitter, a
KalmanSmoother and a KalmanOutlierLogic, which are AlgTools themselves
and implement the appropriate interfaces from TrkFitterInterfaces.
They are further structured by separating the calculations from the
filtering logic, i.e. the Extrapolator and MeasurementUpdator tools
provide the predicted and updates states, respectively.

@subsection TrkKalmanFitterDocMeasurements Fitting of different kinds of measurement
The internal fitting algorithms work with the EDM base classes,
therefore the Kalman fitter can deal with hit measurements from
<i>any kind of ATLAS detector technology</i>, that is clusters
or drift circles from Pixel, SCT, TRT, MDT, TGC, RPC and CSC.
It is also capable of using <i>any kind of abstract measurement</i>
in the track fit, as derived classes from Trk::MeasurementBase
providing a sub-set or the full five Trk::LocalParameters as
measured co-ordinates. That currently includes, but is not limited to,
Trk::SpacePoint, Trk::Segment, Trk::VertexOnTrack,
Trk::PseudoMeasurementOnTrack.

@subsection TrkKalmanFitterDocStructure Code structure
<center>
@image html KalmanFitter_logic.png
<br>
<i>Fig. 2: Use diagram of the KalmanFitter modules. Each white box
symbolises an athena AlgTool. The KalmanFitter main tool
on the left implements the different interfaces defined by Trk::ITrackFitter.
Since they all lead to the same fitting logic, they are not distinguished in
this diagram. The interface class Trk::ITrackFitter as well as the common
tracking tools are of general use and therefore not provided by the
TrkKalmanFitter package.
</i></center>

Figure 2 gives an overview how the different tools use each other. The
fitting and re-fitting methods in the main KalmanFitter tool
subsequently call the Trk::ForwardKalmanFitter, the Trk::KalmanSmoother
and Trk::KalmanOutlierLogic. Since forward-fitter and smoother implement
the Kalman filtering process described above, they use the
Trk::IExtrapolator and the Trk::IUpdator. 
The Trk::IRIO_OnTrackCreator is only called when the input
set of measurements consists of Trk::PrepRawData objects, so that
the corresponding Trk::RIO_OnTrack objects are made on-the-fly and
additional calibrations performed using the extrapolated prediction.
Since track extrapolation and the calibration corrections
during the <nop>RIO_OnTrack creation are complex and general tasks
not restricted to Kalman filtering, they are external packages.
<br>
The Trk::KalmanOutlierLogic is toggled by an optional flag in the fitter
interface. If set and if the logic returns a failure due to a missed
measurement surface or a bad chi2, it flags the corresponding measurement
internally as outlier and excludes it from further filtering.
Those measurements are still stored on the output track (with 
TrackStateOnSurface type outlier) to allow further analysis.
The current outlier logic removes first those hits where the
prediction missed their measurement surface, then those with
a state chi2 value above a given limit and, if this is
not sufficient, the hit with the largest chi2 contribution.
After that the fit is re-run on the reduced
set up to a maximum number of trials or up to the moment when
the rank of the measurements are insufficient to constrain the
fit.
<br>
The ForwardKalmanFitter has an additional logic which flags a
measurement as outlier if it is expected to distort the filtering
massively. The criterium is the filtered chi2 and the cut value
steered by job option. 

<!-- =============================================================== -->
@section TrkKalmanFitter_TrkKalmanFitterDocUsage Using TrkKalmanFitter in your code
<!-- =============================================================== -->
To get the ITrackFitter interface, add to the requirements file
@htmlonly
<table width="80%"><tr>
<td style="background-color:#e8e0e8; color:#600000; size:+2;
           padding:10px;" >
<pre>use TrkFitterInterfaces   TrkFitterInterfaces-*    Tracking/TrkFitter
</pre></td></tr></table>
Do not do "use TrkKalmanFitter", an explicit compile-time dependency
is not needed. Since you retrieve the fitter as an AlgTool, you'd want
to have it e.g. as a private member in your class:
<table width="80%"><tr>
<td style="background-color:#e8e0e8; color:#600000; size:14;
           padding:10px;" >
<pre>#include "TrkFitterInterfaces/ITrackFitter.h"
...
ToolHandle<Trk::ITrackFitter> m_trackFitter;
</pre></td></tr></table>
in you class's constructor, the fitter handle should point to
one of the track fitter implementations, here KalmanFitter:
<table width="80%"><tr>
<td style="background-color:#e8e0e8; color:#600000; size:14;
           padding:10px;" >
<pre>m_trackFitter("Trk::KalmanFitter/KalmanFitter")
</pre></td></tr></table>
at the point of setting up, e.g. in the athena initialise() method,
retrieve the tool:
<table width="80%"><tr>
<td style="background-color:#e8e0e8; color:#600000; size:14;
           padding:10px;" >
<pre>
if (m_trackFitter.retrieve().isFailure()) {
    m_log << MSG::ERROR << "can not retrieve the track fitter tool "
          << m_trackFitter.typeAndName() << endmsg;
    return StatusCode::FAILURE;
  }
</pre></td></tr></table>
The call to the fitting method depends on your use case.
It is documented in the Trk::ITrackFitter interface. It could be,
  for example,
<table width="80%"><tr>
<td style="background-color:#e8e0e8; color:#600000; size:14;
           padding:10px;" >
<pre>Trk::Track* newtrack = m_trackFitter->fit(vector_of_rioontrack,
                                         *(oldtrack->trackParameters()->front()),
                                         m_runOutlier,m_matEffects);
</pre></td></tr></table>
or 
<table width="80%"><tr>
<td style="background-color:#e8e0e8; color:#600000; size:14;
           padding:10px;" >
<pre>newtrack = m_TrackFitter->fit(*oldtrack,m_runOutlier,m_matEffects);
</pre></td></tr></table>

Finally you need to make sure the fitter is configured to your needs,
so often it is preferrable to set up a python configurable instance
of the KalmanFitter which is passed onto the ToolHandle in your code.
<table width="80%"><tr>
<td style="background-color:#e8e0e8; color:#600000; size:14;
           padding:10px;" >
<pre>from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter \
    as ConfiguredKalmanFitter
MyTrackFitter = ConfiguredKalmanFitter(
    name                           = 'MyTrackFitter',
    ExtrapolatorHandle             = MyExtrapolator,
    RIO_OnTrackCreatorHandle       = MyRotCreator,
    MeasurementUpdatorHandle       = MyMeasurementUpdator,
    DynamicNoiseAdjustorHandle     = None,
    AlignableSurfaceProviderHandle = None)
</pre></td></tr></table>
In this example it is assumed that the <tt>MyExtrapolator</tt>,
<tt>MyRotCreator</tt> and <tt>MyMeasurementUpdator</tt> have been set
up as python classes in the prior parts of the reconstruction set-up.<br>
@endhtmlonly


<!-- =============================================================== -->
@section TrkKalmanFitter_TrkKalmanFitterDocSteering KalmanFitter Job Options
<!-- =============================================================== -->
@subsection TrkKalmanFitterDocSteer1 Tool Handles
The KalmanFitter and its extensions are configured and put together
from a set of tool-handles.
@htmlonly
<table cellspacing=10px><tr>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
ExtrapolatorHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
extrapolation tool for transporting track pars and handling material effects<br>
<pre>Trk::Extrapolator/AtlasExtrapolator</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
RIO_OnTrackCreatorHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tool to create <nop>RIO_OnTrack from <nop>PrepRawData.
<br><pre>Trk::RIO_OnTrackCreator/RIO_OnTrackCreator</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
MeasurementUpdatorHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tool to perform measurement update and chi2 calculation<br>
<pre>Trk::KalmanUpdator/KalmanUpdator</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
DynamicNoiseAdjustorHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tool to handle brem as dynamically adjusted q/p noise<br>
<pre>None</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
AlignableSurfaceProviderHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tool to replace measurement surface by an alignable one<br>
<pre>None</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
ForwardKalmanFitterHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tool for running the forward filter along the internal trajectory<br>
<pre>Trk::ForwardKalmanFitter/FKF</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
KalmanSmootherHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tool for performing the backward smoothing on the internal trajectory<br>
<pre>Trk::KalmanSmoother/BKS</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
KalmanOutlierLogicHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tool for fit quality analysis and outlier flagging<br>
<pre>Trk::KalmanOutlierLogic/KOL</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
FitterValidationToolHandle</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tool for fitter validation (writes intermediate results to ntuple)<br>
<pre>None</pre></td>
</tr></table>
@endhtmlonly

@subsection TrkKalmanFitterDocSteer2 Fitter Steering
In addition, the main KalmanFitter tool has several flags which
control its execution flow or input decoding and allow its flexible
re-use in different places of the offline reconstruction.

@htmlonly
<table cellspacing=10px><tr>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
DoHitSorting</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  Enables hit sorting along the reference direction when the KF starts.
  In any case the hits <b>need</b> to be sorted in ascending order on
  the trajectory either by the preceding pattern recognition or by the
  fitter itself.
  <pre>True</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
DoSmoothing</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  enables the backward filter to produce smoothed track parameters at
  every surface.<br>
default is <pre>TRUE</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
NoPrepRawDataInRefit</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  enables the track refit method to use the existing hit calibration.
  A 'false' will make the refit method dissolve a the RIO_OnTracks from
  an input Trk::Track back into Trk::PrepRawData and re-run the ROT
  creators. Keep on 'true' for non-standard measurements (Segments,
  CompetingROTs) and drift circles (left-right solution may go wrong).<br>
default is <pre>TRUE</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
SortingReferencePoint</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  If the fitter is supposed to take care of the hit sorting, this given
  reference point is taken to order the hits according to their
  distance from it. If doSorting is set to False, the refit options still
  uses this reference point to extract the initial parameters from the
  input track. With the current versions it should be warned that the
  sorting of hits does not work for some configurations of single coordinate
  hits, because the surface center (and not the track intersection at
  the wire/strip) is used as global position.
  For LHC collisions the reference point defaults to the primary vertex
  while for test beam or cosmics it might need to be set out of atlas to
  allow e.g. a downward order for cosmic muons.
<pre>[0., 0., 0.]</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
RecoverSeedFromFailedLogic</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  enables predictive outlier search for cases with problematic track seed.
  To be used with care since it is currently cpu-time expensive.<br>
default is <pre>TRUE</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
MaxNumberOfFitIterations</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
the maximum number of fit iterations (in case the fit quality does
not pass the cut without flagging hits as outliers and in case
outlier removal is enabled)<br>
<pre>3</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
CalculatePerigeeAtOrigin</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
enables fitter to make reference parameters as Perigee near origin,
otherwise (if set to false) the fit result will be expressed at
the surface associated to the input reference parameters.<br>
<pre>true</pre></td>

<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
ReintegrateOutliers</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
if enabled it resets all outlier flags and tries the fit using once
more all available hits. This flag only applies to those interface
methods (2, 4, 5 and 6 above) which use Trk::Track as one of the
input objects.<br>
<pre>false</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
DoValidationAction</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
if enabled validation methods in the extrapolation engine are called after
each iteration. Useful for debugging the navigation.<br>
<pre>false</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
CallValidationToolForFailedFitsOnly</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
if a fitter-validation tool is active under the FitterValidationToolHandle,
this flag toggles if the tool is used for every fitter call or only for failed
fits.<br>
<pre>false</pre></td>
</tr></table>
@endhtmlonly

@subsection TrkKalmanFitterDocSteer3 Fitter Tuning
Finally also the associated tool as part of the KalmanFitter,
ForwardKalmanFitter (FKF), KalmanSmoother (BKS) and KalmanOutlierLogic (KOL),
have job options mainly aimed at tuning the fit stability. The aim is to
address those options directly through the KalmanFitter's ToolHandles under
python but this has turned out not to work. Therefore, public instances
of those tools need to be defined first under python, so that defaults
can be changed.

@htmlonly
<table cellspacing=10px><tr>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
FKF.StateChi2PerNDFPreCut</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  Coarse pre-cut on the predicted state's chi2/ndf against outliers
  destabilising the filter. This removes outliers which could bend
  the fit away from the real trajectory. It should be warned, however,
  that this mechanism can not replace a proper pattern recognition strategy.<br>
<pre>50.0</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
BKS.InitialCovarianceSeedFactor</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  factor by which to scale filtered cov matrix as initial matrix for backward
  filter<br>
<pre>1000.0</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
BKS.RelativeInitialErrorLimit</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  limit by which a coordinate is deemed unconstraint and its error reset
  instead of scaled. Assuming a default of 10, if the error after all 
  forward-filtering is more than one 10th of the forward filter's start
  error value, this coordinate is declared not well constraint and intead
  of scaling it to a potentially instable value the backward filter re-set
  to the same start value as the forward filter had.<br>
<pre>10.0</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
KOL.TrackChi2PerNDFCut</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  cut on the smoothed track's chi2/ndf in the outlier logic.
  If the outlier logic parameter is set, hits will be removed from 
  tracks with a higher chi2/ndf than this value. The cut is translated
  into a chi2 probability.
<pre>10.0</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
KOL.StateChi2PerNDFCut</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  if hits are to be removed, this cut on each states' individual chi2
  contribution decides which states will actually be removed from
  the input set.<br>
<pre>12.5</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
KOL.SurfaceProximityCut</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
tolerance limit in sigmas beyond which a prediction is out of active-sensor
bounds<br>
<pre>5.0</pre></td>
<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
</tr><tr><td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
OutputLevel</td>
<td style="background-color:#a0a0ff; color:#000000;
           padding:10px;" >
  default options for AlgTool message level
</td>
</tr></table>

Those default settings for cut parameters originate mainly from Inner Detector
experience. The concept is, however, that the detector-specific job options
can always set their specific values through the python configurables classes.

@endhtmlonly

@section TrkKalmanFitter_TrkKalmanFitterDocStability Maintaining Fit Stability

@subsection TrkKalmanFitterDocStab1 Fitting at Trk::PrepRawData level
  The KalmanFitter is designed to be a mathematical fitting tool
  and therefore can not make a sophisticated local pattern recognition.
  For the case of DriftCircles, the EDM requests the drift radius
  left-right ambiguity to be solved during the process of making
  Trk::RIO_OnTrack objects. Track fitting on the level of Trk::PrepRawData
  is therefore not stable with the KalmanFitter:
  the large extrapolation distances in conjunction with high material
  densities lead to an uncertainty in the predicted state which
  is larger than what is needed for a correct left-right solution.
  This problem is particularly present in the Muon System,
  where the extrapolation lengths are large
  while the error of the measured MDT radius is very small.
  Since the Kalman filtering is single-way progressive, it usually
  can not recover from a wrongly chosen MDT sign solution.
  As a countermeasure the option to refit tracks defaults to
  use Trk::RIO_OnTrack, where the left-right ambiguity is
  expected to be solved.

@subsection TrkKalmanFitterDocStab2 Sorting of Hits
If the option DoHitSorting is not enabled, it is the client's 
responability to provide a vector which is ordered exactly
along the trajectory. The material effects calculation in
the extrapolation needs to know if the filter is moving
along the track's direction of flight or backward, therefore
hits in the wrong order are automatically rejected by the extrapolation.

To offer more flexibility the Kalman fitter (delegated to
Trk::TrackFitInputPreparator) can sort the hit vector.
For that purpose the momentum direction of the reference vector
is taken, the hit positions projected onto this vector, and
the projection length taken as ordering criterium. For stable
functioning, a few points need to be taken into account.

 - the reference parameters need to be close to the actual hits.
 - very-low-pT tracks may be sorted wrongly. In addition,
   for p < 500MeV a slower, but more stable sorting algorithm
   is used.
 - hits which lie at the same, or a numerically very similar surface
   (e.g. same drift tube with different helper frame) may be shuffled
   by accident, causing hits to be removed.

@section TrkKalmanFitter_TrkKalmanFitterDocContacts Contacts
  @author M. Elsing, W. Liebig <http://consult.cern.ch/xwho>
  Please reports bugs via Savannah, project "Atlas Tracking".<br>
  Also bear in mind that the KalmanFitter uses several other EDM classes
  and Tracking Tools, so that a problem may not necessarily
  be caused by methods from this package.
  

@section TrkKalmanFitter_TrkKalmanFitterDocReferences References
[1] R. Fr&uuml;hwirth et al, <i>Application of Kalman Filtering
    to Track and Vertex Fitting</i>, NIM A 262 (1987) 444.


@section TrkKalmanFitter_ExtrasTrkKalmanFitter Extra Pages

*/
