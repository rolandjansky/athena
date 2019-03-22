====================
TauTruthMatchingTool
====================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

**NOTE:** There was a bug in the truth tau logic for unstable decay products in releases 21.2.68 and earlier. Depending on the MC generator this led to too low visible truth momenta in some events. This was fixed in 21.2.69 with this merge request: https://gitlab.cern.ch/atlas/athena/merge_requests/22105
   
**NOTE:** checkout the NEW `FAQ`_, especially `Is there an easy way to find out the truth type and origin of the truth particle? <README-TauTruthMatchingTool.rst#question-is-there-an-easy-way-to-find-out-the-truth-type-and-origin-of-the-truth-particle>`_.
   
------------
Introduction
------------

This tool is designed to perform matching of reconstructed xAOD tau objects to
the visible 4-momentum of truth taus, electrons and muons. The matching is done
with the following preference:

#. Taus
#. Muons
#. Electrons

Also matching to
truth jets is done, but these are treated a bit differently in terms of each tau
can be matched to truth leptons (e, mu, tau) and truth jets at the same
time. The tool needs to include the following header file::

  #include "TauAnalysisTools/TauTruthMatchingTool.h"

The tool needs to be initialized at least in this way::

  TauTruthMatchingTool* T2MT = new TauTruthMatchingTool("T2MT");
  T2MT->initialize();

The verbosity level of the tool can be set like for all AsgTool via::
  
  T2MT->msg().setLevel( MSG::INFO );

The truth match is made with an upper cut on DeltaR between the reconstructed
tau 4-momentum and the visible 4-momentum of the truth tau, electron or
muon. The default value is DeltaR=0.2. However, one can adjust this value via::

  T2MT->setProperty("MaxDeltaR", .2);

The tool checks itself if there is already a TruthTau container stored in the
file (most likely written out by the truth MC derivation). If it does not find
it the TruthTau container will be generated from the TruthParticles container
with a very similar code used in the derivations (w/o information from
MCTruthClassifier). In this case it is necessary to have a sufficiently complete
TruthParticle container available, i.e. one should not have slimmed the tau
decay products from the container. Information on how to change the default container names can be found in the section `Tool configuration`_.
  
The truth matching is done for a single xAOD tau object with one of these
functions::

  xAOD::TruthParticle* TauTruthMatchingTool::applyTruthMatch(const xAOD::TauJet& xTau)
  xAOD::TruthParticle* TauTruthMatchingTool::getTruth(const xAOD::TauJet& xTau)

which returns a pointer to the truth lepton particle the xAOD tau object was
matched to. If there was no match found, the return value is ``NULL``. Similarly
for a vector of xAOD tau objects one can use this function::

  std::vector<xAOD::TruthParticle*> TauTruthMatchingTool::applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus)

which returns a vector of pointer to the matched truth lepton particle in the
same order. Note, that again, if there is no truth particle found, the entry in
the returned vector is ``NULL``.

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

Calling applyTruthMatch(xTau) or getTruth(xTau) adds a link to the truth lepton
particle as decoration: ``truthParticleLink``. A link to the matched truth jet
is decorated as well with name ``truthJetLink``. Please check the validity of
this link before trying to access the linked truth jet or make use of the
getLink function from TauxAODHelpers::

    const xAOD::Jet* xTruthJet = xAOD::TauHelpers::getLink<xAOD::Jet>(xTau, "truthJetLink");

If TruthTaus is created from the TauTruthMatchingTool and not connected to the
event store with option WriteTruthTaus, only a pointer to the TruthTau can be
attached to the tau; decoration: ``TruthTau``.

In addition a variable of type char is decorated to the tau named
``IsTruthMatched`` indicating, if a match to a truth lepton was found (ignoring
possible matches to a truth jet).

-------------------
Truth Tau Container
-------------------

In case the TruthTau container is created by the tool the following information
will be added to the truth taus. This is similar to the information added to
truth taus in the derivation, missing is only the information form
MCTruthClassifier (i.e. branches particleType and particleOrigin).

.. list-table::
   :header-rows: 1
   :widths: 10 20 20
   
   * - decoration name
     - variable type
     - explanation
     
   * - IsHadronicTau
     - char
     - true if it is a hadronic tau decay, false if it is leptonic

   * - originalTruthParticle
     - element link to truth particle
     - link to the truth particle in the TruthParticlesContainer
       
   * - numCharged, numChargedPions, numNeutral, numNeutralPions
     - size_t
     - Number of charged/neutral particles and pions
     
   * - pt_vis, eta_vis, phi_vis, m_vis
     - double
     - The truth visible 4 momentum components

   * - pt_invis, eta_invis, phi_invis, m_invis
     - double
     - The truth invisible 4 momentum components

   * - pt_prompt, eta_prompt, phi_prompt, m_prompt
     - double
     - The 4 momentum components of the first occurrence of the tau

   * - DecayModeVector
     - std::vector<int>
     - pdg ID's of stable decay products of the tau decay stored as a vector

    
