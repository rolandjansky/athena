/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigT1Result_page TrigT1Result Package

@section TrigT1Result_TrigT1ResultIntro Introduction

This package contains the %LVL1 RoI, RDO and Result objects 
produced by the RoIBuilder or the different %LVL1 subsystems.
Usually they are written to RDO or BS files.

@section TrigT1Result_TrigT1ResultOverview Class Overview
  The TrigT1Result package contains of following data classes:

  Result classes:

  - ROIB::RoIBResult
  - ROIB::RecRoIBResult
  - ROIB::CTPResult
  - ROIB::MuCTPIResult
  - ROIB::EMTauResult
  - ROIB::JetEnergyResult
  - ROIB::L1TopoResult

  RDO classes:

  - CTP_RDO
  - MuCTPI_RDO

  RoI classes:

  - ROIB::CTPRoI
  - ROIB::MuCTPIRoI
  - ROIB::EMTauRoI
  - ROIB::JetEnergyRoI

  Helper classes:

  - ROIB::Header
  - ROIB::Trailer

  For all classes, except ROIB::RecRoIBResult, a dictionary exist. 
  See @link TrigT1ResultDict.h @endlink and @link selection.xml @endlink for more details.

@section TrigT1Result_TrigT1Result Extra Pages

*/
