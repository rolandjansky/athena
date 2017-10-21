================
TauSelectionTool
================

:authors: Dirk Duschinger, David Kirchmeier
:contact: dirk.duschinger@cern.ch, david.kirchmeier@cern.ch

.. contents:: Table of contents

------------
Introduction
------------


This tool intends to perform simple selections on a set of tau properties. By
default a set of recommended cuts are applied. To use it you first need to
include the corresponding header file::

  #include "TauAnalysisTools/TauSelectionTool.h"
  
The tool at least needs to be created and initialized like::

  TauAnalysisTools::TauSelectionTool TauSelTool( "TauSelectionTool" );
  TauSelTool->initialize();
  
This creates the tool with the recommended cuts, which are defined in the
default config file
/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData/TauAnalysisTools/00-00-30/Selection/recommended_selection_mc15.conf
(or in newer versions).

------------------
Release Check
------------------

**Note:** this release check is not needed in 21 releases anymore. The properties which are discussed below have no effect anymore and will be removed in one of the next updates of TauAnalysisTools. 

Since TauAnalysisTools-00-02-43 an automatic release check is implemented. 
For samples which are not AODFix (before 20.7.8.5, excluding 20.7.8.2) the electron overlap removal is re-run. For AODFix samples (20.7.8.5 and above)
the available electron overlap removal is used. One can turn off this release 
check via::
   
   TauSelTool.setProperty("IgnoreAODFixCheck", true);

In that case one can force the electron overlap-removal to be 
re-calculated via::
  
   TauSelTool.setProperty("RecalcEleOLR", true);

**Note:** that in case one wants to use the electron overlap-removal `EleOLR` in
 not AOD fixed samples it
 is recommended to run the `TauOverlappingElectronLLHDecorator
 <README-TauOverlappingElectronLLHDecorator.rst>`_ before calling the *accept*
 function of the TauSelectionTool. However, if this is not done, the tool uses
 the TauOverlappingElectronLLHDecorator internally to decorate the tau with the
 electron likelihood score. In this case it is recommended to call
 intializeEvent() in each new event, otherwise the electron container will be
 retrieved for each tau, which is overhead and much slower.

------------------
Tool configuration
------------------

The default config file looks like this::

  SelectionCuts: PtMin AbsEtaRegion AbsCharge NTracks JetIDWP EleOLR

  PtMin: 20
  AbsEtaRegion: 0; 1.37; 1.52; 2.5
  AbsCharge: 1
  NTracks: 1; 3
  JetIDWP: JETIDBDTMEDIUM
  EleOLR: TRUE

The top line lists the cuts to be applied. Below are the configurations on the
cuts, like the pt threshold of 20 GeV. If there is a cut specified, e.g. `PtMin:
20` but `PtMin` is not listed in `SelectionCuts`, the cut will not be made.

Entries with semicolons are treated as list of values, which only makes sense
for setups of vector type (e.g. ``AbsEtaRegion`` or ``NTracks``).

