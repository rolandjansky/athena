/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiPropertiesTool_page SiPropertiesTool Package

@authors Grant Gorfine, Susumu.Oda@cern.ch

@section SiPropertiesTool_SiPropertiesToolIntro Introduction

Tool for providing silicon properties for each detector element.
The properties are available via the SiliconProperties class from
which one can obtain hall and drift mobility, diffusion constant and
number of electron hole pairs per deposited energy.  Some of these
are dependent on temperature and voltages which are obtained via the
appropriate tool and updated via a condition algorithm.

@section SiPropertiesTool_SiPropertiesToolOverview Class Overview

- ISiPropertiesTool: Interface for SiPropertiesTool
- SiPropertiesTool: The main tool for both Pixel and SCT
- InDet::SiliconProperties: Data object to hold various silicon properties
- InDet::SiliconPropertiesVector: Vector of SiliconProperties for condition algorithms and SiPropertiesTool
- PixelSiPropertiesCondAlg: Condition algorithm for Pixel
- SCTSiPropertiesCondAlg: Condition algorithm for SCT
- SCTSiPropertiesTestAlg: Test algorithm for SiPropertiesTool for SCT

@section SiPropertiesTool_SiPropertiesToolConfigurationTest Configuration and test scripts

- PixelSiPropertiesConfig.py: Configuration script for Pixel for the new job configuration
- PixelSiPropertiesToolSetup.py: Configuration script for Pixel for the old job configuration
- SCT_SiPropertiesConfig.py: Configuration script for SCT for the new job configuration
- SCT_SiPropertiesToolSetup.py: Configuration script for SCT for the old job configuration
- testSCTProperties.py: Test script to run SCTSiPropertiesTestAlg

*/
