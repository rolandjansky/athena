/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MinBiasScintillatorSD
// Sensitive detector for the Minimum Bias Scintillator
//
// Author: A. Solodkov
// April 01, 2006.
//
//************************************************************

#ifndef MINBIASSCINTILLATOR_MINBIASSCINTILLATORSD_H
#define MINBIASSCINTILLATOR_MINBIASSCINTILLATORSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// For the hits
#include "TileSimEvent/TileHitVector.h"
#include "StoreGate/WriteHandle.h"

#include "MinBiasScintSDOptions.h"

// STL headers
#include <vector>

// G4 needed classes
class G4Step;
class G4TouchableHistory;

class TileTBID;

class MinBiasScintillatorSD: public G4VSensitiveDetector {
  public:
    MinBiasScintillatorSD(const G4String& name, const std::string& hitCollectionName,
        const MinBiasScintSDOptions& opts);
    ~MinBiasScintillatorSD();

    virtual void Initialize(G4HCofThisEvent*) override final;

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

    void EndOfAthenaEvent();

  private:
    // Options for the SD configuration
    const MinBiasScintSDOptions m_options;

    // The hits collection
    SG::WriteHandle<TileHitVector> m_HitColl;

    const TileTBID* m_tileTBID;

    static const int N_SIDE = 2;
    static const int N_PHI = 8;
    static const int N_ETA = 2;
    static const int N_CELLS = N_SIDE * N_PHI * N_ETA;
    static const int N_DIST = N_SIDE * N_ETA;

    inline int cell_index(int side, int phi, int eta) const {
      return (side * N_PHI + phi) * N_ETA + eta;
    }
    inline int dist_index(int side, int eta) const {
      return side * N_ETA + eta;
    }

    /** @brief number of contributions to the energy in each cell
     (size=nCell).
     */
    std::vector<int> m_numberOfHitsInCell;

    /** @brief local temporary TileSimHit for each cell (size=nCell).
     */
    std::vector<TileSimHit*> m_tempSimHit;

    /** @brief array to cache look-ups of Identifiers for each cell
     (size=nCell).
     */
    Identifier m_channelID[N_CELLS];

    /** @brief granularity in time for hits
     */
    double m_deltaT;
    std::vector<double> m_deltaTvec;

    /** @brief max allowed time for hits
     */
    double m_timeCut;

    /** @brief time for hits which are above m_timeCut threshold
     it is equal to m_tilesize_deltaT - m_deltaT
     */
    double m_lateHitTime;

    double m_deltaT_cell[N_DIST];

    G4double BirkLaw(const G4Step* aStep) const;

    /** @brief function to provide correct deltaT bin
     width for given time
     */

    inline double deltaT(double time) const {
      unsigned int i = 0;
      double delta = m_deltaTvec[i++];
      while (i < m_deltaTvec.size()) {
        if (time > m_deltaTvec[i++] && time < m_deltaTvec[i++]) break;
        delta = m_deltaTvec[i++];
      }
      return delta;
    }
};

#endif //MINBIASSCINTILLATOR_MINBIASSCINTILLATORSD_H
