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

#ifndef MinBiasScintillatorSD_H
#define MinBiasScintillatorSD_H 

#include <vector>

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "TileSimEvent/TileHitVector.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

using namespace FADS;

class G4Step;
class G4HCofThisEvent;
class StoreGateSvc;
//class MinBiasScintillatorDescription;
class TileTBID;
//class AthenaHitsCollectionHelper;

class MinBiasScintillatorSD : public FadsSensitiveDetector
{
public:
  MinBiasScintillatorSD(G4String);
  ~MinBiasScintillatorSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }
private:
  TileHitVector* m_hitsCollection;
  AthenaHitsCollectionHelper m_hitCollHelper;

  //MinBiasScintillatorDescription* detectorDescription;
  StoreGateSvc* m_sgSvc;
  StoreGateSvc* m_detStore;
  mutable Athena::MsgStreamMember m_msg;

  const TileTBID* m_tileTBID;
  std::string m_containerName;
  bool  m_iamowner;

  static const int nSide = 2;
  static const int nPhi  = 8;
  static const int nEta  = 2;
  static const int nCell = nSide*nPhi*nEta;
  static const int nDist = nSide*nEta;

  inline int cell_index(int side, int phi, int eta) const { return (side*nPhi+phi)*nEta+eta; }
  inline int dist_index(int side, int eta) const { return side*nEta+eta; }
  
  int m_nhits[nCell];
  TileSimHit* m_hit[nCell];
  Identifier m_id[nCell];

  /** @brief granularity in time for hits
   */
  double m_deltaT;
  std::vector<double> m_deltaTvec;

  /** @brief max allowed time for hits
   */
  double m_timeCut;

  /** @brief set to true to apply Birks' law
   */
  bool   m_doBirk;

  /** @brief true if time of flight correction is applied
      for particles coming from ATLAS center
   */
  bool   m_doTOFCorr;

  /** @brief time for hits which are above m_timeCut threshold
      it is equal to m_tilesize_deltaT - m_deltaT
   */
  double m_lateHitTime;

  double m_deltaT_cell[nDist];

  G4double BirkLaw(const G4Step* aStep) const;

  /** @brief function to provide correct deltaT bin 
      width for given time
  */

  inline double deltaT(double time) const {
    unsigned int i=0;
    double delta = m_deltaTvec[i++];
    while (i<m_deltaTvec.size()) {
      if (time>m_deltaTvec[i++] && time<m_deltaTvec[i++]) break;
      delta = m_deltaTvec[i++];
    }
    return delta;
  }
};

#endif
