===================================================
TauEfficiencyCorrectionsTool special notes for mc12
===================================================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

EfficiencyCorrectionTypes
=========================

For the non-default value "mc12-final" for ``RecommendationTag``, which should
be used for run 1 analyses, the following enums are available for
``EfficiencyCorrectionTypes``

* SFJetIDHadTau: scale factors for tau jet identification of true hadronic tau decays
* SFContJetIDHadTau: scale factors for jet identification of true hadronic tau decays (depending on continuous BDT score rather then ID working point)
* SFEleIDHadTau: scale factors for tau electron identification of true hadronic tau decays

More configurations for specific EfficiencyCorrectionTypes
==========================================================

SFJetIDHadTau
-------------

The following working points are available:

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

SFContJetIDHadTau
-----------------

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

SFEleIDHadTau
-------------

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
