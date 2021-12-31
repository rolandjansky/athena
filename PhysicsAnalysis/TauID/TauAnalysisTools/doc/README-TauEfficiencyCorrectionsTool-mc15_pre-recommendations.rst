============================
TauEfficiencyCorrectionsTool
============================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

EfficiencyCorrectionTypes
-------------------------

the default value for ``RecommendationTag`` the following enums for the property
``EfficiencyCorrectionTypes`` can be used to obtain the corresponding scale
factors:

* SFRecoHadTau: scale factors for tau reconstruction of true hadronic tau decays
* SFEleOLRHadTau: scale factors for tau electron overlap removal of true hadronic tau decays
* SFJetIDHadTau: scale factors for tau jet identification of true hadronic tau decays

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
