/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigT1RoIB_page TrigT1RoIB Package

@author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
@author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>

@section TrigT1RoIB_TrigT1RoIBIntro Introduction

This package contains the simulation of the RoIBuilder running between LVL1
and LVL2 producing the RoIBResult. In addition it contains helper 
algorithms to access the ROIB::RoIBResult and ROIB::RecRoIBResult objects.

@section TrigT1RoIB_TrigT1RoIBOverview Class Overview
  The TrigT1RoIB package contains of following classes:

  - ROIB::RoIBuilder: simulation of the RoIBuilder process between LVL1 and LVL2
  - TrigT1RoIBConfig::RoIBuilder: algorithm configuration for running in a reconstruction job (rerunning LVL1)
  - TrigT1RoIBConfig::RoIBuilderInDigi: algorithm configuration for running in a digitization job

@section TrigT1RoIB_TrigT1RoIBJobOptions Job Options
  The algorithms of the TrigT1RoIB package can be configured using configurables.
  For an example see TrigT1RoIBJobOptions.py:
  @include TrigT1RoIBJobOptions.py

  Configuration classes for the ROIB::RoIBuilder algorithm are:
  - TrigT1RoIBConfig::DefaultRoIBuilder: common setup (do not use directly)
  - TrigT1RoIBConfig::RoIBuilder: configuration for running in a reconstruciton job (rerunning LVL1)
  - TrigT1RoIBConfig::RoIBuilderInDigi: configuration for running in a digitzation job

  The list of properties for each algorithm can be found here:
  - @link ROIB::RoIBuilder::RoIBuilder @endlink

@section TrigT1RoIB_TrigT1RoIB Extra Pages

*/
