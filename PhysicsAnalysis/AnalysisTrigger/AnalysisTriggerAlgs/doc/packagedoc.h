/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AnalysisTriggerAlgs_page AnalysisTriggerAlgs Package

@author Tadashi.Maeno <Tadashi.Maeno@cern.ch>
@author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
@author Alan Watson <Alan.Watson@cern.ch>
@author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>

@section AnalysisTriggerAlgs_AnalysisTriggerAlgsIntro Introduction

This package contains some algorithms to create Trigger ESD/AOD objects.

@section AnalysisTriggerAlgs_AnalysisTriggerAlgsOverview Class Overview
  The AnalysisTriggerAlgs package contains of following classes:

  - RoIBResultToAOD: convert ROIB::RoIBResult into CTP_Decision and LVL1_ROI.
                     One important feature is the rebuilding of the trigger type.
		     For the configuration see section \ref AnalysisTriggerAlgsJobOptions.

@section AnalysisTriggerAlgs_AnalysisTriggerAlgsJobOptions Job Options
  The algorithms of the AnalysisTriggerAlgs package can be configured using configurables.
  For an example see AnalysisTrigger_jobOptions.py:
  @include AnalysisTrigger_jobOptions.py

  Configuration classes for the RoIBResultToAOD algorithm are:
  - AnalysisTriggerAlgsConfig::DefaultRoIBResultToAOD: common setup (do not use directly)
  - AnalysisTriggerAlgsConfig::RoIBResultToAOD: configuration for running in a reconstruciton job
  - AnalysisTriggerAlgsConfig::RoIBResultToAODLegacy: configuration for running in a reconstruciton job rebuilding the LVL1 trigger type

  The list of properties for each algorithm can be found here:
  - @link RoIBResultToAOD::RoIBResultToAOD @endlink

@section AnalysisTriggerAlgs_AnalysisTriggerAlgs Extra Pages

*/
