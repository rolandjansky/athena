/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage The TrkParticleBase Package 

@author Andreas.Wildauer@cern.ch

@section introductionTrkParticleBase Introduction
This package contains the definition of the TrackParticleBase class which defines the tracking related part of the TrackParticle that
inherits from the TrackParticleBase class. All tracking oriented reconstruction software should operate on this class rather then
the TrackParticle because the latter introduces a dependence of the Tracking realm on the Reconstruction realm.

@section designTrkParticleBase Design
The Trk::TrackParticleBase class holds slimmed down information of the Trk::Track object. It holds a Trk::TrackSummary object with hits on track information and a Trk::FitQuality
object with chi2 and ndf information of the track fit. In addition it holds a vector of Trk::ParametersBase which is the top level
object for all charged and neutral representations inherit. One of the entries are the so called definig parameters, i.e. the parameters
that define the kinematics of the particle. This is mostly (in the ID) a Trk::MeasuredPerigee but could also be a Trk::MeasuredNeutralPerigee
or anything that inherits from Trk::ParametersBase. Hence the Trk::TrackParticleBase object can represent a neutral particle
(in contrary to the Trk::Track which is always charged). Last but not least it holds an ElementLink to the original Trk::Track
and to the reconstructed Trk::VxCandidate object.
    
@htmlinclude used_packages.html

@include requirements

*/
