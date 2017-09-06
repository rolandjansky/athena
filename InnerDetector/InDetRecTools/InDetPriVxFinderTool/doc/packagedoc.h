/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetPriVxFinderTool_page InDetPriVxFinderTool

@section InDetPriVxFinderTool_introduction Introduction:
InDetPriVxFinderTool is a package which contains an AlgTool to calculate one or more primary vertices (in case of pile up events) of
an event. The package contains four different approaches to reconstruct primary vertices. InDetPriVxFinderTool.cxx makes use of the Billoir fitting AlgTools in the package TrkVertexBilloirTools. InDetAdaptivePriVxFinderTool.cxx and InDetAdaptiveMultiPriVxFinderTool.cxx make use of deterministic annealing methods.
All units are in mm and MeV.

@section InDetPriVxFinderTool_howitworks How it works:
<ul>
<li> InDetPriVxFinderTool.cxx </li>
It is a prototype usecase of the mathematical billoir fit AlgTools of the VxBilloirTools package. It is a typical <i>tear down</i> algorithm. It starts with a given collection of tracks, applies some quality cuts on tracks and feeds the fit AlgTool with all tracks which passed. The billoir tool stores the result in a VxCandidate object (see package Tracking/TrkEvent/VxVertex). After a first fit, InDetPriVxFinderTool.cxx loops again through tracks and selects them according to their \f$ \chi^{2} \f$ of the vertex fit. Tracks with too high \f$ \chi^{2} \f$ are rejected and the vertex is refittet. The result is stored in the VxContainer.<br>
<br>
There are two versions on how the algorithm handles the \f$ \chi^{2} \f$ cut:<br>
<ul>
<li>Version 1: (chi2CutMethod = 1)</li>
  <ul>
    <li>get rid of all tracks with \f$ \chi^{2} \f$ > m_maxChi2PerTrack in one go</li>
    <li>no refit after removing one track</li>
    <li>refit after all tracks with too high \f$ \chi^{2} \f$ were removed</li>
  </ul>
<li>Version 2: (chi2CutMethod = 2)</li>
  <ul>
    <li>get rid of tracks one by one starting with the one with highest \f$ \chi^{2} \f$ > m_maxChi2PerTrack</li>
    <li>refit after this track has been removed<br>
        and do a \f$ \chi^{2} \f$ cut again until all \f$ \chi^{2} \f$ < m_maxChi2PerTrack</li>
  </ul>
</ul>
The track quality cuts \f$ ( p_t, d_0, \frac{d_0}{\sigma_{d_0}}, z_0,  \chi^{2} ) \f$ can be set via jobOptions as well as the billoir method to be used (FullFit/FastFit).

<li> InDetAdaptivePriVxFinderTool.cxx </li>

This class provides an implementation for a primary 
vertex finding tool, which uses the Adaptive Vertex 
Fitter to reject outliers not belonging to the primary 
vertex interaction.

The steps done are simply;
- Tracks are selected according to the specified cuts
- The Adaptive Vertex Finder is used to fit them

Contrary to the InDetPriVxFinderTool, the outlier 
rejection is done by the fitter and not by the finder.

One only vertex can be fit, so it is not suited (as a 
finder) when many minimum bias vertices can be expected.
In this case please use the <i>InDetPriVxFinderTool</i>.


<li>InDetAdaptiveMultiPriVxFinderTool.cxx </li>

This class provides an implementation for a primary 
vertex finding tool, which uses the <i>Adaptive MultiVertex</i> 
Fitter to solve the problem of finding the multiple 
interaction vertices and to find out the best possible 
assignment of the track to the vertices.

The steps done are the following:
- the selection cuts are applied

then iteratively:
- a new vertex is seeded with the remaining tracks
   (the seed finder is used)
- all the tracks whose Z at PCA is closer to the seeded 
   vertex by more than TracksMaxZinterval (by JobOption), 
   are added to the new vertex candidate
- the new vertex candidate is added on top of the previous fit and 
   the AdaptiveMultiVertexFitter is used to fit all them 
   together (more information in the <br><i>TrkVertexFitters</br></i>
   package).
- the tracks already used are removed from the tracks 
   from which the next seed would be obtained and if there 
   are more than 2 left, a new iteration is started.

when no more than 2 seeding tracks are left:
- a vector of MVFVxCandidate is provided as result and 
  according to the selection type, the order in which it is 
  provided represents how high the probability of that 
  particular vertex to come from the primary vertex is. 

In general the first VxCandidate* in the collection is 
the one with highest sqrt(N_tracks)*Sum Pt_track^2. This 
is the case if the selectiontype in the jobOptions is set 
to 0 (default).

This finder is particularly suited for the high luminosities 
scenarios which will came up at LHC.
</ul>

