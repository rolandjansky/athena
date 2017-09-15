/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigT1CTP_page TrigT1CTP Package

@author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
@author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>

@section TrigT1CTP_TrigT1CTPIntro Introduction

This package contains the simulation of the Central Trigger Processor (CTP).
It gets inputs from the MuCTPI and the CALO sub-system and calculates if the
%LVL1 trigger accept (L1A) for a given trigger menu. In addition certain internal
triggers are simulated: random trigger, prescaled clock trigger and bunch group trigger.

@section TrigT1CTP_TrigT1CTPOverview Class Overview
  The TrigT1CTP package contains of following algorithms:

  - LVL1CTP::CTPSimulation: simulation of the CTP hardware
  - LVL1CTP::CBNTAA_CTP_RDO: creates CBNTAA for CTP RDO
  - LVL1CTP::CBNTAA_ReadCTP_RIO: creates CBNTAA for CTP RIO (header information)

  The LVL1CTP::CTPSimulation algorithm uses a list of helper classes:
  LVL1CTP::ResultBuilder,
  LVL1CTP::CTPTriggerThreshold, LVL1CTP::CTPTriggerItem, LVL1CTP::CTPTriggerItemNode,
  LVL1CTP::ItemMap, LVL1CTP::ThresholdMap, LVL1CTP::EventInfo, LVL1CTP::CTPUtil.
  See the class description for details.

  The internal trigger simulation uses the following classes:
  - LVL1CTP::ISpecialTrigger: interface definition
  - LVL1CTP::SpecialTrigger: common code and data implementation
  - LVL1CTP::BunchGroupTrigger: simulation of the internal bunch group trigger
  - LVL1CTP::PrescaledClockTrigger: simulation of the internal prescaled clock trigger
  - LVL1CTP::RandomTrigger: simulation of the internal random trigger

@section TrigT1CTP_TrigT1CTPJobOptions Job Options
  The algorithms of the TrigT1CTP package can be configured using configurables.
  For an example see TrigT1CTP_jobOptions.py:
  @include TrigT1CTP_jobOptions.py

  Configuration classes for the LVL1CTP::CTPSimulation algorithm are:
  - python::TrigT1CTPConfig::DefaultCTPSimulation: common setup (do not use directly)
  - python::TrigT1CTPConfig::CTPSimulation: configuration for running in a reconstruciton job (rerunning %LVL1)
  - python::TrigT1CTPConfig::CTPSimulationInDigi: configuration for running in a digitzation job

  The list of properties for each algorithm can be found here:
  - @link LVL1CTP::CTPSimulation::CTPSimulation @endlink
  - @link LVL1CTP::CBNTAA_CTP_RDO::CBNTAA_CTP_RDO @endlink
  - @link LVL1CTP::CBNTAA_ReadCTP_RIO::CBNTAA_ReadCTP_RIO @endlink

@section TrigT1CTP_TrigT1CTP Extra Pages

*/
