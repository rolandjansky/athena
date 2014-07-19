========================================================
TauAnalysisTools: Package hosting tools for tau analysis
========================================================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. meta::
   :description: TauAnalysisTools: Package hosting tools for tau analysis
   :keywords: TauAnalysisTools, tau, maddog

.. contents:: Table of contents

------------
Introduction
------------

This package is designed to provide dual-use tools for easy use of taus in your
analysis. Currently following tools are available:

* **TauSelectionTool:** generic tool to apply a set of requirements on tau
    candidates
* **TauSmearingTool:** currently support tau energy corrections
* **TauEfficiencyCorrectionTool:** currently provides jet identification scale
    factors and the associated uncertainties

All relevant information about the actual measurement of these uncertainties can
be found here: `TauRecommendationsWinterConf2013
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauRecommendationsWinterConf2013>`_.
These numbers are mostly valid for 2012 data analysis using reprocessed data,
i.e. p1443 (p1344/p1345).

Most of the functionality of TauEfficiencyCorrectionsTool and TauSmearingToolwas
copied from former `TauCorrUncert Tool
<https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauCorrUncert>`_.

-----
Setup
-----

First start with a clean shell and setup rootcore via::

  export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
  alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
  setupATLAS

  rcSetup Base,2.X.X
  rc find_packages

and comile with::

  rc compile

or directly with::

  rc compile_pkg TauAnalysisTools

---------------
General Remarks
---------------

For each tool the message level can be adjusted like::

  TauSelTool.msg().setLevel( MSG::VERBOSE );

----------------
TauSelectionTool
----------------

This tool intends to perform simple selections on a set of tau properties. It
also offers a common set of recommended cuts for a quick start. The
TauSelectionTool can be for example created by::

  TauAnalysisTools::TauSelectionTool TauSelTool( "TauSelectorTool" );
  // define eta regions, excluding crack region
  std::vector<double> vAbsEtaRegion = {0, 1.37, 1.52, 2.5};
  TauSelTool.SetProperty("AbsEtaRegion", vAbsEtaRegion);
  // define pt threshold, note that pt has to be given in GeV
  TauSelTool.SetProperty("PtMin", 20);
  // define absolute charge requirement, in general should be set to 1
  TauSelTool.SetProperty("AbsCharge", 1);
  // define number of tracks required, most analysis use 1 and 3 track taus
  std::vector<int> vNTracks = {1,3};
  TauSelTool.SetProperty( "NTracks", vNTrack);
  // requiring tau to pass a jet BDT working point
  TauSelTool.SetProperty( "JetIDWP", TauAnalysisTools::JetBDTTight);
  // and finally define cuts actually to be executed:
  TauSelTool.SetProperty( "SelectionCuts",
                          TauAnalysisTools::CutPt |
                          TauAnalysisTools::CutAbsEta |
			  TauAnalysisTools::CutAbsCharge |
			  TauAnalysisTools::CutNTrack |
			  TauAnalysisTools::CutJetIDWP);
  TauSelTool.initialize();

To test if a tau has passed all selection requirements just ask::

  TauSelTool.accept(*tau);

which returns ``true``, in case all cuts defined in the property ``"Selection
Cuts"`` are passed, and ``false`` otherwise.

If the user wants to use recommended cuts, he should setup the tool via::

  TauAnalysisTools::TauSelectionTool TauSelTool( "TauSelectorTool" );
  TauSelTool.setRecommendedProperties(); 
  TauSelTool.initialize();

The following table gives an overview of all currently available cuts and their setup:

.. list-table:: 
   :header-rows: 1
   :widths: 10 10 80
   
   * - Cut
     - Cut setup
     - Description

   * - CutPt
     - PtRegion
     - accepting taus within pt regions, each `odd` in the vector is a lower bound, each `even` is an upper bound

   * -
     - PtMin
     - accepting taus with a pt above a lower bound

   * -
     - PtMax
     - accepting taus with a pt below an upper bound

   * - CutAbsEta
     - AbsEtaRegion
     - accepting taus within absolute eta regions, each `odd` in the vector is a lower bound, each `even` is an upper bound
     
   * -
     - AbsEtaMin
     - accepting taus with an absolute eta above a lower bound

   * -
     - AbsEtaMax
     - accepting taus with an absolute eta below an upper bound

   * - CutAbsCharge
     - AbsCharges
     - accepting taus with a set of absolute charges, each value in the vector will be accepted

   * -
     - AbsCharge
     - accepting taus with the given absolute charge

   * - CutNTracks
     - NTracks
     - accepting taus with a set of track multiplicities, each value in the vector will be accepted

   * -
     - NTrack
     - accepting taus with the given track multiplicity

   * - CutJetBDTRegion
     - JetBDTRegion
     - accepting taus within jet BDT score regions, each `odd` in the vector is a lower bound, each `even` is an upper bound

   * -
     - JetBDTMin
     - accepting taus with a jet BDT score above a lower bound

   * -
     - JetBDTMax
     - accepting taus with a jet BDT score below an upper bound

   * - CutJetIDWP
     - JetIDWP
     - accepting taus passing the given working point

   * - CutEleBDTRegion
     - EleBDTRegion
     - accepting taus within ele BDT score regions, each `odd` in the vector is a lower bound, each `even` is an upper bound

   * -
     - EleBDTMin
     - accepting taus with a ele BDT score above a lower bound

   * -
     - EleBDTMax
     - accepting taus with a ele BDT score below an upper bound

   * - CutEleBDTWP
     - EleBDTWP
     - accepting taus passing the given working point

Currently implemented working points for ``CutJetIDWP`` are:

