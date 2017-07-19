/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonGeoModelTest_page The MuonGeoModelTest Package

@section MuonGeoModelTest_IntroMuonGeoModelTest Introduction

This package is a colection of algorithms used for stand-alone tests of the geometry implementation provided by MuonGeoModel.

@section MuonGeoModelTest_MuonGeoModelTestAlgos Algorithm Overview

MuonGMCheck       - can be run in several modes, depending on the setting of internal flags (job-options);
                    most relevant are:
                    
                    check_mdt: loops over all MdtReadoutElements, dump the position of the center of first and last tube of each multilayer (in addition to a few other control quantities)
                    check_csc: loops over all CscReadoutElements, dump the position of the center of first and last strip of each strip plane (in addition to a few other control quantities)
                    check_rpc: loops over all RpcReadoutElements, dump the position of the center of first and last strip of each strip plane (in addition to a few other control quantities)
                    check_tgc: loops over all TgcReadoutElements, dump the position of the center of first and last strip or wire gang of each gas gap (in addition to a few other control quantities)
                    build_MdtRegionSelectorMap: produces MdtRegionSelectorMap
                    build_CscRegionSelectorMap: produces CscRegionSelectorMap
                    build_RpcRegionSelectorMap: produces RpcRegionSelectorMap
                    build_TgcRegionSelectorMap: produces TgcRegionSelectorMap

MuonHitRelocation - runs on SimHit files; gets the local_position of the hit and relocate it back in the global reference frame;
                    produces a ntuple with all hits (identifier fields, local positions, global positions, residuals in phi and
                    z direction of the global positions with respect to a straight line track originating from a not spreaded IP at 0,0,0).
                    Residuals are <0.1microns for samples of geantino's hits generated from a fixed IP.




*/