A second feature available in this tool is to retrieve a container of truth tau
particles which can be stored in an output file. The type of the created
container is xAOD::TruthParticleContainer and its corresponding auxiliary
container. It contains all hadronically and leptonically decaying taus found in
the passed truth particle container. To retrieve the truth tau container and
auxiliary container like::

  xAOD::TruthParticleContainer* xTruthTauContainer = T2MT->getTruthTauContainer();
  xAOD::TruthParticleAuxContainer* xTruthTauAuxContainer = T2MT->getTruthTauAuxContainer();

-----------------
Wrapper functions
-----------------

To make more easy access to the truth match information a couple of wrapper
functions are available

* functions to get the total number of truth particles of a given type::
  
    virtual int getNTauDecayParticles(const xAOD::TauJet& xTau, int iPdgId, bool bCompareAbsoluteValues = false) const;
    virtual int getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues = false) const;

  count number of decay particles of given pdg id. if bCompareAbsoluteValues is
  set to true, absolute values are compared, which may be used to count
  positively and negatively charged particles in one rush. If the functions are
  used passing the xTruthTau, the information will be directly parsed from it's
  DecayModeVector. If the reconstructed xTau is passed, the truth particle will
  be searched first and numbers are calculated from this match.

* functions returning the DecayMode are only available for mc15 and so for 2.3.X
  releases. The returned enum is identical with the enum DecayMode defined in
  xAODTau/TauDefs.h. The functions are::

    virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TauJet& xTau) const;
    virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TruthParticle& xTruthTau) const;

* get type of truth match particle (hadronic tau, leptonic tau, electron, muon, jet)::
      
    virtual TauAnalysisTools::TruthMatchedParticleType getTruthParticleType(const xAOD::TauJet& xTau) const;

  Although the particle can be matched to taus/muon/electron and jets at the
  same time, the truth particle type will only be ``TruthJet``, if no match to a
  taus/muon/electron was found, but a match to a truth jet. The definition of
  ``TauAnalysisTools::TruthMatchedParticleType`` can be found in `Enums.h
  <https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/TauAnalysisTools/Enums.h>`_
  
* functions returning the TLorentzVector of the visible, invisible and prompt
  components::

    virtual TLorentzVector getTruthTauP4Vis(const xAOD::TauJet& xTau) const;
    virtual TLorentzVector getTruthTauP4Vis(const xAOD::TruthParticle& xTruthTau) const;

    virtual TLorentzVector getTruthTauP4Invis(const xAOD::TauJet& xTau) const;
    virtual TLorentzVector getTruthTauP4Invis(const xAOD::TruthParticle& xTruthTau) const;

    virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TauJet& xTau) const;
    virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TruthParticle& xTruthTau) const;

---
FAQ
---

**Question:** Is there an easy way to find out the truth type and origin of the truth particle?
-----------------------------------------------------------------------------------------------

**Answer:** Yes, but only if the truth containers are produced in
derivations. Then particles from the TruthTaus, TruthElectrons and TruthMuons
should contain decorations named ``classifierParticleType`` and
``classifierParticleOrigin``. These variables are coming from `MCTruthClassifier
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MCTruthClassifier>`_. For
example if you want to check if a truth particle is a prompt tau you can do the
following::

  if (xTruthTau->auxdata<unsigned int>("classifierParticleType") == MCTruthPartClassifier::IsoTau)
  {
    ...
  }

**Question:** Is there any difference between applyTruthMatch and getTruth?
---------------------------------------------------------------------------

**Answer:** No, it is basically the same function, applyTruthMatch is for
backwards compatibility, so better call getTruth to reduce overhead.

**Question:** How do I check if the truth matched particle is an electron, muon, tau?
-------------------------------------------------------------------------------------

**Answer:** You can simply check, e.g. for electrons::
  
  if (xTruthParticle->isElectron())
  {
    ...
  }

**Question:** How do I check if the tau is matched to an hadronic tau?
----------------------------------------------------------------------

**Answer:** You can check the IsHadronicTau decoration of a truth tau, but you
first need to check if it is a tau::

  if (xTruthParticle->isTau())
  {
    ...
  }

then check the decoration::
  
  if ((bool)xTruthParticle->auxdata<char>("IsHadronicTau"))
  {
    ...
  }

**Question:** I can only find at most one truth matched tau per event. Is this normal?
--------------------------------------------------------------------------------------

**Answer:** No, it isn't. You probably used TauTruthMatchingTool-00-01-06 or
older for the truth matching. There was a bug in this and older tags, which was
fixed in TauTruthMatchingTool-00-01-07. So please update.

   
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
