/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage MboyCoreEvt Package

@author samusog@cern.ch

@section IntroductionMboyCoreEvt Introduction

MboyCoreEvt is a package of the Muonboy suite.
It contains the Muonboy evt data classes.

@section OverviewMboyCoreEvt Class Overview

The MboyCoreEvt package contains the following class categories:


  - MbIOObject is a base class, from which derives
    MuonRecDigit and 
    MbPOT classes.
    It contains basic informations of the nature of the actual object

  - MuonRecDigit is a base class, deriving from MbIOObject, to store digit informations (analog to PRDs)
    from which derive the technology dependent classes:
    MuonRecDigitMDT , 
    MuonRecDigitRPC ,
    MuonRecDigitTGC ,
    MuonRecDigitCSC and
    MuonRecDigitCSCClu .
    MuonRecDigit objects collections are managed by class MuonRecDigiStore

  - MbPOT is a class, deriving from MbIOObject, to carry a point and direction along 
    a segment or a track. From MbPOT derives MbROT and MbHole classes
 
  - MbROT is the base class to store digit-rec informations (analog to ROTs)
    from which derive the technology dependent classes:
    MbROTMDT ,and
    MbROTRPC ,
    MbROTTGC ,
    MbROTCSC 
    MbROTCSCclu .
    MbROT objects collections are managed by class MbIOObjectStore

  - MbHole is the base class to store holes informations 
    from which derive the technology dependent classes:
    MbHoleMDT ,and
    MbHoleRPC ,
    MbHoleTGC ,
    MbHoleCSC 
    MbHoleCSCclu .
    MbHole objects collections are managed by class MbIOObjectStore

  - MbIDTag is the base class to store digit  identifiers
    from which derive the technology dependent classes:
    MbIDTagMDT ,and
    MbIDTagRPC ,
    MbIDTagTGC ,
    MbIDTagCSC 
    MbIDTagCSCclu. From each of the latter classes derive the corresponding technology depedent classes
    derived from MuonRecDigit, MbROT and MbHole

   
  - MB_Segment  is storing data of a segments.
    MB_Segment objects collections are managed by class MB_SegmentStore

  - MB_Track_Base is the base class to manage track data from which derive:
    MB_Track_CaloEntr ,
    MB_Track_Perigee and
    MB_Track_SpecEntr. 
    The latter classes are associated in the class MB_Track.
    MB_Track objects collections are managed by class MB_TrackStore

  - MboyAline and MboyBline are storing A and B lines data.
    Collections of MboyAline and MboyBline are managed by MboyAlineStore and MboyBlineStore

  - MboyGenTrack stores data of a gen track
    Collections of MboyGenTrack are managed by MboyGenTrackStore

  - MuonboyRTAbstract is the base class to connect to RT services

  - MuonboyRTAdjustableT0 is a base class,deriving from MuonboyRTAbstract, 
    allowing to adjust a T0 shift
   
  - MuonboyBFAbstract is the base class to connect to B Field services

  - MboyVarByName is a class to associate name and value 
    of a set of a configuration variables

  - MboyPosDigiChoice is an utility class to specify choice on treatement of
    positions associated to digits

  - MuonboyMuScPlane is for storing data on crossing of MS/Eloss points

  - MuonboyInterfaceAbstract is is the base class interfacing Muonboy reconstruction

@section ExtrasMboyCoreEvt Extra Pages

  - @ref used_MboyCoreEvt
  - @ref requirements_MboyCoreEvt

*/

/**
@page used_MboyCoreEvt Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_MboyCoreEvt Requirements
@include requirements
*/
