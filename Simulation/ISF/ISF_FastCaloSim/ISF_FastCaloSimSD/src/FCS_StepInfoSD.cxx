/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCS_StepInfoSD.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "G4Step.hh"
#include "G4ThreeVector.hh"

FCS_StepInfoSD::FCS_StepInfoSD(G4String a_name, const FCS_Param::Config& config)
  : G4VSensitiveDetector(a_name)
  , m_config(config)
  , m_larEmID(nullptr)
  , m_larFcalID(nullptr)
  , m_larHecID(nullptr)
  , m_larMiniFcalID(nullptr)
  , m_calo_dd_man(nullptr)
{
  m_calo_dd_man  = CaloDetDescrManager::instance(); //FIXME Move somewhere else!!
}

FCS_StepInfoSD::~FCS_StepInfoSD()
{
}

G4bool FCS_StepInfoSD::ProcessHits(G4Step*,G4TouchableHistory*)
{
  G4ExceptionDescription description;
  description << "ProcessHits: Base class method should not be called!!!";
  G4Exception("FCS_StepInfoSD", "FCSBadCall", FatalException, description);
  abort();
  return false;
}

void FCS_StepInfoSD::update_map(const CLHEP::Hep3Vector & l_vec, const Identifier & l_cell, double l_energy, double l_time, bool l_valid, int l_detector)
{
  // Drop any hits that don't have a good identifier attached
  if (!m_calo_dd_man->get_element(l_cell)) { return; }

  auto map_item = m_hit_map.find( l_cell );
  if (map_item==m_hit_map.end()) {
    m_hit_map[l_cell] = new std::vector< ISF_FCS_Parametrization::FCS_StepInfo* >;
    m_hit_map[l_cell]->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_cell , l_energy , l_time , l_valid , l_detector ) );
  }
  else {

    // Get the appropriate merging limits
    double tsame;
    const CaloCell_ID::CaloSample& layer = m_calo_dd_man->get_element(l_cell)->getSampling();
    if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
      tsame = m_config.m_maxTimeLAr;
    }
    else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3) {
      tsame = m_config.m_maxTimeHEC;
    }
    else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2) {
      tsame = m_config.m_maxTimeTile;
    }
    else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2) {
      tsame = m_config.m_maxTimeFCAL;
    }
    else {
      tsame = m_config.m_maxTime;
    }

    bool match = false;
    for (auto map_it : * map_item->second) {
      // Time check is straightforward
      double delta_t = fabs(map_it->time()-l_time);
      if ( delta_t >= tsame ) { continue; }

      // Distance check is less straightforward
      CLHEP::Hep3Vector a_diff = l_vec - map_it->position();
      double a_inv_length = 1./a_diff.mag();
      double delta_phi = fabs(sin(l_vec.phi()-map_it->position().phi())*a_diff.mag());
      double delta_eta = fabs(sin(l_vec.theta()-map_it->position().theta())*a_diff.mag());
      double delta_r = fabs(a_diff.dot( l_vec ) * a_inv_length);
      double hit_diff2 = map_it->position().diff2( l_vec );

      // Overall merging scheme
      if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
        // Customized merging scheme for LAr barrel and endcap, use only if we're not changing maxRadiusLAr value
        if(m_config.m_maxRadiusLAr == 25) {
          if (layer==CaloCell_ID::PreSamplerB || layer==CaloCell_ID::PreSamplerE) {
            // PS default is 1mm in eta, 5mm in phi, no cut in r
            if (delta_r >= m_config.m_maxrPS) continue;
            if (delta_eta >= m_config.m_maxEtaPS) continue;
            if (delta_phi >= m_config.m_maxPhiPS) continue;
          }
          else if (layer==CaloCell_ID::EMB1 || layer==CaloCell_ID::EME1) {
            // EM1 default is 1mm in eta, 5mm in phi, 15mm in r
            if (delta_r >= m_config.m_maxrEM1) continue;
            if (delta_eta >= m_config.m_maxEtaEM1) continue;
            if (delta_phi >= m_config.m_maxPhiEM1) continue;
          }
          else if (layer==CaloCell_ID::EMB2 || layer==CaloCell_ID::EME2) {
            // EM2 default is 1mm in eta, 5mm in phi, 60mm in r
            if (delta_r >= m_config.m_maxrEM2) continue;
            if (delta_eta >= m_config.m_maxEtaEM2) continue;
            if (delta_phi >= m_config.m_maxPhiEM2) continue;
          }
          else if (layer==CaloCell_ID::EMB3 || layer==CaloCell_ID::EME3) {
            // EM3 default is 1mm in eta, 5mm in phi, 8mm in r
            if (delta_r >= m_config.m_maxrEM3) continue;
            if (delta_eta >= m_config.m_maxEtaEM3) continue;
            if (delta_phi >= m_config.m_maxPhiEM3) continue;
          }
        }
        else { // Merging schemes done by changing maxRadiusLAr
          if ( hit_diff2 >= m_config.m_maxRadiusLAr ) continue;
        }
      }
      else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3) {
        if ( hit_diff2 >= m_config.m_maxRadiusHEC ) continue;
      }
      else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2) {
        if ( hit_diff2 >= m_config.m_maxRadiusTile ) continue;
      }
      else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2) {
        if ( hit_diff2 >= m_config.m_maxRadiusFCAL ) continue;
      }
      else {
        if ( hit_diff2 >= m_config.m_maxRadius ) continue;
      }
      // Found a match.  Make a temporary that will be deleted!
      ISF_FCS_Parametrization::FCS_StepInfo my_info( l_vec , l_cell , l_energy , l_time , l_valid , l_detector );
      *map_it += my_info;
      match = true;
      break;
    } // End of search for match in time and space
    if (!match) {
      map_item->second->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_cell , l_energy , l_time , l_valid , l_detector ) );
    } // Didn't match
  } // ID already in the map

} // That's it for updating the map!

void FCS_StepInfoSD::EndOfAthenaEvent( ISF_FCS_Parametrization::FCS_StepInfoCollection* hitContainer )
{
  // Unpack map into vector
  for (auto it : m_hit_map) {
    for (auto a_s : * it.second) {
      // Giving away ownership of the objects!
      hitContainer->push_back( a_s );
    }
    it.second->clear();
    delete it.second;
  } // Vector of IDs in the map
  m_hit_map.clear();
  if (m_config.verboseLevel > 4) {
    G4cout <<this->GetName()<< " DEBUG EndOfAthenaEvent: After initial cleanup, N=" << hitContainer->size() << G4endl;
  }
  return;
}
