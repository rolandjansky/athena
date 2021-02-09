/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MINBIASSCINTILLATOR_MINBIASSCINTSDOPTIONS_h
#define MINBIASSCINTILLATOR_MINBIASSCINTSDOPTIONS_h

/* This is a very simple struct for holding sensitive detector
 configuration options; formerly this was in
 TileCalorimeter/TileSimUtils/TileSimUtils/TileG4SimOptions.h
 But now that there are real tools, that setup is not necessary
 */

//CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

#include <string>
#include <vector>

struct MinBiasScintSDOptions {
    /** Setup defaults */
    MinBiasScintSDOptions()
        : timeCut(350.5),
          tileTB(false),
          doBirk(true),
          birk1(0.0130 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2)),
          birk2(9.6e-6 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2) * CLHEP::g / (CLHEP::MeV * CLHEP::cm2)),
          doTOFCorrection(true),
          verboseLevel(0),
          rDBAccessSvcName("RDBAccessSvc"),
          geoModelSvcName("GeoModelSvc")
    {
      deltaTHit = {0.5 , -75.25 , 75.25 , 5.};
    }

    /** A time granularity for G4 hits in TileHit
     it can be just one number, e.g. 5.0 which means that the same
     granularity is used everywhere or it can be a vector which contains
     3*N+1 elements, e.g. 0.1,-5,5, 0.5,-75,75, 5.0 which means that
     for [-5,5] ns interval granularity 0.1ns will be used
     for [-75,75] ns interval granularity 0.5ns will be used
     and 5ns granularity will be used for all other hits  */
    std::vector<double> deltaTHit;
    /** Time cut for hits, all hits go to one single time bin if time is above this cut */
    double timeCut;
    /** Flag to indicate TileCal TestBeam configuration */
    bool tileTB;
    /** Flag to enable Birk's law */
    bool doBirk;
    /** Birks Law parameters - exp. values from NIM 80 (1970) 239-244 */
    double birk1;
    double birk2;
    /** Flag to apply TOF correction (subtract Time Of Flight from ATLAS center) */
    bool doTOFCorrection;
    /** Helper to keep the same verbosity everywhere */
    int verboseLevel;
    //** Name of the RDBAccessSvc instance to use */
    std::string rDBAccessSvcName;
    //** Name of the GeoModelSvc instance to use */
    std::string geoModelSvcName;
  };

#endif
