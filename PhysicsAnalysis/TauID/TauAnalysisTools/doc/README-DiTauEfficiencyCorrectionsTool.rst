==============================
DiTauEfficiencyCorrectionsTool
==============================

:authors: David Kirchmeier
:contact: david.kirchmeier@cern.ch

.. contents:: Table of contents

-------
Preface
-------

**NOTE:** To use this tool it is necessary that the decoration
``IsTruthHadronic`` is available for each boosted di-tau candidate. 
This information is decorated by `DiTauTruthMatchingTool <README-DiTauTruthMatchingTool.rst>`_, which should run before DiTauEfficiencyCorrectionsTool.

------------
Introduction
------------

This tool aims to provide nominal and systematically varied efficiency scale
factors for boosted di-tau reconstruction and identification. 

For the tool the following line needs to be added to include the header file::

  #include "TauAnalysisTools/DiTauEfficiencyCorrectionsTool.h"

The tool is in general created and initialized by::

  TauAnalysisTools::DiTauEfficiencyCorrectionsTool DiTauEffTool( "DiTauEfficiencyCorrectionsTool" );
  DiTauEffTool.initialize();

Scale factors can be decorated to the boosted di-tau or retrieved from the tool by::
  
  DiTauEffTool.applyEfficiencyScaleFactor(xDiTau);                                     // either directly appending scale factors to the xAOD di-tau auxiliary store
  DiTauEffTool.getEfficiencyScaleFactor(xDiTau, dEfficiencyScaleFactor);               // or storing fake factors in variable dEfficiencyScaleFactor

The variable names for the scale factors have default values, but can be
configured. For information on this please refer to the `section Available
properties <README-DiTauEfficiencyCorrectionsTool.rst#available-properties>`_
below.

This configures the set of ``EfficiencyCorrectionTypes`` and if needed the jet
(electron) ID working points, depending on the applied cuts.  Please note, that
the tool is only able to see the configuration of the passed TauSelectionTool,
any selection applied outside the passed tool (e.g. harder jet ID, ID control
regions etc...) will not be taken into account.

A set of recommended systematic variations can in general be retrieved by
calling::

  CP::SystematicSet recommendedSystematicSet = TauEffTool.recommendedSystematics();

which gives at the moment only the systematics for boosted di-tau identification.

--------------------
Available properties
--------------------

Overview
========

The tool can be used to retrieve scale factors for a specific
``RecommendationTag``. For the default ``RecommendationTag`` "2017-moriond" the following properties are available for tool steering:

.. list-table::
   :header-rows: 1
   :widths: 25 10 55

   * - property name
     - type
     - default value
 
   * - ``EfficiencyCorrectionTypes``
     - ``std::vector<int>``
     - ``{SFJetIDHadTau}``

   * - ``InputFilePathJetIDHadTau``
     - ``std::string``
     - ``"TauAnalysisTools/"+ <SharedFilesVersion> +"EfficiencyCorrections/JetID_TrueHadDiTau_2017-prerec.root"``

   * - ``VarNameJetIDHadTau``
     - ``std::string``
     - ``"DiTauScaleFactorJetIDHadTau"``

Details
=======

EfficiencyCorrectionTypes
-------------------------

The following enum for the property
``EfficiencyCorrectionTypes`` can be used to obtain the corresponding scale
factor:

* SFJetIDHadTau: scale factors for boosted di-tau jet identification of true hadronic tau decays

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

   * - ``JETIDNONE``
     - only reconstruction uncertainties are applied

   * - ``JETIDBDTVERYLOOSE``
     - corresponding to a cut on jet BDT score > 0.60

   * - ``JETIDBDTLOOSE``
     - corresponding to a cut on jet BDT score > 0.65
     
   * - ``JETIDBDTMEDIUM``
     - corresponding to a cut on jet BDT score > 0.72

   * - ``JETIDBDTTIGHT``
     - corresponding to a cut on jet BDT score > 0.77

These can be accessed, for example via::

  TauEffTool.setProperty("IDLevel", (int)JETIDBDTLOOSE);


Scale Factor Extraction
=========================

This is minimal code example on how to extract scale factors (including systematic variations) for a list of recommended systematics::

  for (auto sSystematicSet: DiTauEffTool->recommendedSystematics())
  {
    ANA_CHECK( DiTauEffTool->applySystematicVariation(sSystematicSet));
    ANA_CHECK( DiTauEffTool->applyEfficiencyScaleFactor(*xDiTau) );
    Info( "execute()",
          "SystType %s: JetIDSF: %g",
          sSystematicSet.name().c_str(),
          xDiTau->auxdata< double >( "DiTauScaleFactorJetIDHadTau" ));
  }


----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `DiTauTruthMatchingTool <README-DiTauTruthMatchingTool.rst>`_
  * `DiTauSmearingTool <README-DiTauSmearingTool.rst>`_
  * `DiTauEfficiencyCorrectionsTool <README-DiTauEfficiencyCorrectionsTool.rst>`_

