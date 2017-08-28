========================
DiTauTruthMatchingTool
========================

:authors: David Kirchmeier
:contact: david.kirchmeier@cern.ch

.. contents:: Table of contents

**NOTE:** To use this tool it is necessary that the decoration
``n_subjets`` is available for each boosted di-tau candidate. 
This information is decorated by `tauRecTools/DiTauIDVarCalculator <https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/tauRecTools/tags/tauRecTools-00-00-12-21/Root/DiTauIDVarCalculator.cxx>`_, which should run before DiTauTruthMatchingTool.

------------
Introduction
------------

This tool performs a truth matching for reconstructed boosted di-tau candidates. 
Most of the functionality of this tool is adapted from `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_. This tool searches for truth taus, muons and electrons which are matching to subjets of boosted di-tau candidates. A di-tau candidate is considered as ``"IsTruthMatched"`` when the leading subjet as well subleading subjet is matching to truth leptons within a certain delta R cone. When both subjets are matching to different truth hadronic tau decays, this candidate is additionally considered as ``"IsTruthHadronic"``. Those information are decorated to the xAOD DiTauJet.

The tool needs to include the following header file::

  #include "TauAnalysisTools/DiTauTruthMatchingTool.h"

The tool needs to be initialized at least in this way::

  DiTauTruthMatchingTool* DiTauTruthMatchingTool = new TauTruthMatchingTool("DiTauTruthMatchingTool");
  DiTauTruthMatchingTool->initialize();

The truth match is made with an upper cut on DeltaR between the reconstructed
ditau subjet and the visible 4-momentum of the truth tau, electron or
muon. The default value is DeltaR=0.2. However, one can adjust this value via::

  DiTauTruthMatchingTool->setProperty("MaxDeltaR", .2);

The tool checks itself if there is already a TruthTau container stored in the
file or generates the TruthTau container if necessary. (For more information see `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_).

The truth matching is done for either a single xAOD DiTauJet object or for a DiTauJets container with these functions::

  void DiTauTruthMatchingTool::applyTruthMatch(const xAOD::DiTauJet& xDiTau);
  void DiTauTruthMatchingTool::applyTruthMatch(const std::vector<const xAOD::DiTauJet*>& vDiTaus);


------------------
Tool configuration
------------------

.. list-table::
   :header-rows: 1

   * - property name
     - default value
     - explanation

   * - MaxDeltaR
     - 0.2
     - Maximum delta R between reconstructed tau and truth

   * - TruthTauContainerName
     - TruthTaus
     - Name of truth tau container within the xAOD file

   * - TruthMuonContainerName
     - TruthMuons
     - Name of truth muon container within the xAOD file

   * - TruthElectronContainerName
     - TruthElectrons
     - Name of truth electron container within the xAOD file

   * - TruthJetContainerName
     - AntiKt4TruthJets
     - Name of truth jet container within the xAOD file

   * - TruthParticlesContainerName
     - TruthParticles
     - Name of truth particles container within the xAOD file needed if truth
       tau container needs to be created

   * - WriteTruthTaus
     - false
     - switching this option to 'true' writes the created container (only in
       case it is not found in the event store and thus newly created) to the
       event store

   * - NewTruthTauContainerName
     - TruthTaus
     - Name of the newly created truth tau container if WriteTruthTaus == true

------
Output
------

Calling applyTruthMatch(xDiTau) adds a vector of element links to the truth lepton
particle as decoration: ``truthParticleLink``. Those can be accessed via::

  auto vElementLinks = xDiTau.auxdata<std::vector<ElementLink<xAOD::TruthParticleContainer>>>("truthParticleLinks");

In addition two variables of type char are decorated to the DiTauJet 

* ``IsTruthMatched`` indicating if two leading subjets match to truth leptons
* ``IsTruthHadronic`` indicating if two leading subjets match to truth hadronic tau decays

After calling applyTruthMatch(xDiTau) those information can be retrieved via::

  bool bMatched = (bool)xDiTau.auxdata<char>("IsTruthMatched")
  bool bHadronic = (bool)xDiTau.auxdata<char>("IsTruthHadronic")


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
  * `TauOverlappingElectronLLHDecorator <README-TauOverlappingElectronLLHDecorator.rst>`_

