
================
TauSelectionTool
================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

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
/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData/TauAnalysisTools/00-00-25/Selection/recommended_selection.conf
(or in newer versions).

Please note, that in case one wants to use the electron overlap-removal `EleOLR`
one needs to call intializeEvent() in each new event, otherwise the electron
container will be retrieved for each tau, which is overhead and much slower.

Tool configuration
------------------

The default config file looks like this::

  SelectionCuts: PtMin AbsEtaRegion AbsCharge NTracks JetIDWP EleBDTWP

  PtMin: 20
  AbsEtaRegion: 0; 1.37; 1.52; 2.5
  AbsCharge: 1
  NTracks: 1; 3
  JetIDWP: JETIDBDTMEDIUM
  EleOLR: TRUE

On the top there are the `SelectionCuts` listed which will be applied. Below are
the configurations on the cuts, like the pt threshold of 20 GeV. For further
information of additional cuts please have a look below. If there is a cut
specified, e.g. `PtMin: 20` but `PtMin` is not listed in `SelectionCuts`, the
cut will not be made.

If one wants to use a different setup one has two options.

The first option is to overwrite the configs, for example via::

  TauAnalysisTools::TauSelectionTool TauSelTool( "TauSelectionTool" );
  // define eta regions, excluding crack region
  std::vector<double> vAbsEtaRegion = {0, 1.37, 1.52, 2.5};
  TauSelTool.setProperty("AbsEtaRegion", vAbsEtaRegion);
  // define pt threshold, note that pt has to be given in GeV
  TauSelTool.setProperty("PtMin", 20.);
  // define absolute charge requirement, in general should be set to 1
  TauSelTool.setProperty("AbsCharge", 1);
  // define number of tracks required, most analysis use 1 and 3 track taus
  std::vector<size_t> vNTracks = {1,3};
  TauSelTool.setProperty( "NTracks", vNTracks);
  // requiring tau to pass a jet BDT working point
  TauSelTool.setProperty( "JetIDWP", int(TauAnalysisTools::JETIDBDTTIGHT));
  // and finally define cuts actually to be executed:
  TauSelTool.setProperty( "SelectionCuts", int(
                          TauAnalysisTools::CutPt |
                          TauAnalysisTools::CutAbsEta |
			  TauAnalysisTools::CutAbsCharge |
			  TauAnalysisTools::CutNTrack |
			  TauAnalysisTools::CutJetIDWP));
  TauSelTool.initialize();

Please note, that due to technical reasons all enums need to be casted to int in
the ``setProperty`` function.

The second option is to create a new file like the recommended_selection.conf
and modify it as needed. You then have to tell the tool very it can find your
configuration file via::

  TauSelTool.setProperty( "ConfigPath", "/PATH/TO/CONFIG/FILE"); 

Note that entries with semicolons are treated as vectors, e.g.::

  AbsEtaRegion: 0; 1.37; 1.52; 2.5

configures the tool the same way as::

  std::vector<double> vAbsEtaRegion = {0, 1.37, 1.52, 2.5};
  TauSelTool.setProperty("AbsEtaRegion", vAbsEtaRegion); 


Tool application
----------------

To test if a tau has passed all selection requirements just ask::

  TauSelTool.accept(*tau);

which returns ``true``, in case all cuts defined in the property
``"SelectionCuts"`` are passed, and ``false`` otherwise.

The following table gives an overview of all currently available cuts and their setup:

.. list-table:: 
   :header-rows: 1
   :widths: 10 10 80
   
   * - Cut
     - Cut setup
     - Description

   * - CutPt
     - PtRegion
     - accepting taus within pt regions (in GeV), each `odd` in the vector is a lower bound, each `even` is an upper bound

   * -
     - PtMin
     - accepting taus with a pt above a lower bound (in GeV)

   * -
     - PtMax
     - accepting taus with a pt below an upper bound (in GeV)

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

   * - CutJetBDT
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

   * - CutEleBDT
     - EleBDTRegion
     - accepting taus within electron BDT score regions, each `odd` in the vector is a lower bound, each `even` is an upper bound

   * -
     - EleBDTMin
     - accepting taus with a electron BDT score above a lower bound

   * -
     - EleBDTMax
     - accepting taus with a electron BDT score below an upper bound

   * - CutEleBDTWP
     - EleBDTWP
     - accepting taus passing the given working point

   * -
     - EleOLR
     - accepting taus not overlapping with a good reconstructed electron

   * - CutMuonVeto
     - -
     - accepting taus passing the muon veto

Currently implemented working points for ``CutJetIDWP`` are:

.. list-table::
   :header-rows: 1

   * - Jet ID working points
     - description
     
   * - JETIDBDTNONE
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

and for ``CutEleBDTWP``:

.. list-table::
   :header-rows: 1

   * - Electron veto working points
     - description
     
   * - ELEIDBDTLOOSE
     - BDT loose electron veto 
     
   * - ELEIDBDTMEDIUM
     - BDT medium electron veto 
     
   * - ELEIDBDTTIGHT
     - BDT tight electron veto 

------------------
control histograms
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

----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `TauSelectionTool <README-TauSelectionTool.rst>`_
  * `TauSmearingTool <README-TauSmearingTool.rst>`_
  * `TauEfficiencyCorrectionsTool <README-TauEfficiencyCorrectionsTool.rst>`_
  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_
