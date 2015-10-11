============================
TauEfficiencyCorrectionsTool
============================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

-------
Preface
-------

**NOTE:** To use this tool it is necessary that the decoration
``truthParticleLink`` is available for each tau. Further this link should be a
valid link, that means, that the linked truth particle needs to be
accessible. I.e. if the linking is done in derivations to the TruthTau
container, the TruthTau container must be kept. For more information on how to
achieve this, please refer to the `tau pre-recommendations TWiki
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information>`_

**IMPORTANT: If you used TauTruthMatchingTool-00-01-06 or older, for example
within the derivation, the variable** ``truthParticleLink`` **is bugged. This
can be fixed by rerunning the truth matching with a newer tag, i.e. before
accessing efficiency corrections you have to call for each tau**::

  xAOD::TruthParticle* TauTruthMatchingTool::getTruth(const xAOD::TauJet& xTau)

For more information on truth matching please refer to `TauTruthMatchingTool
<README-TauTruthMatchingTool.rst>`_.

------------
Introduction
------------

This tool aims to provide nominal and systematically varied efficiency scale
factors for tau reconstruction, identification and overlap removal.

For the tool the following line needs to be added to include the header file::

  #include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

The tool is in general created and initialized by::

  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTool( "TauEfficiencyCorrectionsTool" );
  TauEffTool.initialize();

Scale factors can be decorated to the tau or retrieved from the tool by::
  
  TauEffTool.applyEfficiencyScaleFactor(xTau);                                     // either directly appending scale factors to the xAOD tau auxiliary store
  TauEffTool.getEfficiencyScaleFactor(xTau, dEfficiencyScaleFactor);               // or storing fake factors in variable dEfficiencyScaleFactor

The variable names for the scale factors have default values, but can be
configured. For information on this please refer to the section `Available
properties` below.
  
In addition to the standard tool constructor, the TauEfficiencyCorrectionsTool
can be constructed, passing the `TauSelectionTool
<README-TauSelectionTool.rst>`_, which was used for the tau selection::

  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTool( "TauEfficiencyCorrectionsTool" , TauSelTool);

This configures the set of ``EfficiencyCorrectionTypes`` and if needed the jet
(electron) ID working points, depending on the applied cuts.  Please note, that
the tool is only able to see the configuration of the passed TauSelectionTool,
any selection applied outside the passed tool (e.g. harder jet ID, ID control
regions etc...) will not be taken into account.

A set of recommended systematic variations can in general be retrieved by
calling::

  CP::SystematicSet recommendedSystematicSet = TauEffTool.recommendedSystematics();

A complete set of available (including the recommended) systematic variations
are retrieved via::

  CP::SystematicSet affectingSystematicSet = TauEffTool.affectingSystematics();

The tool can be configured to use a specific set of systematic variations
calling::

  TauEffTool.applySystematicVariation(customSystematicSet);

--------------------
Available properties
--------------------

Overview
========

The tool can be used to retrieve scale factors for a specific
``RecommendationTag``:

+-------------------------------+------------------+----------------------------+-----------------------+
| property name                 | type             | default value              | other sensible values |
+===============================+==================+============================+=======================+
| RecommendationTag             | std::string      | "mc15-pre-recommendations" | "mc12-final"          |
+-------------------------------+------------------+----------------------------+-----------------------+

For the default ``RecommendationTag`` "mc15-pre-recommendations" the following
properties are available for tool steering:

+-------------------------------+------------------+---------------------------------------------------------------------------------------------------------+-----------------------+
| property name                 | type             | default value                                                                                           | other sensible values |
+===============================+==================+=========================================================================================================+=======================+
| EfficiencyCorrectionTypes     | std::vector<int> | {SFRecoHadTau, SFJetIDHadTau, SFEleOLRHadTau}                                                           |                       |
+-------------------------------+------------------+---------------------------------------------------------------------------------------------------------+-----------------------+
| InputFilePathRecoHadTau       | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/Reco_TrueHadTau_mc15-prerec.root"   |                       |
+-------------------------------+------------------+---------------------------------------------------------------------------------------------------------+-----------------------+
| InputFilePathEleOLRHadTau     | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/EleOLR_TrueHadTau_mc15-prerec.root" |                       |
+-------------------------------+------------------+---------------------------------------------------------------------------------------------------------+-----------------------+
| InputFilePathJetIDHadTau      | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/JetID_TrueHadTau_mc15-prerec.root"  |                       |
+-------------------------------+------------------+---------------------------------------------------------------------------------------------------------+-----------------------+
| VarNameRecoHadTau             | std::string      | "TauScaleFactorReconstructionHadTau"                                                                    |                       |
+-------------------------------+------------------+---------------------------------------------------------------------------------------------------------+-----------------------+
| VarNameEleOLRHadTau           | std::string      | "TauScaleFactorEleOLRHadTau"                                                                            |                       |
+-------------------------------+------------------+---------------------------------------------------------------------------------------------------------+-----------------------+
| VarNameJetIDHadTau            | std::string      | "TauScaleFactorJetIDHadTau"                                                                             |                       |
+-------------------------------+------------------+---------------------------------------------------------------------------------------------------------+-----------------------+

