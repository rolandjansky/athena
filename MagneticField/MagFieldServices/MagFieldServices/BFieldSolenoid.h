/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldSolenoid.h
//
// Magnetic field map for the ATLAS solenoid
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDSOLENOID_H
#define BFIELDSOLENOID_H

#include <vector>
#include <iostream>
#include "TFile.h"
#include "MagFieldElements/BFieldZone.h"


class BFieldSolenoid {
public:
    // constructor
    BFieldSolenoid() : m_orig(nullptr), m_tilt(nullptr) {;}
    // destructor
    ~BFieldSolenoid() { delete m_orig; if (m_orig!=m_tilt) delete m_tilt; }
    // read/write map from/to file
    int readMap( std::istream& input );
    int readMap( TFile* rootfile );
    void writeMap( TFile* rootfile, bool tilted = false );
    // move and tilt the map
    void moveMap( double dx, double dy, double dz, double ax, double ay );
    // compute magnetic field
    void getB( const double *xyz, double *B, double *deriv=nullptr ) const;
    // accessor
    const BFieldMesh<double> *tiltedMap() const { return m_tilt; }
private:
    // data members
    BFieldMesh<double> *m_orig; // original map as it was read from file
    BFieldMesh<double> *m_tilt; // tilted and moved map
};

#endif
