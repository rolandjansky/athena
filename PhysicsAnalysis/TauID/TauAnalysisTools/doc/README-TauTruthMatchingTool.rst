====================
TauTruthMatchingTool
====================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

------------
Introduction
------------

This tool is designed to perform matching of reconstructed xAOD tau objects to
the visible 4-momentum of truth taus. The tool needs to include the following
header file::

  #include "TauAnalysisTools/TauTruthMatchingTool.h"

The tool needs to be initialized at least in this way::

  TauTruthMatchingTool* T2MT = new TauTruthMatchingTool("T2MT");
  T2MT->initialize();

The verbosity level of the tool can be set like for all AsgTool via::
  
  T2MT->msg().setLevel( MSG::INFO );

There is an option to define the used type of MC generator::  
  
  T2MT->setProperty("SampleType", (int)TauAnalysisTools::SHERPA);
  // or
  T2MT->setProperty("SampleType", (int)TauAnalysisTools::PYTHIA);

The currently available options are `SHERPA` and `PYTHIA`, where `PYTHIA` is the
default. The truth match is made with an upper cut on DeltaR between the
reconstructed tau 4-momentum and the visible 4-momentum of the truth tau. The
default value is DeltaR=0.2. However, one can adjust this value via::

  T2MT->setProperty("MaxDeltaR", .2);

Before applying a truth match, the tool needs to know about to the truth
container. This is achieved by passing the pointer to the truth container to the
tool in each event::

  T2MT->setTruthParticleContainer(m_xTruthParticleContainer);

The truth matching is done for a single xAOD tau object with this function::

  xAOD::TruthParticle* TauTruthMatchingTool::applyTruthMatch(const xAOD::TauJet& xTau)

which returns a pointer to the truth particle the xAOD tau object was matched
to. If there was no match found, the return value is NULL. Similarly for a
vector of xAOD tau objects one can use this function::

  std::vector<xAOD::TruthParticle*> TauTruthMatchingTool::applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus)

which returns a vector of pointer to the matched truth particle in the same
order. Note, that again, if there is no truth particle found, the entry in the
returned vector is NULL.

To make more easy access to the truth match information a couple of information
is added as auxiliary decorations to the passed xAOD tau objects and the truth
particle as well. The following provides an overview of the available decorations:


.. list-table::
   :header-rows: 1
   :widths: 10 10 20 5
   
   * - property name
     - variable type
     - explanation
     - value if there is no truth match?
     
   * - IsTruthMatched
     - bool
     - true if a truth match was found, false otherwise
     - false

   * - TruthProng
     - size_t
     - number of charged decay particles from truth tau decay
     - 1111
     
   * - TruthCharge
     - int
     - charge of truth tau particle
     - -1111

.. list-table::
   :header-rows: 1
   :widths: 10 10 20

   * - property name
     - variable type
     - explanation
     
   * - IsHadronicTau
     - bool
     - added to truth tau objects; is false, if at least one tau decay particle
       is a lepton, which includes processes, like tau->tau+gamma, or if there
       is no decay vertex. It's true otherwise.
     
   * - pt_vis, eta_vis, phi_vis, m_vis
     - double
     - kinematic variables of the visible 4-momentum

   * - DecayMode
     - std::string
     - decay mode of the hadronic decay, e.g. 1p1n or 3p2n
     
   * - NTracks
     - size_t
     - If it is a truth hadronic tau decay: number of charged decay particles

-------------------
Truth Tau Container
-------------------
    
A second feature available in this tool is to retrieve a container of truth tau
particles which can be stored in an output file. The type of the created
container is xAOD::TruthParticleContainer and its corresponding auxiliary
container. It contains all hadronic decaying taus found in the passed truth
particle container. To retrieve the truth tau container again one needs first to
set the truth particle container in each event::

  T2MT->setTruthParticleContainer(m_xTruthParticleContainer);

After that, one can create the container via::

  T2MT->createTruthTauContainer();

and retrieve the truth tau container and auxiliary container like::

  xAOD::TruthParticleContainer* xTruthTauContainer = T2MT->getTruthTauContainer();
  xAOD::TruthParticleAuxContainer* xTruthTauAuxContainer = T2MT->getTruthTauAuxContainer();

----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `TauSelectionTool <README-TauSelectionTool.rst>`_
  * `TauSmearingTool <README-TauSmearingTool.rst>`_
  * `TauEfficiencyCorrectionsTool <README-TauEfficiencyCorrectionsTool.rst>`_
  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_

