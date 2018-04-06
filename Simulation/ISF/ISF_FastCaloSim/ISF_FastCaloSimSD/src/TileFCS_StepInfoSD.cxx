/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileFCS_StepInfoSD
// Sensitive detector for TileCal G4 simulations with TileGeoModel
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
// Major updates: July, 2013 (Sergey)
//
//************************************************************

// class header
#include "TileFCS_StepInfoSD.h"
/// Geant4 headers
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"

/// Athena headers
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "TileG4Interfaces/ITileCalculator.h"

TileFCS_StepInfoSD::TileFCS_StepInfoSD(G4String name, const FCS_Param::Config & config)
  : FCS_StepInfoSD(name, config)
  , m_calculator(config.m_TileCalculator)
{
}

TileFCS_StepInfoSD::~TileFCS_StepInfoSD() {
}

G4bool TileFCS_StepInfoSD::ProcessHits(G4Step* a_step, G4TouchableHistory* /*ROhist*/) {
  G4bool result(false);
  // If there's no energy, there's no hit.  (Aside: Isn't this energy
  // the same as the energy from the calculator?  Not necessarily.
  // The calculator may include detector effects such as
  // charge-collection which are not modeled by Geant4.)
  if(a_step->GetTotalEnergyDeposit() <= 0.) { return result; }

  if (m_calculator) {
    //calculation of MicroHit with a_step
    TileHitData hitData;
    TileMicroHit micHit = m_calculator->GetTileMicroHit(a_step, hitData);
    Identifier invalid_id;

    //Check if MicroHit is not in scintillator
    if ((micHit.pmt_up == invalid_id) && (micHit.pmt_down == invalid_id)) {
      G4cout <<this->GetName()<<" WARNING ProcessHits: Invalid hit in Tile??"<<G4endl;
      return result;
    }
    else {
      // Some cells is the gap scintillator are only read out by one
      // PMT, so only print warnings if this is not the case.
      if ((micHit.pmt_up == invalid_id) && !(cellReadOutByOnePMT(micHit.pmt_down))) {
        G4cout <<this->GetName()<<" WARNING ProcessHits: Something wrong in identifier: tile pmt_up: "<<micHit.pmt_up<<std::endl;
        G4cout <<this->GetName()<<" WARNING ProcessHits: tile pmt_down : "<<micHit.pmt_down<<" "<<m_tileID->to_string(micHit.pmt_down,-1)<<std::endl;
        G4cout <<this->GetName()<<" WARNING ProcessHits: E up: "<<micHit.e_up<<" E down: "<<micHit.e_down<<" T up: "<<micHit.time_up<<" T down: "<<micHit.time_down<<std::endl;
      }
      if ((micHit.pmt_down == invalid_id) && !(cellReadOutByOnePMT(micHit.pmt_up))) {
        G4cout <<this->GetName()<<" WARNING ProcessHits: Something wrong in identifier: tile pmt_down: "<<micHit.pmt_down<<std::endl;
        G4cout <<this->GetName()<<" WARNING ProcessHits: tile pmt_up: "<<micHit.pmt_up<<" "<<m_tileID->to_string(micHit.pmt_up,-1)<<std::endl;
        G4cout <<this->GetName()<<" WARNING ProcessHits: E up: "<<micHit.e_up<<" E down: "<<micHit.e_down<<" T up: "<<micHit.time_up<<" T down: "<<micHit.time_down<<std::endl;
      }
      // Store TileHits Information
      const G4ThreeVector pos = 0.5*(a_step->GetPreStepPoint()->GetPosition()+a_step->GetPostStepPoint()->GetPosition());
      const int numberOfProcessedHits(1);
      const double timeWindow(m_config.m_maxTimeTile);
      const double distanceWindow(m_config.m_maxRadiusTile);
      this->update_map(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true, numberOfProcessedHits, timeWindow, distanceWindow);
      this->update_map(pos, micHit.pmt_down, micHit.e_down,micHit.time_down, true, numberOfProcessedHits, timeWindow, distanceWindow);
    }
  }
  return true;
}
