/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page LArG4ShowerLibSvc_page LArG4ShowerLibSvc Package

@author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
@author Alexandre Glazov <alexandre.glazov@desy.de>

@section LArG4ShowerLibSvc_LArG4ShowerLibSvcIntro Introduction

This package contains the service to use Geant 4 based shower
parametrisation in the simulation. At initialisation time the shower
libraries for different particles and calorimeters are read into
memory. At execution time the service can be asked to substiute a
certain particle and a list of energy deposits is returned. These
need a dedicated sensitive detector to be put back into the simulation.

@section LArG4ShowerLibSvc_LArG4ShowerLibSvcOverview Class Overview

  The LArG4ShowerLibSvc package contains of following services:

  - ILArG4ShowerLibSvc: abstract class for interface definition
  - LArG4ShowerLibSvc: service which returns list of energy deposits for a given particle

  The service is used by the fast simulation contained in the LArG4FastSimulation package.
  The entry point is the LArG4FastSimulation algorithm.

  @sa 
  - Shower library data structure: ShowerLib::ShowerLib (package )
  - Dedicated sensitive detectors: IFastSimDedicatedSD and BarrelFastSimDedicatedSD, EndcapFastSimDedicatedSD, FCALFastSimDedicatedSD
  
@section LArG4ShowerLibSvc_LArG4ShowerLibSvcJobOptions Job Options and Configuration

  The service can and should be configured using configurables. The customised code is in 
  LArG4ShowerLibSvcConfig.py. 

  Some example job options are available:
  - LArG4ShowerLibSvc_testread_textfile_joboptions.py - setting up shower libary service using plain format shower libraries from
  default location
  - LArG4ShowerLibSvc_testread_joboptions.py - setting up shower library service using pool format shower libraries

  The list of properties for each service can be found here:
  - @link LArG4ShowerLibSvc::LArG4ShowerLibSvc @endlink

@section LArG4ShowerLibSvc_LArG4ShowerLibSvc Extra Pages

*/
