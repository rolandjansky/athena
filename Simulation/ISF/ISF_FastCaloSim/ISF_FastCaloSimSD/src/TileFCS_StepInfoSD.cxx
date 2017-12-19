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
    Identifier m_invalid_id;

    //Check if MicroHit is not in scintillator
    if ((micHit.pmt_up == m_invalid_id) && (micHit.pmt_down == m_invalid_id)) {
      G4cout <<this->GetName()<<" WARNING ProcessHits: Invalid hit in Tile??"<<G4endl;
      return result;
    }
    else {
      // Store TileHits Information
      if ((micHit.pmt_up == m_invalid_id) ||  (micHit.pmt_down == m_invalid_id)) {
        G4cout <<this->GetName()<<" WARNING ProcessHits: Something wrong in identifier: One tile pmt: "<<micHit.pmt_up<<" "<<micHit.pmt_down<<std::endl;
        G4cout <<this->GetName()<<" WARNING ProcessHits: E up: "<<micHit.e_up<<" E down: "<<micHit.e_down<<" T up: "<<micHit.time_up<<" T down: "<<micHit.time_down<<std::endl;
      }
      const G4ThreeVector pos = 0.5*(a_step->GetPreStepPoint()->GetPosition()+a_step->GetPostStepPoint()->GetPosition());

      this->update_map(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true,1);
      this->update_map(pos, micHit.pmt_down, micHit.e_down,micHit.time_down , true,1);
    }
  }
  return true;
}

void TileFCS_StepInfoSD::update_map(const CLHEP::Hep3Vector & l_vec, const Identifier & l_cell, double l_energy, double l_time, bool l_valid, int l_detector)
{
  // Drop any hits that don't have a good identifier attached
  if (!m_calo_dd_man->get_element(l_cell)) { return; }

  auto map_item = m_hit_map.find( l_cell );
  if (map_item==m_hit_map.end()) {
    m_hit_map[l_cell] = new std::vector< ISF_FCS_Parametrization::FCS_StepInfo* >;
    m_hit_map[l_cell]->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_cell , l_energy , l_time , l_valid , l_detector ) );
  }
  else {
    bool match = false;
    for (auto map_it : * map_item->second) {
      // Time check
      const double delta_t = std::fabs(map_it->time()-l_time);
      if ( delta_t >= m_config.m_maxTimeTile ) { continue; }
      // Distance check
      const double hit_diff2 = map_it->position().diff2( l_vec );
      if ( hit_diff2 >= m_config.m_maxRadiusTile ) { continue; }
      // Found a match.  Make a temporary that will be deleted!
      const ISF_FCS_Parametrization::FCS_StepInfo my_info( l_vec , l_cell , l_energy , l_time , l_valid , l_detector );
      *map_it += my_info;
      match = true;
      break;
    } // End of search for match in time and space
    if (!match) {
      map_item->second->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_cell , l_energy , l_time , l_valid , l_detector ) );
    } // Didn't match
  } // ID already in the map

} // That's it for updating the map!
