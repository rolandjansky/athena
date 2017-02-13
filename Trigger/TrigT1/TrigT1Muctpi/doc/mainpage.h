/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
  @mainpage TrigT1Muctpi Package

  @author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
  @author Thorsten Wengler <Thorsten.Wengler@cern.ch>

  @section TrigT1MuctpiIntro Introduction

  This package holds the simulation code of the Muon to CTP Interface (MuCTPI).
  This hardware is responsible for collecting the muon candidates reconstructed by
  the RPC and TGC sector logics, count their multiplicity in 6 different thresholds
  considering overlaps between the trigger sectors, and send the multiplicity
  result to the CTP. It also supplies information on the LVL1 muon candidates
  to the RoI Builder and the DAQ system.

  @section TrigT1MuctpiOverview Class Overview

  The package contains the following algorithms:

   - LVL1MUCTPI::L1Muctpi : The main MuCTPI simulation algorithm.
   - CBNTAA_ReadMuCTPI_RDO : Algorithm that saves MuCTPI variables in a CBNTAA ntuple.
   - CBNTAA_ReadMuCTPI_RIO : Algorithm that saves "extended" MuCTPI variables in a CBNTAA ntuple.
   - CBNTAA_MuctpiRoI : Algorithm saving eta-phi coordinates for muon RoIs to CBNTAA ntuple.
   - LVL1MUCTPI::L1MuctpiPatGen : Algorithm generating random input for the MuCTPI simulation. For testing only.
   - LVL1MUCTPI::L1MuctpiTestReadResults : Algorithm "testing" the output of the MuCTPI simulation.

   The actual hardware simulation uses a lot of classes. Those can all be found in the "Classes" tab.

 */

/**
   @page used_TrigT1Muctpi Used Packages
   @htmlinclude used_packages.html
*/

/**
   @page requirements_TrigT1Muctpi Requirements
   @include requirements
*/
