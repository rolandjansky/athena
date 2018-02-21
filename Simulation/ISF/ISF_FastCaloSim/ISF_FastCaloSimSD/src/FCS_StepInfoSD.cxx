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
#include "CaloIdentifier/TileID.h"
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
  , m_tileID(nullptr)
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

inline double FCS_StepInfoSD::getMaxTime(const CaloCell_ID::CaloSample& layer) const
{
  /// NB The result of this function should actually be constant for each SD
  if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
    return m_config.m_maxTimeLAr;
  }
  if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3) {
    return m_config.m_maxTimeHEC;
  }
  if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2) {
    return m_config.m_maxTimeFCAL;
  }
  return m_config.m_maxTime;
}

inline double FCS_StepInfoSD::getMaxRadius(const CaloCell_ID::CaloSample& layer) const
{
  /// NB The result of this function should actually be constant for each SD
  if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
    return m_config.m_maxRadiusLAr;
  }
  if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3) {
    return m_config.m_maxRadiusHEC;
  }
  if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2) {
        return m_config.m_maxRadiusFCAL;
  }
  return m_config.m_maxRadius;
}

inline double FCS_StepInfoSD::getMaxDeltaR(const CaloCell_ID::CaloSample& layer) const
{
  /// NB The result of this function should actually be constant for each SD
  if(m_config.m_maxRadiusLAr != 25) return 999.;
  if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
    if (layer==CaloCell_ID::PreSamplerB || layer==CaloCell_ID::PreSamplerE) {
      // PS default is 1mm in eta, 5mm in phi, no cut in r
      return m_config.m_maxrPS;
    }
    else if (layer==CaloCell_ID::EMB1 || layer==CaloCell_ID::EME1) {
      // EM1 default is 1mm in eta, 5mm in phi, 15mm in r
      return m_config.m_maxrEM1;
    }
    else if (layer==CaloCell_ID::EMB2 || layer==CaloCell_ID::EME2) {
      // EM2 default is 1mm in eta, 5mm in phi, 60mm in r
      return m_config.m_maxrEM2;
    }
    else if (layer==CaloCell_ID::EMB3 || layer==CaloCell_ID::EME3) {
      // EM3 default is 1mm in eta, 5mm in phi, 8mm in r
      return m_config.m_maxrEM3;
    }
  }
  return 999.;
}

inline double FCS_StepInfoSD::getMaxDeltaEta(const CaloCell_ID::CaloSample& layer) const
{
  /// NB The result of this function should actually be constant for each SD
  if(m_config.m_maxRadiusLAr != 25) return 999.;
  if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
    if (layer==CaloCell_ID::PreSamplerB || layer==CaloCell_ID::PreSamplerE) {
      // PS default is 1mm in eta, 5mm in phi, no cut in r
      return m_config.m_maxEtaPS;
    }
    else if (layer==CaloCell_ID::EMB1 || layer==CaloCell_ID::EME1) {
      // EM1 default is 1mm in eta, 5mm in phi, 15mm in r
      return m_config.m_maxEtaEM1;
    }
    else if (layer==CaloCell_ID::EMB2 || layer==CaloCell_ID::EME2) {
      // EM2 default is 1mm in eta, 5mm in phi, 60mm in r
      return m_config.m_maxEtaEM2;
    }
    else if (layer==CaloCell_ID::EMB3 || layer==CaloCell_ID::EME3) {
      // EM3 default is 1mm in eta, 5mm in phi, 8mm in r
      return m_config.m_maxEtaEM3;
    }
  }
  return 999.;
}

inline double FCS_StepInfoSD::getMaxDeltaPhi(const CaloCell_ID::CaloSample& layer) const
{
  /// NB The result of this function should actually be constant for each SD
  if(m_config.m_maxRadiusLAr != 25) return 999.;
  if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
    if (layer==CaloCell_ID::PreSamplerB || layer==CaloCell_ID::PreSamplerE) {
      // PS default is 1mm in eta, 5mm in phi, no cut in r
      return m_config.m_maxPhiPS;
    }
    else if (layer==CaloCell_ID::EMB1 || layer==CaloCell_ID::EME1) {
      // EM1 default is 1mm in eta, 5mm in phi, 15mm in r
      return m_config.m_maxPhiEM1;
    }
    else if (layer==CaloCell_ID::EMB2 || layer==CaloCell_ID::EME2) {
      // EM2 default is 1mm in eta, 5mm in phi, 60mm in r
      return m_config.m_maxPhiEM2;
    }
    else if (layer==CaloCell_ID::EMB3 || layer==CaloCell_ID::EME3) {
      // EM3 default is 1mm in eta, 5mm in phi, 8mm in r
       return m_config.m_maxPhiEM3;
    }
  }
  return 999.;
}


void FCS_StepInfoSD::update_map(const CLHEP::Hep3Vector & l_vec, const Identifier & l_identifier, double l_energy, double l_time, bool l_valid, int l_detector, double timeWindow, double distanceWindow)
{
  // NB l_identifier refers to:
  // - the cell identifier for LAr
  // - the PMT identifier for Tile

  // Drop any hits that don't have a good identifier attached
  if (!m_calo_dd_man->get_element(l_identifier)) {
    if(m_config.verboseLevel > 4) {
      G4cout<<this->GetName()<<" DEBUG update_map: bad identifier: "<<l_identifier.getString()<<" skipping this hit."<<G4endl;
    }
    return;
  }

  auto map_item = m_hit_map.find( l_identifier );
  if (map_item==m_hit_map.end()) {
    m_hit_map[l_identifier] = new std::vector< ISF_FCS_Parametrization::FCS_StepInfo* >;
    m_hit_map[l_identifier]->reserve(200);
    m_hit_map[l_identifier]->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_identifier , l_energy , l_time , l_valid , l_detector ) );
  }
  else {
    bool match = false;
    for (auto map_it : * map_item->second) {
      // Time check
      const double delta_t = std::fabs(map_it->time()-l_time);
      if ( delta_t >= timeWindow ) { continue; }
      // Distance check
      const double hit_diff2 = map_it->position().diff2( l_vec );
      if ( hit_diff2 >= distanceWindow ) { continue; }
      // Found a match.  Make a temporary that will be deleted!
      const ISF_FCS_Parametrization::FCS_StepInfo my_info( l_vec , l_identifier , l_energy , l_time , l_valid , l_detector );
      *map_it += my_info;
      match = true;
      break;
    } // End of search for match in time and space
    if (!match) {
      map_item->second->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_identifier , l_energy , l_time , l_valid , l_detector ) );
    } // Didn't match
  } // ID already in the map
  return;
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
