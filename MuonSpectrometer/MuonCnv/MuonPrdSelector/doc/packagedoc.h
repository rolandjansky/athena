/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonPrdSelector_page MuonPrdSelector Package
@author Zdenko.van.Kesteren@cern.ch
@author Andrew Meade (ameade@physics.umass.edu)                                                                            
@section MuonPrdSelector_MuonPrdSelectorIntro Introduction
                                                                                
Contains jobOptions to read and reconstruct cosmic data (real data and simulation)


@section MuonPrdSelector_MuonPrdSelector Class Overview
The MuonPrdSelector package is an Athena algorithm filtering Muon::MuonPrepDataCollection. The program writes a new collection for each old collection and sellects only the events that pass all cuts.  The cuts are defined by MuonIdCutTool, and can include cuts on technology, station name or region, eta/phi station numbers, and or more specific cuts.  The downstream reconstruction programmes do not need to be adjusted to run over the skimmed set of MuonPrepDataCollections since the jobOptions change the storeGate name of the original Containers. The original name of the input Containers is assigned for the outputContainers.

- MuonPrdSelectorAlg: The algorithm retrieves the unfiltered Muon::MuonPrepDataContainer, loops over each event in each of the Muon::MuonPrepDataCollections and stores the selected hits in new Muon::MuonPrepDataCollections into another Muon::MuonPrepDataContainer.

- MuonIdCutTool: This tool's main function: bool IsCut(Identifier).  False signifies all cuts are passed.  In principle any cut based on Identifier could be implimented.  The tool is overloaded to accept a MuonFixedId as well.

@section MuonPrdSelector_MuonPrdSelector Usage Information

To use this package, you must insert the following lines in top options:
    if doPrdSelect:
        include("MuonPrdSelector/MuonPrdSelector_jobOptions.py")

These lines should go just before: include( "CscClusterization/CscThresholdClusterizationOptions.py" )

For most top options this is done in MuonRec_jobOptions.py in MuonRecExample.  Add a flag doPrdSelect = true to your topOptions. If you want to cut truth information, you must download and alter MuonCalibAlg.cxx to use MuonIdCut to write out only the truth hits that pass the cuts set by the tool.



                                                                                
                                                                                
*/
