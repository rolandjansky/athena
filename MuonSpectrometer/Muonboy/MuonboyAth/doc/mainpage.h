/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage MuonboyAth Package

@author samusog@cern.ch

@section IntroductionMuonboyAth Introduction

MuonboyAth is a package of the Muonboy suite.
It forms a user interface to the MboySvc service which performs
the Muon reconstruction in the Muon Spectrometer.

@section OverviewMuonboyAth Class Overview

The MuonboyAth package contains:

  - MboyDigiEmptyLoop is a top algo the only function of which is
    to loop on PrepRawaData containers before the reconstruction
    it self,in order to not count the corresponding time in the
    reconstruction time
     
  - MboyRec is the top algorithm performing the Muon reconstruction
    in Muon Spectrometer
  
  - MboyChecks is performing dumps of the collections of 
    MboyCoreEvt objects

  - MboySegmentCheck is performing dumps of the collections of 
    MboyCoreEvt object for segments

@section ExtrasMuonboyAth Extra Pages

  - @ref used_MuonboyAth
  - @ref requirements_MuonboyAth

*/

/**
@page used_MuonboyAth Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MuonboyAth Requirements
@include requirements
*/