@section InDetPriVxFinderTool_jobOptions The jobOptions file:
The AlgTool is automatically configured (see genConf). Following properties can be set:
<ul>
<li> InDetPriVxFinderTool.cxx </li>

  <ul>
  <li>Specifiy the name of the vertex fitter:<br>
  <b>'VertexFitterTool' : PublicToolHandle('Trk::FastVertexFitter') </b><br> </li>
  At the moment VxBilloirTools supports two fitting methods: FastFit and FullFit.
  <li>Specify the name of the track summary tool:<br>
  <b>'TrackSummaryTool' : PublicToolHandle('Trk::TrackSummaryTool') </b></li>
  <li>
  <b>VxPrimary.chi2CutMethod = 1;</b><br>
  See above for the two possibilities.<br>
  <b>'maxChi2PerTrack' : 5.0</b>
  </li>
  <li><b>VxPrimary.maxchi2perTrack = 5.;</b> // Maximum \f$ \chi^{2} \f$ of track to pass (applied after first vertex fit)</li>
  <li>Track parameter quality cuts:<br>
      <ul>
        <li><b>'minPt' : 1000.0 </b> // Minimal Pt(MeV)</li>
        <li><b>'maxZ0' : 150.0 </b> // Maximal z0(mm)</li>
        <li><b>'maxD0' : 0.25 </b> // Maximal d0(mm)</li>
        <li><b>'maxD0overSigmaD0' : 3.0 </b> // Maximal d0/sigmad0</li>
	<li><b>'useSiHitCut' : 1 </b> //Check if track has at least one b-layer hit and more than one pixel hit</li>
      </ul>
  </li>
  <li>Use a beam constraint: (bool)<br>
  <b>'useBeamConstraint' : False </b></li>
  <li> In case of pile up: <br>
  <b>'enableMultipleVertices' : 0 </b> <br>
  <b>'clusterLength' : 3.0, </b> </li>
  </ul>

<li> InDetAdaptivePriVxFinderTool.cxx </li>
<ul>
   <li><b>'minPt' : 500.0 </b> //Minimal Pt(MeV) </li>
   <li><b>'maxZ0' : 1000.0 </b> //Maximal z0(mm) </li>
   <li><b>'maxD0' : 1 </b> //Minimal d0(mm) </li>
   <li><b>'maxSigD0' : 0.35 </b> //max transv error  </li>
   <li><b>'maxSigZ0' : 2.5 </b> //max long error  </li>
   <li><b>'maxD0overSigmaD0' </b> //max transv IP significance </li> 
   <li><b>'useSiHitCut' : 1 </b> // Do cuts on number of hits in tracker </li>
   <li><b>'minBHits' : 0 </b> // Number of minimum B layer hits required </li>
   <li><b>'minPixelHits' : 1 </b>// Number of minimum SCT hits required </li>
   <li><b>'minSCTHits' : 5 </b>// Number of minimum SCT hits required </li>
   <li><b>'minPixelAndSCTHits' : 7 </b> // Number of minimum sum of pixel + SCT hits required </li>
   <li><b>'maxTrkChi2' : 3.5 </b> //Cut on the track chi2 from tracking algo for preselection </li>
</ul>

<li>InDetAdaptiveMultiPriVxFinderTool.cxx </li>
<ul>
   <li><b>'minPt' : 500.0 </b> //Minimal Pt(MeV) </li>
   <li><b>'maxZ0' : 1000.0 </b> //Maximal z0(mm) </li>
   <li><b>'maxD0' : 1 </b> //Minimal d0(mm) </li>
   <li><b>'maxSigD0' : 0.35 </b> //max transv error  </li>
   <li><b>'maxSigZ0' : 2.5 </b> //max long error  </li>
   <li><b>'maxD0overSigmaD0' </b> //max transv IP significance </li> 
   <li><b>'useSiHitCut' : 1 </b> // Do cuts on number of hits in tracker </li>
   <li><b>'minBHits' : 0 </b> // Number of minimum B layer hits required </li>
   <li><b>'minPixelHits' : 1 </b>// Number of minimum SCT hits required </li>
   <li><b>'minSCTHits' : 5 </b> // Number of minimum SCT hits required </li>
   <li><b>'minPixelAndSCTHits' : 7 </b> // Number of minimum sum of pixel + SCT hits required </li>
   <li><b>'maxTrkChi2' : 3.5 </b> //Cut on the track chi2 from tracking algo for preselection </li>
   <li><b>'TracksMaxZinterval' 4 </b> //Description:
      When adding a new vertex to the multi vertex fit,
      only the tracks whose Z at PCA is closer 
      to the seeded vertex by more than this TracksMaxZinterval 
      value are added to this new vertex.
      Default is 4 mm. If you cut too hard, you cut out 
      the good cases where the seed finder is not 
      reliable, but the fit would be still able to converge 
      towards the right vertex. If you cut too soft, you 
      consider a lot of tracks which just slow down the fit.
   </li> 
   <li><b>'maxVertexChi2' 10 </b> //Description:
      After having added one vertex to the fit and having 
      performed the MultiVertex fit again, all the tracks 
      which are compatible to the new vertex by more than 
      this maxVertexChi2 (in units of chi2) value are eliminated from the 
      tracks from which still to seed the next vertex.
   </li>
   <li><b>'cleaningZinterval' 2 </b> //Description:
      In case the fit gets stuck (a new seed found but after the full fit 
      of the new vertex, all the tracks are found not compatible with it), the 
      fit will find a seed for a new vertex only after having eliminated from 
      the game all tracks whose Z at PCA is nearer to the actual seeded vertex by 
      more then the cleaningZinterval (by JO), given in mm.
   </li>
   <li><b>'finalCutMaxVertexChi2' 10 </b> //Description:
      During the estimation of probability of vertex candidate to be the primary interaction 
      vertex, only all the tracks which have chi2 in the vertex fit higher than this value 
      are used for the sum of p_t^2 or as input for the Neural Network.
   </li>
   <li><b>'MinWeight' 0.001 </b> //Description:
      Final limit on track weight (between 0 and 1) in the fit to have a track 
      stored in the saved vertex candidate. 
      (not all people will give a look at the stored track weight in the fit...)      </ul>
   </li>
</ul>



\namespace Vtx
A namespace for all vertexing packages and related stuff.
*/
