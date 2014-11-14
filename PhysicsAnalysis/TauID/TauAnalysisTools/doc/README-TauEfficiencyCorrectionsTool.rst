============================
TauEfficiencyCorrectionsTool
============================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

------------
Introduction
------------

This tool aims to provide nominal and systematically varied efficiency scale
factors for tau identification and electron veto, i.e. recommended scale factors
labelled `JetID` (binned in pt), preliminary scale factors labelled `ContJetID`
(binned in jet BDT score) and scale factors for the electron veto labelled
`EleID` (binned in eta).

For the tool the following line needs to be added to include the header file::

  #include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"

The tool is in general initialized, e.g. for JetID scale factors, by::

  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTool( "TauEfficiencyCorrectionsTool" );
  TauEffTool.setProperty("EfficiencyCorrectionType", (int)SFJetID);                // note that the cast of the variable to ``int`` is mandatory, due to technical reasons

  TauEffTool.initialize();                                                          // initialize the tool with all previously set properties

  TauEffTool.applyEfficiencyScaleFactor(xTau);                                     // either directly appending scale factors to the xAOD tau auxiliary store
  TauEffTool.getEfficiencyScaleFactor(xTau, dEfficiencyScaleFactor);               // or storing fake factors in variable dEfficiencyScaleFactor

In addition to the standard tool constructor, the TauEfficiencyCorrectionsTool
can be constructed, passing the TauSelectionTool, which was used for the tau
selection::

  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTool( "TauEfficiencyCorrectionsTool" , TauSelTool);

This configures the variables ``IDLevel`` and ``EVLevel``, depending on the
applied cuts ``JetIDWP`` and ``EleBDTWP`` (see `TauSelectionTool <README-TauSelectionTool.rst>`_).

Systematic variations depending on the type of scale factors. A set of
recommended systematic variations can in general be retrieved by calling::

  CP::SystematicSet recommendedSystematicSet = TauEffTool.recommendedSystematics();

A complete set of available (including the recommended) systematic variations
TauAnalysisTools are retrieved via::

  CP::SystematicSet affectingSystematicSet = TauEffTool.affectingSystematics();

The tool can be configured to use a specific set of systematic variations calling::

  TauEffTool.applySystematicVariation(customSystematicSet);

Note, that combination of different systematic variation is only available for
jet ID scale factors.   

The other scale factors can be retrieved by choosing a different
`EfficiencyCorrectionType`. For further scale factor related specifications
please refer to the following sections.

The default settings are as follows:

.. list-table:: 
   :header-rows: 1
   :widths: 20 20

   * - property name
     - default value

   * - EfficiencyCorrectionType
     - SFJetID

-----
JetID
-----

As already given in the example above, the property ``EfficiencyCorrectionType``
needs to be configured like this::

  TauEffTool.setProperty("EfficiencyCorrectionType", (int)SFJetID);

There are a couple of working points provided for jet ID scale factors

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

E.g. these can be accessed via::

  TauEffTool.setProperty("IDLevel", JETIDBDTLOOSE);

To change the default behaviour call::

  TauEffTool.setProperty("UseInclusiveEta", false);

which will switch to eta inclusive scale factors. Call::

  TauEffTool.setProperty("UseIDExclusiveSF", false);

to get exclusive ID (i.e. fail loose, loose not medium, medium not tight and
tight) scale factors. Call::

  TauEffTool.setProperty("UsePtBinnedSF", false);

in order to access pt binned scale factors. Call::

  SFTool->SwitchOnHighPtUncert(true);

in order to inflate uncertainty for pT > 100 GeV. For details please refer to
these `slides
<https://indico.cern.ch/event/304094/contribution/2/material/slides/0.pdf>`_.

The default setup is:

