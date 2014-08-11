/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage TrigRootAnalysis - Tools for trigger-aware D3PD analysis

   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   @section TrigRootAnalysisIntro Introduction

   This package holds some code meant to help with analyzing the trigger
   information in D3PD files. It's main use is to decode the full trigger
   decision, and trigger configuration information found in many D3PD
   types.

   @section TrigRootAnalysisUsage Usage instructions

   A TWiki page with some usage instructions can be found
   <a href="https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TriggerD3PDMaker#ROOT_helper_code_for_reading_som">here</a>.

   The package can be compiled in 3 different ways:
     - In an Athena environment with CMT
     - In a ROOT environment using RootCore
     - In a ROOT environment in a completely standalone way

   Examples demonstrating the usage of D3PD::TrigDecisionToolD3PD can be found
   under <strong>/afs/cern.ch/user/k/krasznaa/public/roottdt/</strong>.

   @htmlinclude used_packages.html

   @include requirements
*/
