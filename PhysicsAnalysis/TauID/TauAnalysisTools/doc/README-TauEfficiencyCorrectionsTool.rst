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
container, the ``TruthTaus`` and ``TruthElectrons`` container must be kept. For
more information on how to achieve this, please refer to the `tau
pre-recommendations TWiki
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauPreRecommendations2015#Accessing_Tau_Truth_Information>`_.
For more information on truth matching please refer to `TauTruthMatchingTool
<README-TauTruthMatchingTool.rst>`_.

------------
Introduction
------------

This tool aims to provide nominal and systematically varied efficiency scale
factors for tau reconstruction, identification and overlap removal. Also tau
trigger efficiency scale factors are provided by this tool, but there are some
differences to the other scale factors. For more information on how to use the
tool for trigger scale factors please refer to the `TauEfficiencyCorrectionsTool
-- Trigger readme <README-TauEfficiencyCorrectionsTool_Trigger.rst>`_. Please
note that this documentation is only valid for the latest recommendations.
More information for older recommendations can be found following the links
listed in `Section Special Notes on older recommendations
<README-TauEfficiencyCorrectionsTool.rst#special-notes-on-older-recommendations>`_.

For the tool the following line needs to be added to include the header file::

  #include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

The tool is in general created and initialized by::

  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTool( "TauEfficiencyCorrectionsTool" );
  TauEffTool.initialize();

Scale factors can be decorated to the tau or retrieved from the tool by::
  
  TauEffTool.applyEfficiencyScaleFactor(xTau);                                     // either directly appending scale factors to the xAOD tau auxiliary store
  TauEffTool.getEfficiencyScaleFactor(xTau, dEfficiencyScaleFactor);               // or storing fake factors in variable dEfficiencyScaleFactor

The variable names for the scale factors have default values, but can be
configured. For information on this please refer to the `section Available
properties <README-TauEfficiencyCorrectionsTool.rst#available-properties>`_
below.

The ``TauEfficiencyCorrectionsTool`` can be configured in an atomised way by
passing a tool handle of the `TauSelectionTool <README-TauSelectionTool.rst>`_,
which was used for the tau selection

.. code-block:: python

  ToolHandle < TauAnalysisTools::ITauSelectionTool > TauSelToolHandle = TauSelTool;
  TauEffTool.setProperty( "TauSelectionTool" , TauSelToolHandle);

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
``RecommendationTag``.

.. list-table::
   :header-rows: 1
   :widths: 15 10 20 55
      
   * - property name
     - type
     - default value
     - other sensible values

   * - ``RecommendationTag``
     - ``std::string``
     - ``"mc16-prerec"``
     - ``"2017-moriond"``, ``"2016-fall"``, ``"2016-ichep"``, ``"mc15-moriond"``, ``"mc15-pre-recommendations"``, ``"mc12-final"``

For the default ``RecommendationTag`` "mc16-prerec" the following properties
are available for tool steering:

.. list-table::
   :header-rows: 1
   :widths: 25 10 55

   * - property name
     - type
     - default value
 
   * - ``EfficiencyCorrectionTypes``
     - ``std::vector<int>``
     - ``{SFRecoHadTau, SFJetIDHadTau, SFEleOLRHadTau, SFEleOLRElectron}``

   * - ``InputFilePathRecoHadTau``
     - ``std::string``
     - ``"TauAnalysisTools/"+ <SharedFilesVersion> +"EfficiencyCorrections/Reco_TrueHadTau_2016-ichep.root"``

   * - ``InputFilePathEleOLRHadTau``
     - ``std::string``
     - ``"TauAnalysisTools/"+ <SharedFilesVersion> +"EfficiencyCorrections/EleOLR_TrueHadTau_2016-ichep.root"``

   * - ``InputFilePathEleOLRHadTau``
     - ``std::string``
     - ``"TauAnalysisTools/"+ <SharedFilesVersion> +"EfficiencyCorrections/EleOLR_TrueElectron_2017-moriond.root"``

   * - ``InputFilePathJetIDHadTau``
     - ``std::string``
     - ``"TauAnalysisTools/"+ <SharedFilesVersion> +"EfficiencyCorrections/JetID_TrueHadTau_2017-moriond.root"``

   * - ``VarNameRecoHadTau``
     - ``std::string``
     - ``"TauScaleFactorReconstructionHadTau"``

   * - ``VarNameEleOLRHadTau``
     - ``std::string``
     - ``"TauScaleFactorEleOLRHadTau"``

   * - ``VarNameEleOLRElectron``
     - ``std::string``
     - ``"TauScaleFactorEleOLRElectron"``

   * - ``VarNameJetIDHadTau``
     - ``std::string``
     - ``"TauScaleFactorJetIDHadTau"``