.. list-table::
   :header-rows: 1

   * - Jet ID working points
     - description
     
   * - JetBDTNONE
     - not passing BDT loose working point
     
   * - JetBDTLoose
     - passing BDT loose working point
     
   * - JetBDTMedium
     - passing BDT medium working point
     
   * - JetBDTTight
     - passing BDT tight working point
     
   * - JetBDTLooseNotTight
     - passing BDT loose but not BDT tight working point
     
   * - JetBDTLooseNotMedium
     - passing BDT loose but not BDT medium working point
     
   * - JetBDTMediumNotTight
     - passing BDT medium but not BDT tight working point

and for ``CutEleBDTWP``:

.. list-table::
   :header-rows: 1

   * - Electron Veto working points
     - description
     
   * - EVETOBDTLOOSE
     -
     
   * - EVETOBDTMEDIUM
     -
     
   * - EVETOBDTTIGHT
     -


---------------
TauSmearingTool
---------------

This tool provide mainly corrections to scale your tau pT up or down (in MC)
according to the TES uncertainties (which are analysis independent). The
momentum of xAOD taus have already been corrected for proper tau energy scale,
and therefore the tool returns for simulation a correction factor of 1. In data
the tau pT is shifted and needs to be corrected.

The tool can in general be used calling::

   TauAnalysisTools::TauSmearingTool TauSmeTool( "TauSmaringTool" ); // setting
   direction for systematics (only effective for simulation)
   TauSmeTool.setProperty("Direction", 1);
   TauSmeTool.initialize();

   TauSmeTool.applyCorrection(*tau);

If you like to access a specific component of the TES uncertainty, i.e.
statistical uncertainty of the in-situ measurement you need to call before
initializing::

    TauSmeTool.setProperty( "TESComponent", = FINAL );

**However, you should get in contact with TauWG first before doing this in your analysis!**

New nuisance parameters are provided for single TES components::

  * FINAL: "old style" total TES uncertainty
  * TOTAL: total TES uncertainty w/ constraints from in-situ measurement at low
    pt (pt < 50 GeV), i.e. sqrt(MODELING**2 + CLOSURE**2 + INSITUINTERPOL**2 +
    SINGLEPARTICLE**2)
  * INSITU: total in-site component, i.e. sqrt(INSITUSYS**2 + INSITUSTAT**2)   
    **NOTE: no interpolation is applied here; if you want to apply interpolation take INSITUINTERPOL**
  * INSITUINTERPOL: total in-situ component with pt interpolation accoording to 
    sqrt(1 - (pt -50)/20) * INSITU for 50 GeV < pt < 70 GeV; above pt > 70 GeV
    the interpolation factor is 0, while for pt < 50 GeV it is 1
  * INSITUSTAT/INSITUSYST: statistical and systematic component of in-situ
    measurement
  * SINGLEPARTICLEINTERPOL: single particle response interpolated as
    "switch-on", i.e. (1 - sqrt(1 - (pt -50)/20)) * SINGLEPARTICLE * MODELING:
    modeling component
  
---------------------------
TauEfficiencyCorrectionTool
---------------------------

This tool aims to provide efficiency scale factors and the according statistical
and systatic uncertainties for tau identification and electron veto,
i.e. recommended scale factors labeled `JetID` (binned in pt), preliminary scale
factors labeled `ContJetID` (binned in jet BDT score) and scale factors for the
electron veto labeled `EleID` (binned in eta).

The tool is in general initialized, e.g. for JetID scale factors, by::

  TauAnalysisTools::TauEfficiencyCorrectionsTool TauEffTool( "TauEfficiencyCorrectionsTool" );
  TauEffTool.setProperty("EfficiencyCorrectionType", SFJetID)
  TauEffTool.initialize();

  TauEffTool.applyEfficiencyScaleFactor(xTau);                                     // either directly appending scale factors to the xAOD tau auxiliary store
  TauEffTool.getEfficiencyScaleFactor(xTau, dEfficiencyScaleFactor);               // or storing fake factors in variable dEfficiencyScaleFactor
  TauEffTool.applyEfficiencyScaleFactorStatUnc(xTau);                              // either directly appending scale factors statistical uncertainty to the xAOD tau auxiliary store
  TauEffTool.getEfficiencyScaleFactorStatUnc(xTau, dEfficiencyScaleFactorStatUnc); // or storing fake factors statistical uncertainty in variable dEfficiencyScaleFactor
  TauEffTool.applyEfficiencyScaleFactorSysUnc(xTau);                               // either directly appending scale factors systematic uncertainty to the xAOD tau auxiliary store
  TauEffTool.getEfficiencyScaleFactorSysUnc(xTau, dEfficiencyScaleFactorSysUnc);   // or storing fake factors systematic uncertainty in variable dEfficiencyScaleFactor
      
The other scale factors can be retreived by choosing a differnt
`EfficiencyCorrectionType`. For further scale factor related specifications
please refer to the following sections.

The default settings are as follows:

+------------------+---------------+
| property name    | default value |
+------------------+---------------+
| SharePath        | "../share/"   |
+------------------+---------------+
| IDLevel          | SFJetID       |
+------------------+---------------+
| SysDirection     | 1             |
+------------------+---------------+




JetID
-----

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

+------------------+---------------+
| property name    | default value |
+------------------+---------------+
| FileName         | "ID/sf.root"  |
+------------------+---------------+
| UseIDExclusiveSF | false         |
+------------------+---------------+
| UseInclusiveEta  | false         |
+------------------+---------------+
| UsePtBinnedSF    | false         |
+------------------+---------------+
| UseHighPtUncert  | false         |
+------------------+---------------+


ContJetID
---------

information to be added


EleID
-----

information to be added

..  LocalWords:  JETIDNONE