For the ``RecommendationTag`` "mc12-final" the following properties are
available for tool steering:

+-------------------------------+------------------+--------------------------------------------------------------------------------------------------------------------+-----------------------+
| property name                 | type             | default value                                                                                                      | other sensible values |
+===============================+==================+====================================================================================================================+=======================+
| EfficiencyCorrectionTypes     | std::vector<int> | {SFJetIDHadTau, SFEleIDHadTau}                                                                                     | SFContJetIDHadTau     |
+-------------------------------+------------------+--------------------------------------------------------------------------------------------------------------------+-----------------------+
| InputFilePathJetIDHadTau      | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/JetID_TrueHadTau_mc12-final_ptinc_etabin.root" |                       |
+-------------------------------+------------------+--------------------------------------------------------------------------------------------------------------------+-----------------------+
| InputFilePathContJetIDHadTau  | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/ContJetIDSF.root"                              |                       |
+-------------------------------+------------------+--------------------------------------------------------------------------------------------------------------------+-----------------------+
| InputFilePathEleIDHadTau      | std::string      | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/EleID_TrueHadTau_mc12-final.root"              |                       |
+-------------------------------+------------------+--------------------------------------------------------------------------------------------------------------------+-----------------------+
| VarNameJetIDHadTau            | std::string      | "TauScaleFactorJetIDHadTau"                                                                                        |                       |
+-------------------------------+------------------+--------------------------------------------------------------------------------------------------------------------+-----------------------+
| VarNameContJetIDHadTau        | std::string      | "TauScaleFactorContJetIDHadTau"                                                                                    |                       |
+-------------------------------+------------------+--------------------------------------------------------------------------------------------------------------------+-----------------------+
| VarNameEleIDHadTau            | std::string      | "TauScaleFactorEleIDHadTau"                                                                                        |                       |
+-------------------------------+------------------+--------------------------------------------------------------------------------------------------------------------+-----------------------+

Further comments
================

For the default value for `RecommendationTag` the following enums can be used to
obtain the corresponding scale factors:

* SFRecoHadTau: scale factors for tau reconstruction of true hadronic tau decays
* SFEleOLRHadTau: scale factors for tau electron overlap removal of true hadronic tau decays
* SFJetIDHadTau: scale factors for tau jet identification of true hadronic tau decays

Another sensible value for `RecommendationTag` is "mc12-final" which should be
used for run 1 analyses. The following enums for this `RecommendationTag` are
available:

* SFJetIDHadTau: scale factors for tau jet identification of true hadronic tau decays
* SFContJetIDHadTau: scale factors for jet identification of true hadronic tau decays (depending on continuous BDT score rather then ID working point)
* SFEleIDHadTau: scale factors for tau electron identification of true hadronic tau decays

The InputFilePath* strings are predefined to load the files in
/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData/ or
/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ using PathResolver, but own
files can be used as well. If you plan to do this, please contact the author as
there are requirements on the input file for some EfficiencyCorrectionTypes. For
documentation on PathResolver please refer to
https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/PathResolver.

The variable names VarName* for the scale factor decorations are only available
if the corresponding type is requested in `EfficiencyCorrectionTypes`.

More configurations for specific EfficiencyCorrectionTypes
==========================================================

SFJetIDHadTau
-------------

There are a couple of working points provided for jet ID scale factors:

* JETIDBDTLOOSE
* JETIDBDTMEDIUM
* JETIDBDTTIGHT

The following working points are only available for ``mc12-final``

* JETIDNONE
* JETIDBDTFAIL
* JETIDBDTOTHER
* JETIDLLHLOOSE
* JETIDLLHMEDIUM
* JETIDLLHTIGHT
* JETIDLLHFAIL

These can be accessed, for example via::

  TauEffTool.setProperty("IDLevel", (int)JETIDBDTLOOSE);

The default behaviour for ``mc12-final`` can be changed using the following
property settings. Call::

  TauEffTool.setProperty("UseInclusiveEta", true);

which will switch to eta inclusive scale factors. Call::

  TauEffTool.setProperty("UseIDExclusiveSF", true);

