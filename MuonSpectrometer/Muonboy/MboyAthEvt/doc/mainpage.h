/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage MboyAthEvt Package

@author samusog@cern.ch

@section IntroductionMboyAthEvt Introduction

MboyAthEvt is a package of the Muonboy suite.

It contains Muonboy evt data classes.

These classes are built on the classes of MboyCoreEvt 
package adding Athena specificities

@section OverviewMboyAthEvt Class Overview

The MboyAthEvt package contains the following class categories:

  - MB_SegmentAth is managing the segment data. It derives from
    MB_Segment to which are added connections to PrepData. Collection
    of MB_SegmentAth are managed by MB_SegmentStoreAth. MB_SegmentStoreTDS
    allows to store a MB_SegmentStoreAth in TDS
    
  - MB_TrackAth is managing the track data. It derives from
    MB_TrackA to which are added connections to PrepData. Collection
    of MB_TrackAth are managed by MB_TrackStoreAth. MB_TrackStoreTDS
    allows to store a MB_TrackStoreAth in TDS

  - MuonRecDigiStoreAth is managing the digit data. It derives from
    MuonRecDigiStore to which are added connections to PrepData. 
    MuonRecDigiStoreTDS allows to store a MuonRecDigiStoreAth in TDS

@section ExtrasMboyAthEvt Extra Pages

  - @ref used_MboyAthEvt
  - @ref requirements_MboyAthEvt

*/

/**
@page used_MboyAthEvt Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MboyAthEvt Requirements
@include requirements
*/