In addition the following properties are available for further configurations:
     
.. list-table::
   :header-rows: 1
   :widths: 5 10 5

   * - property name
     - type
     - default value
     
   * - ``TauSelectionTool``
     - ``ToolHandle<TauAnalysisTools::TauSelectionTool>``
     - empty

   * - ``PileupReweightingTool``
     - ``ToolHandle<CP::PileupReweightingTool>``
     - empty

   * - ``TriggerName``
     - ``std::string``
     - ``""``

   * - ``TriggerYear``
     - ``std::string``
     - ``"2016"``
     
   * - ``TriggerSFMeasurement``
     - ``std::string``
     - ``"combined"``
     
   * - ``UseIDExclusiveSF``
     - ``bool``
     - ``false``

   * - ``UseInclusiveEta``
     - ``bool``
     - ``false``

   * - ``UseTriggerInclusiveEta``
     - ``bool``
     - ``true``

   * - ``UsePtBinnedSF``
     - ``bool``
     - ``false``

   * - ``UseHighPtUncert``
     - ``bool``
     - ``false``

   * - ``IDLevel``
     - ``int``
     - ``JETIDBDTTIGHT``

   * - ``OLRLevel``
     - ``int``
     - ``TAUELEOLR``

   * - ``ContSysType``
     - ``int``
     - ``TOTAL``

   * - ``TriggerPeriodBinning``
     - ``int``
     - ``PeriodBinningAll``

Details
=======

EfficiencyCorrectionTypes
-------------------------

The following enums for the property
``EfficiencyCorrectionTypes`` can be used to obtain the corresponding scale
factors:

* SFRecoHadTau: scale factors for tau reconstruction of true hadronic tau decays
* SFEleOLRHadTau: scale factors for tau electron overlap removal of true hadronic tau decays
* SFEleOLRElectron: scale factors for tau electron overlap removal of true electrons faking hadronic taus
* SFJetIDHadTau: scale factors for tau jet identification of true hadronic tau decays

The InputFilePath* strings are predefined to load the files in
/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData/ or
/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ using PathResolver, but own
files can be used as well. If you plan to do this, please contact the author as
there are requirements on the input file for some EfficiencyCorrectionTypes. For
documentation on PathResolver please refer to
https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/PathResolver.

The variable names VarName* for the scale factor decorations are only available
if the corresponding type is requested in ``EfficiencyCorrectionTypes``.

SFJetIDHadTau
-------------

Jet ID scale factors are provided for a couple of working points:

.. list-table::
   :header-rows: 1
   :widths: 5 10

   * - value
     - description

   * - ``JETIDBDTLOOSE``
     - the TauWG jet ID loose working point

   * - ``JETIDBDTMEDIUM``
     - the TauWG jet ID medium working point

   * - ``JETIDBDTTIGHT``
     - the TauWG jet ID tight working point

These can be accessed, for example via::

  TauEffTool.setProperty("IDLevel", (int)JETIDBDTLOOSE);

SFEleOLRElectron
----------------

Electron overlap removal scale factors are provided for a couple of working
points:

.. list-table::
   :header-rows: 1
   :widths: 5 10

   * - value
     - description

   * - ``TAUELEOLR``
     - the TauWG eVeto (OLR very loose e)

   * - ``ELEBDTLOOSE``
     - electron BDT loose working point

   * - ``ELEBDTLOOSEPLUSVETO``
     - electron BDT loose working point + TauWG eVeto

   * - ``ELEBDTMEDIUM``
     - electron BDT medium working point

   * - ``ELEBDTMEDIUMPLUSVETO``
     - electron BDT medium working point + TauWG eVeto

These can be accessed, for example via::

  TauEffTool.setProperty("OLRLevel", (int)TAUELEOLR);



--------------------------------------
Special notes on older recommendations
--------------------------------------

* mc12-final and mc11-final: `README-TauEfficiencyCorrectionsTool-mc12 <README-TauEfficiencyCorrectionsTool-mc12.rst>`_
* mc15-prerecommendations: `README-TauEfficiencyCorrectionsTool-mc15_pre-recommendations <README-TauEfficiencyCorrectionsTool-mc15_pre-recommendations.rst>`_

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

    * `mc12 recommendations <README-TauEfficiencyCorrectionsTool-mc12.rst>`_ 
    * `mc15 pre-recommendations <README-TauEfficiencyCorrectionsTool-mc15_pre-recommendations.rst>`_
    * `TauEfficiencyCorrectionsTool Trigger <README-TauEfficiencyCorrectionsTool_Trigger.rst>`_
  
  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_
  * `TauOverlappingElectronLLHDecorator <README-TauOverlappingElectronLLHDecorator.rst>`_
