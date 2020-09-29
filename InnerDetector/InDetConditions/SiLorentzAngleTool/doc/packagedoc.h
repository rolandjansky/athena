/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiLorentzAngleTool_page SiLorentzAngleTool Package

@authors Grant Gorfine, Susumu.Oda@cern.ch

@section SiLorentzAngleTool_SiLorentzAngleToolIntro Introduction

Tool for providing Lorentz angle (and the corresponding shift of the measurement)
for each detector element. 

@section SiLorentzAngleTool_SiLorentzAngleToolOverview Class Overview

- SiLorentzAngleCondData: Data class to hold Lorentz angle related data
- SiLorentzAngleTool: The main tool. Inherit ISiLorentzAngleTool of InDetCondTools package
- PixelSiLorentzAngleCondAlg: Condition algorithm for Pixel
- SCTSiLorentzAngleCondAlg: Condition algorithm for SCT
- SCTSiLorentzAngleTestAlg: Test algorithm of SiLorentzAngleTool for SCT. Can be run by testSCTLorentzAngle.py  

@section SiLorentzAngleTool_SiLorentzAngleToolConfiguration Configuration

@subsection SiLorentzAngleTool_SiLorentzAngleToolNewJobConfig In the new job configuration
- PixelLorentzAngleConfig.py and SCT_LorentzAngleConfig.py are used to configure SiLorentzAngleTool for Pixel and SCT, respectively.

@subsection SiLorentzAngleTool_SiLorentzAngleToolOldJobConfig In the old job configuration
- PixelLorentzAngleToolSetup.py and SCTLorentzAngleToolSetup.py are used to configure SiLorentzAngleTool for Pixel and SCT, respectively.
- SiLorentzAngleToolConfig.py and SiLorentzAngleToolConfigDb.py can be used to retrieve SiLorentzAngleTool using ConfiguredFactory (https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory). However, PixelLorentzAngleToolSetup.py and SCTLorentzAngleToolSetup.py are necessary to configure condition alogrithms.

*/
