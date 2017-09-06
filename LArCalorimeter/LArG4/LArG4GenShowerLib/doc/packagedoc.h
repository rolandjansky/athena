/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page LArG4GenShowerLib_page LArG4GenShowerLib Package

@author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
@author Alexandre Glazov <alexandre.glazov@desy.de>

@section LArG4GenShowerLib_LArG4GenShowerLibIntro Introduction

This package contains the algorithms to generate the Geant 4 based
shower parametrisation libraries. The generation process is a two step
process. First, full Geant 4 simulation is run with single
particles. The information about the hits in the sensitive calorimeter
detectors is store using the ShowerLib::StepInfo class. In a second
step the collected hits are clusterd and collected into a frozen
shower library using the ShowerLib::ShowerLib class.

@section LArG4GenShowerLib_LArG4GenShowerLibOverview Class Overview
  The LArG4GenShowerLib package contains of following algorithms:

  - LArG4GenShowerLib: clustering and collecting hits into shower library
  - TestActionShowerLib: collecting of hit information in full Geant 4 simulation

  The data structure of the the shower library is contained in the
  LArG4ShowerLib package. The entry point is the ShowerLib::ShowerLib class.
  The hit information collected by the test action is contained in the
  LArG4ShowerLib package. The entry point is the
  ShowerLib::StepInfoCollection class.
  
@section LArG4GenShowerLib_LArG4GenShowerLibJobOptions Job Options
  For both algorithms dedicated job options are supplied which are used by the script
  generateShowerLib.py.

  - LArG4GenShowerLib_generateHits_topoptions.py: configuration of single particle gun for hit generation
  - LArG4GenShowerLib_testActionShowerLib_joboptions.py: configuration of test action for hit generation
  - LArG4GenShowerLib_combine_topoptions.py: configuration of hit clustering and output options
  - LArG4GenShowerLib_setupCalo_joboptions.py: configure calorimeter
  - LArG4GenShowerLib_setup_joboptions.py: configure general LArG4GenShowerLib algorithm properties

  The list of properties for each algorithm can be found here:
  - @link LArG4GenShowerLib::LArG4GenShowerLib @endlink

@section LArG4GenShowerLib_LArG4GenShowerLibScripts Scripts
  The whole shower library generation process can be done in one step using the
  generateShowerLib.py script. For the options see the help function of the script.


@section LArG4GenShowerLib_LArG4GenShowerLibDefaultSettings Default Settings
  
  The default settings of the library generation process are stored in
  @include generate_libs.txt.

@section LArG4GenShowerLib_LArG4GenShowerLib Extra Pages

*/