It is possible to use a hashtag (``#``) as the first character of the line, to
ignore the line.

The following table gives an overview of all currently available cuts and their
setup:

.. list-table:: 
   :header-rows: 1
   :widths: 5 5 5 55 30
   
   * - Cut
     - Cut setup
     - Type
     - Description
     - Note
     
   * - ``CutPt``
     - ``PtRegion``
     - ``std::vector<double>``
     - accepting taus within pt regions (in GeV), each `odd` in the vector is a lower bound, each `even` is an upper bound
     -
     
   * -
     - ``PtMin``
     - ``double``
     - accepting taus with a pt above a lower bound (in GeV)
     - if ``PtMin`` is configured, ``PtRegion`` configuration wont be considered

   * -
     - ``PtMax``
     - ``double``
     - accepting taus with a pt below an upper bound (in GeV)
     - if ``PtMax`` is configured, ``PtRegion`` configuration wont be considered

   * - ``CutAbsEta``
     - ``AbsEtaRegion``
     - ``std::vector<double>``
     - accepting taus within absolute eta regions, each `odd` in the vector is a lower bound, each `even` is an upper bound
     -
     
   * -
     - ``AbsEtaMin``
     - ``double``
     - accepting taus with an absolute eta above a lower bound
     - if ``AbsEtaMin`` is configured, ``AbsEtaRegion`` configuration wont be considered

   * -
     - ``AbsEtaMax``
     - ``double``
     - accepting taus with an absolute eta below an upper bound
     - if ``AbsEtaMax`` is configured, ``AbsEtaRegion`` configuration wont be considered

   * - ``CutAbsCharge``
     - ``AbsCharges``
     - ``std::vector<int>``
     - accepting taus with a set of absolute charges, each value in the vector will be accepted
     - 

   * - 
     - ``AbsCharge``
     - ``double``
     - accepting taus with the given absolute charge
     - if ``AbsCharge`` is configured, ``AbsCharges`` configuration wont be considered

   * - ``CutNTracks``
     - ``NTracks``
     - ``std::vector<size_t>``
     - accepting taus with a set of track multiplicities, each value in the vector will be accepted
     -

   * -
     - ``NTrack``
     - ``double``
     - accepting taus with the given track multiplicity
     - if ``NTrack`` is configured, ``NTracks`` configuration wont be considered

   * - ``CutJetBDTScore``
     - ``JetBDTRegion``
     - ``std::vector<double>``
     - accepting taus within jet BDT score regions, each `odd` in the vector is a lower bound, each `even` is an upper bound
     -

   * -
     - ``JetBDTMin``
     - ``double``
     - accepting taus with a jet BDT score above a lower bound
     - if ``JetBDTMin`` is configured, ``JetBDTRegion`` configuration wont be considered

   * - 
     - ``JetBDTMax``
     - ``double``
     - accepting taus with a jet BDT score below an upper bound
     - if ``JetBDTMax`` is configured, ``JetBDTRegion`` configuration wont be considered

   * - ``CutJetIDWP``
     - ``JetIDWP``
     - ``int``
     - accepting taus passing the given working point
     -

   * - ``CutEleBDTScore``
     - ``EleBDTRegion``
     - ``std::vector<double>``
     - accepting taus within electron BDT score regions, each `odd` in the vector is a lower bound, each `even` is an upper bound
     - 

   * -
     - ``EleBDTMin``
     - ``double``
     - accepting taus with a electron BDT score above a lower bound
     - if ``EleBDTMin`` is configured, ``EleBDTRegion`` configuration wont be considered

   * -
     - ``EleBDTMax``
     - ``double``
     - accepting taus with a electron BDT score below an upper bound
     - if ``EleBDTMax`` is configured, ``EleBDTRegion`` configuration wont be considered

   * - ``CutEleBDTWP``
     - ``EleBDTWP``
     - ``int``
     - accepting taus passing the given working point
     - 

   * - ``CutEleOLR``
     - ``EleOLR``
     - ``bool``
     - if ``EleOLR == true``, accepting taus not overlapping with a good reconstructed electron
     - should only be used for run 2 analysis

   * - ``CutMuonVeto``
     - ``MuonVeto``
     - ``bool``
     - if ``MuonVeto == true``, accepting taus passing the muon veto
     - should only be used for run 1 analysis

   * - ``CutMuonOLR``
     - ``MuonOLR``
     - ``bool``
     - if ``MuonOLR == true``, removing tau overlapped with muon satisfying pt>2GeV and not calo-tagged
     - should only be used for run 2 analysis

Currently implemented working points for ``CutJetIDWP`` are:

.. list-table::
   :header-rows: 1

   * - Jet ID working points
     - description
     
   * - JETIDNONE
     - no cut at all
     
   * - JETIDBDTLOOSE
     - passing BDT loose working point
     
   * - JETIDBDTMEDIUM
     - passing BDT medium working point
     
   * - JETIDBDTTIGHT
     - passing BDT tight working point
     
   * - JETIDBDTLOOSENOTTIGHT
     - passing BDT loose but not BDT tight working point
     
   * - JETIDBDTLOOSENOTMEDIUM
     - passing BDT loose but not BDT medium working point
     
   * - JETIDBDTMEDIUMNOTTIGHT
     - passing BDT medium but not BDT tight working point
     
   * - JETIDBDTNOTLOOSE
     - not passing BDT loose working point

   * - JETIDBDTVERYLOOSE
     - passing BDT very loose working point, new since release 21

   * - JETBDTBKGLOOSE
     - loose background working point, new since release 21

   * - JETBDTBKGMEDIUM
     - medium background working point, new since release 21

   * - JETBDTBKGTIGHT
     - tight background working point, new since release 21

and for ``CutEleBDTWP``:

.. list-table::
   :header-rows: 1

   * - Electron veto working points
     - description
     
   * - ELEIDBDTLOOSE
     - BDT loose electron veto 
     
   * - ELEIDBDTMEDIUM
     - BDT medium electron veto 
     

If one wants to use a different setup one has three options:

1. Using an own config file
===========================

One needs to create a new file like the recommended_selection_mc15.conf and
modify it as needed. You then have to tell the tool where it can find your
configuration file via::

  TauSelTool.setProperty( "ConfigPath", "/PATH/TO/CONFIG/FILE"); 

**IMPORTANT:** the last line of the file needs to be an empty line or should
only contain a comment (starting with the number sign #). Otherwise the tool
might be not properly configured (the last line is ignored by the file parser).

2. Overwrite particular cut setups or the list of cuts to be executed
=====================================================================

If particular cuts are modified, e.g. if one wants to select only taus above pT
> 100 GeV one would do::

  TauSelTool.setProperty("PtMin", 100.);

only the property will be overwritten, but all other cuts in the config file
will be applied as they are defined in the file.

Notes:

#. If one wants to specify the list of cuts to be applied, one can set the
   property ``SelectionCuts`` to a combination of enums defined in
   `TauSelectionTool.h <../TauAnalysisTools/TauSelectionTool.h>`_, which need to
   be casted to int, e.g.::

     TauSelTool.setProperty("SelectionCuts", int(TauAnalysisTools::CutPt |
                                                 TauAnalysisTools::CutAbsEta |
                                                 TauAnalysisTools::CutNTracks);

#. If one wants to use a different working point, e.g. for ``CutJetIDWP`` one
   needs to pass an enum, defined in `Enums.h <../TauAnalysisTools/Enums.h>`_,
   which need to be casted to int, e.g.::

     TauSelTool.setProperty("JetIDWP", int(TauAnalysisTools::JETIDBDTTIGHT));

#. Vector based variables need to get a vector of the correct type. I.e. to
   achieve the same configuration as in the config file::
     
     AbsEtaRegion: 0; 1.37; 1.52; 2.5

   one needs the following code lines::

     std::vector<double> vAbsEtaRegion = {0, 1.37, 1.52, 2.5};
     TauSelTool.setProperty("AbsEtaRegion", vAbsEtaRegion);
   
3. Don't load any config file
=============================

If the property ``ConfigPath`` is set to an empty string::

  TauSelTool.setProperty( "ConfigPath", "");

no config file will be loaded. In this case, if no other properties are
configured, the tool will accept any tau. However, configuration can be achieved
as described in the `previous section
<README-TauSelectionTool.rst#overwrite-particular-cut-setups-or-the-list-of-cuts-to-be-executed>`_.

Other tool configurations for the electron overlap removal
==========================================================

The electron overlap removal uses pT and eta (of the leading track) dependent
thresholds stored in a root file. The input file can be set via::

  TauSelTool.setProperty("EleOLRFilePath", "PATH/TO/FILE");

Currently the following official input files are available in
``/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData``:

.. list-table::
   :header-rows: 1

   * - file
     - description

   * - TauAnalysisTools/<latest tag>/Selection/eveto_cutvals.root
     - 95% signal efficiency, loose working point
       
   * - TauAnalysisTools/<latest tag>/Selection/eveto_cutvals_90.root
     - 90% signal efficiency, medium working point

   * - TauAnalysisTools/<latest tag>/Selection/eveto_cutvals_85.root
     - 85% signal efficiency, tight working point

Use at least ``00-01-09`` for the ``<latest tag>``.
     
The electron overlap removal further depends on the electron container. If for
some reason the electron container name differs from the default
(``Electrons``), just change the property ``ElectronContainerName`` to the
relevant container name::

  TauSelTool.setProperty("ElectronContainerName", "MY/ELECTRON/CONTAINER/NAME");
     
----------------
Tool application
----------------

To test if a tau has passed all selection requirements just ask::

  TauSelTool.accept(xTau);

where xTau needs to be of type ``xAOD::TauJet`` or ``xAOD::IParticle*``. The
function returns a Root::TAccept value, equivalent to ``true``, in case all cuts
defined in the property ``"SelectionCuts"`` are passed, and equivalent to
``false`` otherwise. I.e. most users might make use of the following line in
their analyses::

  if (TauSelTool.accept(xTau))
  {
    // do stuff with accepted taus
    // ...
  }


------------------
Control histograms
------------------
     
This tool has the ability to create control histograms (currently it work not in
EventLoop). Therefore the `option` "CreateControlPlots" must be set to true::
     
  TauSelTool.setProperty("CreateControlPlots", true );

Also the tool needs to know where to write the histograms which is configured by
passing a pointer to the output file::

  TauSelTool.setOutFile( fOutputFile );

After all wanted selections have been made the histograms are written to the
file, via::

  TauSelTool.writeControlHistograms();
  
This adds a folder to the output file named by concatenating the tool name with
the prefix "_control". This folder contains a cutflow histogram showing the
number of processed tau objects before all cuts, and after each applied
cut. Additional control distributions before and after after all cuts are
stored in this folder.

---
FAQ
---

#. **Question:** How can I explicitly not perform a specific cut?

   **Answer:** This can be done by removing the cut name in the line starting
   with *SelectionCuts*.

#. **Question:** How can I find out, whether I correctly configured the tool and
   which cuts will be applied?

   **Answer:** If the tool is initialized with DEBUG message level
   (``TauSelTool->msg().setLevel( MSG::DEBUG );``) you will see for example such
   an output::

    TauSelectionTool          DEBUG Pt: 20 to inf
    TauSelectionTool          DEBUG AbsEta: 0 to 1.37
    TauSelectionTool          DEBUG AbsEta: 1.52 to 2.5
    TauSelectionTool          DEBUG AbsCharge: 1
    TauSelectionTool          DEBUG NTrack: 1
    TauSelectionTool          DEBUG NTrack: 3
    TauSelectionTool          DEBUG BDTJetScore: -inf to inf
    TauSelectionTool          DEBUG BDTEleScore: -inf to inf
    TauSelectionTool          DEBUG JetIDWP: JETIDNONE
    TauSelectionTool          DEBUG EleBDTDWP: ELEIDNONE
    TauSelectionTool          DEBUG EleOLR: 1
    TauSelectionTool          DEBUG MuonVeto: 0
    TauSelectionTool          DEBUG cuts: Pt AbsEta AbsCharge NTrack JetIDWP EleOLR

   **Note:** only the cuts in the last line will be processed


#. **Question:** How can I use different working points for the electron overlap
   removal.

   **Answer:** This is described in this `section <README-TauSelectionTool.rst#other-tool-configurations-for-the-electron-overlap-removal>`_

#. **Question**: After cutting on EleOLR there are still taus with rather large
   likelihood scores.

   **Answer**: These are most probably 3 prong taus which are skipped by the
   electron overlap removal cut.

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
