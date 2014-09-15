/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage MboyEDMTool Package

@author samusog@cern.ch

@section IntroductionMboyEDMTool Introduction

MboyEDMTool is a package of the Muonboy package.
It contains the Muonboy tools dealing with the EDM

@section OverviewMboyEDMTool Class Overview

The MboyEDMTool package contains the following class categories:

  - MboyRTTool is the tool mananging connection to RT services. 
    The connection is implemented in a class derived from MuonboyRTAbstract. 
    This implementation could be MuonboyRTMdtDriftCircleOnTrackCreator (for MdtDriftCircleOnTrackCreator)
    or MuonboyRTAdjustableT0SvcRt 

  - MuonboyRTAdjustableT0SvcRt 
    This class derives from MuonboyRTAdjustableT0 and permit to manipulate RT relations by playing on T0 shift thanks
    to a AdjustableT0Tool

  - AdjustableT0Tool 
    This class derives from IAdjustableT0Tool and allows to adjust the To shift using a AdjustableT0Map to store the T0 shift by station
    
  - MboyBFTool is the tool mananging connection to B Field service 
    The connection is implemented in the class MuonboyBFAth
    derived from MuonboyBFAbstract
  
  - Trk::MuonboyToTrackTool , Trk::MuonboyToSegmentTool and MuonboyToParticleTool 
    are tools to convert  Muonboy Tracks
    and segments to Trk::Track , Trk::Segment , Rec::TrackParticle
    and Analysis::Muon objects  

  - The classes MboyDumpTrackParticleContainerTool 
    MboyDumpTrackCollectionTool and
    MboyDumpSegmentCollectionTool are dumpers on the contents of 
    Trk::Track , Trk::Segment , Rec::TrackParticle
    and Analysis::Muon collections

  - MboyGetPrepDataFromEventTool is extracting PrepRawData vectors

  - MboyPrepDataToDigiStoreTool  is filling a MuonRecDigiStoreAth with PrepRawData

  - MboyGenTrackTool is extracting generated track collection
 
  - MboyEDMHelper is collecting number of utility methods
    on MboyAthEvt and EDM objects

  - MboyNativeHelper is collecting number of utility methods 
    on MboyCoreEvt objects

  - MuonboyToTrackParametersTool 
    This tool returns a TrackParameters for a MbPOT object

  - MuonboyToSurfaceTool 
    This tool returns surface for a MbIOObject object

  - MuonboyToIdentifier 
    This tool Identifier for a MbIOObject object

@section ExtrasMboyEDMTool Extra Pages

  - @ref used_MboyEDMTool
  - @ref requirements_MboyEDMTool

*/

/**
@page used_MboyEDMTool Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MboyEDMTool Requirements
@include requirements
*/
