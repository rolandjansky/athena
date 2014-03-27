/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage MboyEDMToolInterfaces Package

@author samusog@cern.ch

@section IntroductionMboyEDMToolInterfaces Introduction

MboyEDMToolInterfaces is a package of the Muonboy package.
It contains the Muonboy tools dealing with the EDM

@section OverviewMboyEDMToolInterfaces Class Overview

The MboyEDMToolInterfaces package contains the following class categories:

  - IAdjustableT0Tool 
    This interface class is for tool allowing to adjust the To shift

  - Trk::IMuonboyToTrackTool
    This interface class is for tool allowing to convert  Muonboy Tracks to Trk::Track

  - Trk::IMuonboyToParticleTool
    This interface class is for tool allowing to convert  Muonboy Tracks to Rec::TrackParticle

  - Trk::IMuonboyToSegmentTool
    This interface class is for tool allowing to convert  Muonboy segments to Trk::Segment

  - IMboyRTTool
    This interface class is for tool to manage connection to RT services. 

  - IMboyGetPrepDataFromEventTool
    This interface class is for tool to extract PrepRawData vectors
  
  - IMboyPrepDataToDigiStoreTool  
    This interface class is for tool to fill MuonRecDigiStoreAth with PrepRawData vectors

  - IMboyGenTrackTool 
    This interface class is for tool to extract generated track collection

  - The classes IMboyDumpTrackParticleContainerTool 
    IMboyDumpTrackCollectionTool and
    IMboyDumpSegmentCollectionTool are interface classes for dumpers of the contents of 
    Trk::Track , Trk::Segment , Rec::TrackParticle
    and Analysis::Muon collections

  - IMboyBFTool 
    This interface class is for tool to manage connection to B Field service 
 
  - IMboyEDMHelper 
    This interface class is  for a set of utility methods

  - IMboyNativeHelper 
    This interface class is  for a set of utility methods on MboyCoreEvt objects

  - IMuonboyToTrackParametersTool 
    This interface class is  for tool returning a TrackParameters for a MbPOT object

  - IMuonboyToSurfaceTool 
    This interface class is  for tool returning a surface for a MbIOObject object

  - IMuonboyToIdentifier 
    This interface class is  for tool returning a Identifier for a MbIOObject object

@section ExtrasMboyEDMToolInterfaces Extra Pages

  - @ref used_MboyEDMToolInterfaces
  - @ref requirements_MboyEDMToolInterfaces

*/

/**
@page used_MboyEDMToolInterfaces Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MboyEDMToolInterfaces Requirements
@include requirements
*/