to get exclusive ID (i.e. fail loose, loose not medium, medium not tight and
tight) scale factors. Call::

  TauEffTool.setProperty("UsePtBinnedSF", true);

in order to access pt binned scale factors. Call::

  TauEffTool.setProperty("UseHighPtUncert", true);

in order to inflate uncertainty for pT > 100 GeV. For details please refer to
these `slides
<https://indico.cern.ch/event/304094/contribution/2/material/slides/0.pdf>`_.

The default setup is:

+------------------+---------------+
| property name    | default value |
+==================+===============+
| IDLevel          | JETIDBDTTIGHT |
+------------------+---------------+
| UseIDExclusiveSF | false         |
+------------------+---------------+
| UseInclusiveEta  | false         |
+------------------+---------------+
| UsePtBinnedSF    | false         |
+------------------+---------------+
| UseHighPtUncert  | false         |
+------------------+---------------+

SFContJetIDHadTau (mc12 only!!!)
--------------------------------

To obtain eta inclusive scale factors set property ``UseInclusiveEta`` to
``true``, via::

  TauEffTool.setProperty("UseInclusiveEta", true);

To switch between different types of systematic uncertainties call for example::

  TauEffTool.setProperty("ContSysType", QCD);

The following types of systematic uncertainties are available as enums::

* TOTAL
* QCD
* WJETS
* TES
* REMAIN
  
The default setup is:

+------------------+---------------+
| property name    | default value |
+==================+===============+
| UseInclusiveEta  | false         |
+------------------+---------------+
| ContSysType      | TOTAL         |
+------------------+---------------+

SFEleIDHadTau (mc12 only!!!)
----------------------------

There are a couple of working points provided for electron ID scale factors,
depending on the tau ID working point, electron veto working point and the level
of overlap removal.

The jet ID working points are stored as enums as follows:

* JETIDNONE
* JETIDBDTLOOSE
* JETIDBDTMEDIUM
* JETIDBDTTIGHT
* JETIDBDTFAIL
* JETIDBDTOTHER
* JETIDLLHLOOSE
* JETIDLLHMEDIUM
* JETIDLLHTIGHT
* JETIDLLHFAIL

The electron ID working points are stored as enums as follows:

* ELEIDNONE
* ELEIDBDTLOOSE
* ELEIDBDTMEDIUM
* ELEIDBDTTIGHT
* ELEIDOTHER

The overlap removal levels are stored as enums as follows:

* OLRNONE
* OLRLOOSEPP
* OLRMEDIUMPP
* OLRTIGHTPP
* OLROTHER

E.g. these can be accessed via::

  TauEffTool.setProperty("IDLevel", JETIDBDTLOOSE);
  TauEffTool.setProperty("EVLevel", ELEIDBDTLOOSE);
  TauEffTool.setProperty("OLRLevel", OLRLOOSEPP);
  
The default setup is:

+------------------+---------------+
| property name    | default value |
+==================+===============+
| IDLevel          | JETIDBDTTIGHT |
+------------------+---------------+
| EVLevel          | ELEIDBDTLOOSE |
+------------------+---------------+
| OLRLevel         | OLRLOOSEPP    |
+------------------+---------------+

---
FAQ
---

#. **Question:** How can I access systematic variations for a specific nuisance
   parameter

   **Answer:** There are many ways to do that, one is for example on AFII up
   variation::

     // create and initialize the tool
     TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTool( "TauEfficiencyCorrectionsTool" );
     TauEffTool.initialize();

     // create empty systematic set
     CP::SystematicSet customSystematicSet;
     
     // add systematic up variation for AFII systematic and true hadronic taus to systematic set
     customSystematicSet.insert(CP::SystematicVariation ("TAUS_TRUEHADTAU_EFF_RECO_AFII", 1));

     // tell the tool to apply this systematic set
     TauEffTool.applySystematicVariation(customSystematicSet);

     // and finally apply it to a tau
     TauEffTool.applyEfficiencyScaleFactor(xTau);

   if the down variation is needed, one just needs to use a ``-1`` in the line,
   where the systematic variation is added to the systematic set.


#. **Question:** How can I access a different working point for the jet ID scale factors

   **Answer:** One way is to set the property IDLevel before initializing the tool, i.e.::

     // create the tool
     TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTool( "TauEfficiencyCorrectionsTool" );

     // set the IDLevel property to the loose working point
     TauEffTool.setProperty("IDLevel",(int)JETIDBDTLOOSE)

     // initialize the tool
     TauEffTool.initialize();

     ...

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
  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_
  * `TauOverlappingElectronLLHDecorator <README-TauOverlappingElectronLLHDecorator.rst>`_
