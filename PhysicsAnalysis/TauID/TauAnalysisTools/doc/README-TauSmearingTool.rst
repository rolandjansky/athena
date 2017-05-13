===============
TauSmearingTool
===============

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents


-------
Preface
-------

**NOTE**: To use this tool it is necessary that the decoration
``truthParticleLink`` is available for each tau. Further this link should be a
valid link, that means, that the linked truth particle needs to be
accessible. I.e. if the linking is done in derivations to the TruthTau
container, the TruthTau container must be kept. For more information on how to
achieve this, please refer to the `tau pre-recommendations TWiki
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information>`_

**IMPORTANT: If you used TauTruthMatchingTool-00-01-06 or older, for example
within the derivation, the variable** ``truthParticleLink`` **is bugged. This
can be fixed by rerunning the truth matching with a newer tag, i.e. before
applying the correction you have to call for each tau**::

  xAOD::TruthParticle* TauTruthMatchingTool::getTruth(const xAOD::TauJet& xTau)

For more information on truth matching please refer to `TauTruthMatchingTool
<README-TauTruthMatchingTool.rst>`_.

------------
Introduction
------------

This tool provides corrections to your tau pT according to the TES
uncertainties. The momentum of xAOD taus have already been corrected for proper
tau energy scale, and therefore the tool returns for simulation a correction
factor of 1. In run 1 data the tau pT is shifted and needs to be corrected.

The tool can in general be used, including the header file::

  #include "TauAnalysisTools/TauSmearingTool.h"

and calling::

  TauAnalysisTools::TauSmearingTool TauSmeTool( "TauSmaringTool" );
  TauSmeTool.initialize();

  TauSmeTool.applyCorrection(xTau);

The set of recommended systematic variations can in general be retrieved by
calling::

  CP::SystematicSet recommendedSystematicSet = TauSmeTool.recommendedSystematics();

A complete set of available (including the recommended) systematic variations
are retrieved via::

  CP::SystematicSet affectingSystematicSet = TauSmeTool.affectingSystematics();

The tool can be configured to use a specific set of systematic variations calling::

  TauSmeTool.applySystematicVariation(customSystematicSet);


--------------------
Available properties
--------------------

Overview
========

The tool can be used to apply tau pt smearing for a specific
``RecommendationTag``:

+-------------------------------+------------------+----------------+--------------------------------------------------------+
| property name                 | type             | default value  | other sensible values                                  |
+===============================+==================+================+========================================================+
| RecommendationTag             | std::string      | "mc15-moriond" | "mc15-pre-recommendations", "mc12-final", "mc11-final" |
+-------------------------------+------------------+----------------+--------------------------------------------------------+

For the default ``RecommendationTag`` "mc15-moriond" the following properties
are available for tool steering:

+-------------------------------+------------------+------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| property name                 | type             | default value                                                                            | other sensible values                                                         |
+===============================+==================+==========================================================================================+===============================================================================+
| InputFilePath                 | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"Smearing/TES_TrueHadTau_mc15_moriond.root" |                                                                               |
+-------------------------------+------------------+------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+

For the ``RecommendationTag`` "mc15-pre-recommendations" the following
properties are available for tool steering:

+-------------------------------+------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| property name                 | type             | default value                                                                           | other sensible values                                                         |
+===============================+==================+=========================================================================================+===============================================================================+
| InputFilePath                 | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"Smearing/TES_TrueHadTau_mc15_prerec.root" |                                                                               |
+-------------------------------+------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+

For the ``RecommendationTag`` "mc12-final" the following properties are
available for tool steering:

+-------------------------------+------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| property name                 | type             | default value                                                                           | other sensible values                                                         |
+===============================+==================+=========================================================================================+===============================================================================+
| InputFilePath                 | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"Smearing/mc12_p1344_medium.root"          | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"Smearing/mc12_p1344_tight.root" |
+-------------------------------+------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| IsData                        | bool             | false                                                                                   | true                                                                          |
+-------------------------------+------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+

For the ``RecommendationTag`` "mc11-final" the following properties are
available for tool steering:

+-------------------------------+------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| property name                 | type             | default value                                                                           | other sensible values                                                         |
+===============================+==================+=========================================================================================+===============================================================================+
| InputFilePath                 | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"Smearing/mc11.root"                       |                                                                               |
+-------------------------------+------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+
| IsData                        | bool             | false                                                                                   | true                                                                          |
+-------------------------------+------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------+

