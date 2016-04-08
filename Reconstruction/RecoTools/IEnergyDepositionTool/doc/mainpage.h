/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage IEnergyDepositionTool Package

This package contains a simple interface for tools that measure
energy depositions in the calorimeter around a track.

@author David.Lopez@cern.ch

@section IntroductionIEnergyDepositionTool Introduction

Measuring energy depositions around a track has several applications.
This package contains an interface to perform this measurement.
The interface is simplified as much as possible to make it easy for
different existing tools to adjust to it.

@section OverviewIEnergyDepositionTool Class Overview
This package contains a single interface IAlgTool:
  - <b> IEnergyDepositionTool<\b>. Simple interface to measure
  energy depositions in the calorimeter around an input track.

@section ExtrasIEnergyDepositionTool Extra Pages

 - @ref UsedIEnergyDepositionTool
 - @ref RequirementsIEnergyDepositionTool
*/

/**
@page UsedIEnergyDepositionTool Used Packages
@htmlinclude used_packages.html
*/

/**
@page RequirementsIEnergyDepositionTool Requirements
@include requirements
*/
