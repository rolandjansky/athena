===============
TauSmearingTool
===============

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

------------
Introduction
------------

This tool mainly provides corrections to scale your tau pT up or down (in MC)
according to the TES uncertainties (which are analysis independent). The
momentum of xAOD taus have already been corrected for proper tau energy scale,
and therefore the tool returns for simulation a correction factor of 1. In data
the tau pT is shifted and needs to be corrected.

The tool can in general be used, including the header file::

  #include "TauAnalysisTools/TauSmearingTool.h"

and calling::

  TauAnalysisTools::TauSmearingTool TauSmeTool( "TauSmaringTool" );
  TauSmeTool.initialize();

  TauSmeTool.applyCorrection(*tau);

The set of recommended systematic variations can in general be retrieved by
TauAnalysisTools calling::

  CP::SystematicSet recommendedSystematicSet = TauSmeTool.recommendedSystematics();

A complete set of available (including the recommended) systematic variations
TauAnalysisTools are retrieved via::

  CP::SystematicSet affectingSystematicSet = TauSmeTool.affectingSystematics();

The tool can be configured to use a specific set of systematic variations calling::

  TauSmeTool.applySystematicVariation(customSystematicSet);

If you like to access a specific component of the TES uncertainty, i.e.
statistical uncertainty of the in-situ measurement, you need to create a
systematic set and add a systematic variation with the name TAUS_SME_INSITUSTAT
and an integer corresponding to the up/downward variation (a possitive number
corresponds to upward, a negative to downward variation). So for an upward 1
sigma variation one would write::

  CP::SystematicSet sSystematicSet;
  sSystematicSet.insert(CP::SystematicVariation("TAUS_SME_INSITUSTAT", 1));
  TauSmeTool.applySystematicVariation( sSystematicSet );

**However, you should get in contact with TauWG first before doing this in your analysis!**

New nuisance parameters are provided for single TES components::

  * FINAL: "old style" total TES uncertainty
  * TOTAL: total TES uncertainty w/ constraints from in-situ measurement at low
    pt (pt < 50 GeV), i.e. sqrt(MODELING**2 + CLOSURE**2 + INSITUINTERPOL**2 +
    SINGLEPARTICLE**2)
  * INSITU: total in-site component, i.e. sqrt(INSITUSYS**2 + INSITUSTAT**2)   
    **NOTE: no interpolation is applied here; if you want to apply interpolation take INSITUINTERPOL**
  * INSITUINTERPOL: total in-situ component with pt interpolation according to 
    sqrt(1 - (pt -50)/20) * INSITU for 50 GeV < pt < 70 GeV; above pt > 70 GeV
    the interpolation factor is 0, while for pt < 50 GeV it is 1
  * INSITUSTAT/INSITUSYST: statistical and systematic component of in-situ
    measurement
  * SINGLEPARTICLEINTERPOL: single particle response interpolated as
    "switch-on", i.e. (1 - sqrt(1 - (pt -50)/20)) * SINGLEPARTICLE * MODELING:
    modelling component
  
The following table summarizes the possible properties available to configure
the tool:

.. list-table::
   :header-rows: 1
   :widths: 10 20
   
   * - property name
     - default value
     
   * - IsData
     - false

   * - InputFilePath
     - "TauAnalysisTools/"+ ``SharedFilesVersion`` +"/Smearing/mc12_p1344_medium.root"
     
   * - TESComponent
     - FINAL

----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `TauSelectionTool <README-TauSelectionTool.rst>`_
  * `TauSmearingTool <README-TauSmearingTool.rst>`_
  * `TauEfficiencyCorrectionsTool <README-TauEfficiencyCorrectionsTool.rst>`_
  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_

