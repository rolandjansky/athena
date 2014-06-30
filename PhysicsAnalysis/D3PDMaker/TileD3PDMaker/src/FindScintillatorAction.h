/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   FindScintillatorAction.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: GeoVolumeAction to retrieve global transformation coordinates for scintillators.
 *
 * Created in February 2013.
 */

#ifndef FindScintillatorAction_H
#define FindScintillatorAction_H

/// CALORIMETER INCLUDES
#include "CaloEvent/CaloCell.h"

/// UTILITIES
#include "Scintillator.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

/// GEOMODEL INCLUDES
#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVolumeAction.h"

/// CLHEP INCLUDES
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"

/// C++ INCLUDES
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>

/// FORWARD DECLARATIONS
using HepGeom::Transform3D;
using HepGeom::Translate3D;
using CLHEP::Hep3Vector;
using CLHEP::mm;

// #include <iostream>
using namespace std;

//==================================================
class FindScintillatorAction:public GeoVolumeAction{
//==================================================
    public:
        // CONSTRUCTOR AND DESTRUCTOR
        FindScintillatorAction(const CaloCell* cell, std::vector<const Scintillator*>& scintillators);
        ~FindScintillatorAction();

        // HANDLER
        void handleVPhysVol(const GeoVPhysVol* pv);

    private:
        // DEBUGGING METHODS
        void dump(const Transform3D trans);
        void dump(const GeoVPhysVol* pv);
        void dump(const CaloCell* cell);

        // UTILTY METHODS
        bool check(double a, double A, double dA);
        double phidiff(double phi1, double phi2);

        // DATA MEMBERS
        const CaloCell* m_caloCell;
        std::vector<const Scintillator*>& m_scint;
};

#endif //TrackTools_H