Notes for run 1 tau pt smearing
===============================

If you like to access a specific component of the TES uncertainty, i.e.
statistical uncertainty of the in-situ measurement, you need to create a
systematic set and add a systematic variation with the name TAUS_SME_INSITUSTAT
and an integer corresponding to the up/downward variation (a positive number
corresponds to upward, a negative to downward variation). So for an upward 1
sigma variation one would write::

  CP::SystematicSet sSystematicSet;
  sSystematicSet.insert(CP::SystematicVariation("TAUS_SME_INSITUSTAT", 1));
  TauSmeTool.applySystematicVariation( sSystematicSet );

**However, you should get in contact with TauWG first before doing this in your analysis!**

New nuisance parameters are provided for single TES components:

* FINAL: "old style" total TES uncertainty
* TOTAL: total TES uncertainty w/ constraints from in-situ measurement at low pt
  (pt < 50 GeV), i.e. sqrt(MODELING**2 + CLOSURE**2 + INSITUINTERPOL**2 +
  SINGLEPARTICLE**2)
* INSITU: total in-site component, i.e. sqrt(INSITUSYS**2 + INSITUSTAT**2)

  **NOTE: no interpolation is applied here; if you want to apply interpolation
  take INSITUINTERPOL**
* INSITUINTERPOL: total in-situ component with pt interpolation according to
  sqrt(1 - (pt -50)/20) * INSITU for 50 GeV < pt < 70 GeV; above pt > 70 GeV the
  interpolation factor is 0, while for pt < 50 GeV it is 1
* INSITUSTAT/INSITUSYST: statistical and systematic component of in-situ
  measurement
* SINGLEPARTICLEINTERPOL: single particle response interpolated as "switch-on",
  i.e. (1 - sqrt(1 - (pt -50)/20)) * SINGLEPARTICLE
* MODELING: modelling component


---
FAQ
---

#. **Question:** How can I access systematic variations for a specific nuisance
   parameter

   **Answer:** There are many ways to do that, one is for example on AFII up
   variation::

     // create and initialize the tool
     TauAnalysisTools::TauSmearingTool TauSmeTool( "TauSmearingTool" );
     TauSmeTool.initialize();

     // create empty systematic set
     CP::SystematicSet customSystematicSet;
     
     // add systematic up variation for AFII systematic and true hadronic taus to systematic set
     customSystematicSet.insert(CP::SystematicVariation ("TAUS_TRUEHADTAU_SME_RECO_AFII", 1));

     // tell the tool to apply this systematic set
     TauSmeTool.applySystematicVariation(customSystematicSet);

     // and finally apply it to a tau
     TauSmeTool.applyCorrection(xTau);
     
   if the down variation is needed, one just needs to use a ``-1`` in the line,
   where the systematic variation is added to the systematic set.

#. **Question:** I try to apply systematic variation running on derived samples,
   but I get an error like::
     
     TauAnalysisTools::CommonSmearingTool::checkTruthMatch(const TauJet&) const): No truth match information available. Please run TauTruthMatchingTool first.
     terminate called after throwing an instance of 'SG::ExcBadAuxVar'
     what():  SG::ExcBadAuxVar: Attempt to retrieve nonexistent aux data item `::truthParticleLink' (894).

   **Answer:** Did you follow instructions for adding truth information in
   derivations as described in `TauPreRecommendations2015 TWiki
   <https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information>`_?
   If not, do so!

#. **Question:** But I seriously can't wait for new derivations, is there a way
   to avoid the error due to the non existing ``truthParticleLink``?

   **Answer:** Yes there is, but this is only for testing purpose! One simply
   needs to set the property ``SkipTruthMatchCheck`` to true::

     TauEffTool.setProperty("SkipTruthMatchCheck", true );

#. **Question:** I try to apply systematic variation running on xAOD samples,
   but I get an error like::
     
     TauAnalysisTools::CommonSmearingTool::checkTruthMatch(const TauJet&) const): No truth match information available. Please run TauTruthMatchingTool first.
     terminate called after throwing an instance of 'SG::ExcBadAuxVar'
     what():  SG::ExcBadAuxVar: Attempt to retrieve nonexistent aux data item `::truthParticleLink' (894).

   **Answer:** If you have full access to the TruthParticle container, you can
   create a TruthTau container and the link to the matched truth taus by setting
   up the `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_ and to the
   truth matching for each tau. Note that you need to must set the property
   "WriteTruthTaus" to true to get it working.

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