+------------------+-----------------------------------------------------------------------------------+
| property name    | default value                                                                     |
+------------------+-----------------------------------------------------------------------------------+
| InputFilePath    | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/JetIDSF.root" |
+------------------+-----------------------------------------------------------------------------------+
| VarNameBase      | "TauScaleFactorJetID"                                                             |
+------------------+-----------------------------------------------------------------------------------+
| IDLevel          | JETIDBDTTIGHT                                                                     |
+------------------+-----------------------------------------------------------------------------------+
| UseIDExclusiveSF | false                                                                             |
+------------------+-----------------------------------------------------------------------------------+
| UseInclusiveEta  | false                                                                             |
+------------------+-----------------------------------------------------------------------------------+
| UsePtBinnedSF    | false                                                                             |
+------------------+-----------------------------------------------------------------------------------+
| UseHighPtUncert  | false                                                                             |
+------------------+-----------------------------------------------------------------------------------+

---------
ContJetID
---------

The property ``EfficiencyCorrectionType`` for accessing continuous ID scale factor
needs to be configured like this::

  TauEffTool.setProperty("EfficiencyCorrectionType", (int)SFContJetID);

To obtain eta inclusive scale factors set property ``UseInclusiveEta`` to
``true``, via::

  TauEffTool.setProperty("UseInclusiveEta", true);

To switch between different types of systematic uncertainties call for example::

  TauEffTool.setProperty("ContSysType", QCD);

The following types of systematic uncertainties are available::

* TOTAL
* QCD
* WJETS
* TES
* REMAIN
  
The default setup is:

+------------------+---------------------------------------------------------------------------------------+
| property name    | default value                                                                         |
+------------------+---------------------------------------------------------------------------------------+
| InputFilePath    | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/ContJetIDSF.root" |
+------------------+---------------------------------------------------------------------------------------+
| FileName         | "EfficiencyCorrections/ContJetIDSF.root"                                              |
+------------------+---------------------------------------------------------------------------------------+
| VarNameBase      | "TauScaleFactorContJetID"                                                             |
+------------------+---------------------------------------------------------------------------------------+
| UseInclusiveEta  | false                                                                                 |
+------------------+---------------------------------------------------------------------------------------+
| ContSysType      | TOTAL                                                                                 |
+------------------+---------------------------------------------------------------------------------------+
  
-----
EleID
-----

The property ``EfficiencyCorrectionType`` for accessing electron ID scale factor
needs to be configured like this::

  TauEffTool.setProperty("EfficiencyCorrectionType", (int)SFEleID);

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

**NOTE**: Statistical uncertainties are not provided for electron ID scale
  factors. So do not use getEfficiencyScaleFactorStatUnc and
  applyEfficiencyScaleFactorStatUnc functions.

The systematic uncertainty is provided as nominal +- systematic, not relative to
the scale factor, i.e. in your analysis you need to do::

  weight *= scale_factor_unc;

  
The default setup is:

+------------------+-----------------------------------------------------------------------------------+
| property name    | default value                                                                     |
+------------------+-----------------------------------------------------------------------------------+
| InputFilePath    | "TauAnalysisTools/"+ ``SharedFilesVersion`` +"EfficiencyCorrections/EleIDSF.root" |
+------------------+-----------------------------------------------------------------------------------+
| VarNameBase      | "TauScaleFactorEleID"                                                             |
+------------------+-----------------------------------------------------------------------------------+
| IDLevel          | JETIDBDTTIGHT                                                                     |
+------------------+-----------------------------------------------------------------------------------+
| EVLevel          | ELEIDBDTLOOSE                                                                     |
+------------------+-----------------------------------------------------------------------------------+
| OLRLevel         | OLRLOOSEPP                                                                        |
+------------------+-----------------------------------------------------------------------------------+

----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `TauSelectionTool <README-TauSelectionTool.rst>`_
  * `TauSmearingTool <README-TauSmearingTool.rst>`_
  * `TauEfficiencyCorrectionsTool <README-TauEfficiencyCorrectionsTool.rst>`_
  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_
