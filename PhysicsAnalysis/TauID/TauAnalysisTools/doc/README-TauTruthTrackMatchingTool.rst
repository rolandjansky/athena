=========================
TauTruthTrackMatchingTool
=========================

:authors: Dirk Duschinger, Marco Milesi
:contact: dirk.duschinger@cern.ch, marco.milesi@cern.ch

.. contents:: Table of contents

------------
Introduction
------------

This aims to add information to xAOD track particles which are mainly relevant
for analyses with hadronic tau leptons. This tool largely depends on the truth
matching performance of tracks to truth particles. So if the this information is
not correctly calculated, this tool will most probably deliver unreliable
results.

The tool needs to include this header file::

  #include "TauAnalysisTools/TauTruthTrackMatchingTool.h"

The initialization is fairly easy::

  TauTruthTrackMatchingTool* T3MT = new TauTruthTrackMatchingTool("T3MT");
  CHECK(T3MT->initialize());

The information is added to a single track by calling::

  StatusCode TauTruthTrackMatchingTool::classifyTrack(const xAOD::TrackParticle& xTrackParticle)

or to a vector of tracks via::

  StatusCode TauTruthTrackMatchingTool::classifyTracks(std::vector<const xAOD::TrackParticle*>& vTrackParticles)

Note, that one needs to retrieve the TruthParticleContainer in each event, such
that the tool can find the truth information, i.e. one need the following
lines::

  const xAOD::TruthParticleContainer* TruthParticles = 0;
  CHECK( event.retrieve( TruthParticles, "TruthParticle" ) );
  T3MT->setTruthParticleContainer(m_xTruthParticleContainer);

The information added to each track is as follows:

.. list-table::
   :header-rows: 1
   :widths: 10 10 20

   * - property name
     - variable type
     - explanation

   * - IsHadronicTrack
     - bool
     - true if this track is matched to a truth particle somewhere in the decay
       cascade of a hadronic tau, false otherwise

   * - IsHadronicTrackDecayDepth
     - int
     - distance of the matched truth particle to the hadronic tau decay, the
       value is 0, if the truth particle is a direct daughter of the hadronic
       tau decay
       
   * - DecayHistory
     - std::string
     - details the origin of the track as a string in form of pdgID numbers
       separated by a colon, the first number either denotes the hadronic tau
       the track is coming from, or first particle in the decay chain, if it is
       not a track from a hadronic tau decay.

   * - SpuriousType
     - int
     - classification of spurious tracks based on barcode regions and the truth
       matching probability of the corresponding track (see below). Note, that
       this variable is also evaluated for tracks coming from truth hadronic
       taus.


For example if a tau decays into a neutral and a charged pion, where the neutral
decays into two photons and one of the photons decays into an electron positron
pair, one would get if the truth matching performs well three tracks with
IsHadronicTrack == true. The single tracks would have the following information
in the auxiliary store::

* charged pion from hadronic decay: DecayHistory == -15:211, IsHadronicTrackDecayDepth == 0
* electron from photon conversion: DecayHistory == -15:111:22:11, IsHadronicTrackDecayDepth == 2
* positron from photon conversion: DecayHistory == -15:111:22:-11, IsHadronicTrackDecayDepth == 2

all other tracks not matched to a truth hadronic tau decay have
IsHadronicTrackDecayDepth == -1 and the first pdgID in the DecayHistory is not
15 or -15.

The spurious track classification is based on the tracks barcode and truth
matching probability as follows:

.. list-table::
   :header-rows: 1
   :widths: 10 10 20

   * - value
     - truth matching probability
     - barcode

   * - TauAnalysisTools::FakeTrack
     - > 0.5
     - 0

   * - TauAnalysisTools::FakeTrack
     - < 0.5
     - < 200000
     
   * - TauAnalysisTools::UnderlyingEventTrack
     - > 0.5
     - 1 - 10000

   * - TauAnalysisTools::PileupTrack
     - > 0.5
     - 10000 - 200000

   * - TauAnalysisTools::ConversionTrack
     - no cut
     - > 200000

----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `TauSelectionTool <README-TauSelectionTool.rst>`_
  * `TauSmearingTool <README-TauSmearingTool.rst>`_
  * `TauEfficiencyCorrectionsTool <README-TauEfficiencyCorrectionsTool.rst>`_

    * `mc12 recommendations <README-TauEfficiencyCorrectionsTool-mc12.rst>`_
    * `mc15 pre-recommendations <README-TauEfficiencyCorrectionsTool-mc15_pre-recommendations.rst>`_
    * `TauEfficiencyCorrectionsTool Trigger <README-TauEfficiencyCorrectionsTool_Trigger.rst>`_

  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_
