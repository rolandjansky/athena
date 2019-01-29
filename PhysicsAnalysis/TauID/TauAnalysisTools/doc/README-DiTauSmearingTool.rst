===================
DiTauSmearingTool
===================

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

This tool provides corrections to your ditau pT according to the energy scale 
uncertainties. 

The tool can in general be used, including the header file::

  #include "TauAnalysisTools/DiTauSmearingTool.h"

The tool is created and initialized by calling::

  TauAnalysisTools::DiTauSmearingTool DiTauSmeTool( "DiTauSmaringTool" );
  DiTauSmeTool.initialize();

The smearing can be either applied to `non-const` ditaus::

  DiTauSmeTool.applyCorrection(xDiTau);

or by obtaining a `non-const` copy from a `const` ditau::

  DiTauSmeTool.correctedCopy(xDiTauOrig, xDiTauCopy);

The set of recommended systematic variations can in general be retrieved by
calling::

  CP::SystematicSet recommendedSystematicSet = DiTauSmeTool.recommendedSystematics();

A complete set of available (including the recommended) systematic variations
are retrieved via::

  CP::SystematicSet affectingSystematicSet = DiTauSmeTool.affectingSystematics();

The tool can be configured to use a specific set of systematic variations calling::

  DiTauSmeTool.applySystematicVariation(customSystematicSet);

--------------------
Available properties
--------------------

Overview
========

The tool can be used to apply ditau pt smearing for a specific
``RecommendationTag``:

.. list-table::
   :header-rows: 1
   :widths: 15 10 20 55
      
   * - property name
     - type
     - default value
     - other sensible values

   * - ``RecommendationTag``
     - ``std::string``
     - ``"2019-winter"``
     - ``"2019-winter"``

The following table lists other properties for further configurations:


.. list-table::
   :header-rows: 1
   :widths: 15 10 20 55
      
   * - property name
     - type
     - default value
     - comment

   * - ``SkipTruthMatchCheck``
     - ``bool``
     - ``false``
     - Only for testing purposes. Must not be used in production. 


----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `DiTauTruthMatchingTool <README-DiTauTruthMatchingTool.rst>`_
  * `DiTauSmearingTool <README-DiTauSmearingTool.rst>`_
  * `DiTauEfficiencyCorrectionsTool <README-DiTauEfficiencyCorrectionsTool.rst>`_

