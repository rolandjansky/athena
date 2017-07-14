/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page Particle_page Particle

@author Andreas.Wildauer@cern.ch, Fredrik.Akesson@cern.ch

@section Particle_introduction Introduction:
Particle is a first version of a "more than a track" object. It provides the user
with a particle which has (or has not) a track, a vertex, an origin (type of vertex)
and a charge. It is meant as an interface between the reconstruction
world an physics. All algorithms after the particle creation (i.e. JetFinder, b-tagging, ...) should
run on TrackParticle(s) instead of Trk::Track(s) directly.

@section Particle_creation TrackParticle Creation
To create TrackParticles one should <b>not</b> use the TrackParticle constructors but the TrkParticleCreator tool which
is located in Tracking/TrkTools/TrkParticleCreator. The reason for that is that the tool always attempts to create
an as complete TrackParticle as possible. E.g. if your input track does not have a perigee, the tool will notice and will use
the TrkExtrapolator package to calculate a perigee. For info on how to use the TrkParticleCreator tool have a look
at one of its clients: InDetParticleCreation.

@section Particle_general TrackParticle in general
If TrackParticle is used not only as input but also as output (i.e. objects of TrackParticle
are created by an algorithm), please consider the following:<br>
<p>The datamembers m_originalTrack and m_reconstructedVertex are pointer datamembers to
a track and a vertex respectively. Where they point to can be assigned via the constructor.
Usually the ownership of an object is given away with the pointer, and the object which gets
the pointer has to delete the object it points to. For pointer datamembers this is usually
done in the destructor of the class. <b>For TrackParticle this is different.</b> The pointer
datamembers are <b>NOT DELETED</b> when a TrackParticle gets out of scope. The reason is
that usually m_originalTrack and m_reconstructedVertex point to objects in StoreGate and therefore
must not be deleted by the destructor of TrackParticle. This would cause Athena to crash.</p>
<br>
<b>Summary:</b> If you construct your own TrackParticle in one of your algorithms and you use
<b>new</b> statements to pass a vertex or a track, be sure you <b>delete</b> them at the end.

@section Particle_innerdetector TrackParticle in InnerDetector
In InnerDetector software the TrackParticle is created by InDetParticleCreation after
the primary vertex reconstruction. In that case it always has a pointer to a track,
a pointer to a vertex and the info of the vertex type. The vertex pointer <b>always</b>
points to the <b>primary vertex</b>. In case the track of the TrackParticle was used in
the fit of the primary vertex the datamember <i>m_trackParticleOrigin</i> will have
the value Rec::PriVtx. If the track was not used in the primary vertex fit this
enum has the value Rec::NoVtx. The StoreGate key for the ID TrackParticleContainer is "TrackParticleCandidate".


@section Particle_packagecontent Package Contents:
Particle contains the following files/classes:
 - TrackParticle.cxx ... the TrackParticle itself.
 - TrackParticleContainer.cxx ... container to store TrackParticle(s) in StoreGate.

@section Particle_ExtraPages Extra Pages

*/
