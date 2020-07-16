/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/WeightedBDtoElectronFilter.h"
#include "TRandom3.h"

WeightedBDtoElectronFilter::WeightedBDtoElectronFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  std::vector<double> etaBinsDefault;
  etaBinsDefault.push_back( 0. );
  etaBinsDefault.push_back( 10. );
  std::vector<double> ptBinsDefault;
  ptBinsDefault.push_back( 5000. );
  ptBinsDefault.push_back( 7000000. ); // MeV
  std::vector<double> binPrescaleFactorsDefault;
  binPrescaleFactorsDefault.push_back( 1. ); // eta range 1, pt range 1

  declareProperty("EtaBins", m_etaBins = etaBinsDefault );
  declareProperty("PtBins",  m_ptBins = ptBinsDefault ); // MeV
  declareProperty("BinPrescaleFactors", m_binPrescaleFactors = binPrescaleFactorsDefault );

  m_PtMin = m_ptBins[ 0 ];
  m_PtMax = m_ptBins[ m_ptBins.size()-1 ];
  m_EtaRange = m_etaBins[ m_etaBins.size()-1 ];
  m_rand = 0;
}

StatusCode WeightedBDtoElectronFilter::filterInitialize() {
  m_rand = new TRandom3();
  unsigned int nEtaBins    = m_etaBins.size()-1;
  unsigned int nPtBins     = m_ptBins.size()-1;
  unsigned int nPrescaleFactors = m_binPrescaleFactors.size();
  if ( nPrescaleFactors != nEtaBins*nPtBins ) {
    ATH_MSG_ERROR(" number of prescale factors does not match number of eta-pt bins: " <<
                  " #(eta bins) = " << nEtaBins <<
                  " #(pt bins) = " << nPtBins <<
                  " #(prescale factors) = " << nPrescaleFactors);
    return StatusCode::FAILURE;
  }

  // Determine eta and pt min and max values
  if ( m_etaBins.size() < 2 || m_ptBins.size() < 2 ) {
    ATH_MSG_ERROR(" no eta or pt bin defined ");
    return StatusCode::FAILURE;
  }
  m_PtMin = m_ptBins[ 0 ];
  m_PtMax = m_ptBins[ m_ptBins.size()-1 ];
  m_EtaRange = m_etaBins[ m_etaBins.size()-1 ];

  ATH_MSG_DEBUG(" pt min value is: " << m_PtMin <<
                " pt max value is: " << m_PtMax <<
                " |eta| max value is: " << m_EtaRange);
  return StatusCode::SUCCESS;
}


StatusCode WeightedBDtoElectronFilter::filterFinalize() {
  delete m_rand;
  return StatusCode::SUCCESS;
}


StatusCode WeightedBDtoElectronFilter::filterEvent() {
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto pitr: *genEvt) {
      // check stables only
      if ( pitr->status() != 1)  continue;
      // check pdg_id
      if ( std::abs(pitr->pdg_id()) != 11 )  continue;
      double etaAbs = std::abs(pitr->momentum().pseudoRapidity());
      double pt = pitr->momentum().perp();
      // check pt
      if ( pt<m_PtMin || pt>m_PtMax) continue;
      // check eta
      if ( etaAbs > m_EtaRange ) continue;
      // check parent and ancestors for B hadron
      auto bParent = FindBParent( pitr );
      if ( !bParent ) continue;
      // apply prescale factors
      if ( ! PassPrescaleCheck( etaAbs, pt ) ) continue;
      ATH_MSG_VERBOSE(" found good electron (pass prescale): PID = " << pitr->pdg_id() <<
                                  "					   B hadron PID = " << bParent->pdg_id() <<
                                  "					   electron pt	= " << pitr->momentum().perp()/1000. << " GeV " <<
                                  "					   electron eta = " << pitr->momentum().pseudoRapidity() <<
                                  " ===>>> event passed WeightedBDtoElectronFilter ");
      return StatusCode::SUCCESS;
    } // particle loop
  } // gen events loop

  // If we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}


HepMC::ConstGenParticlePtr WeightedBDtoElectronFilter::FindBParent( HepMC::ConstGenParticlePtr part ) {
  if ( part->production_vertex() == 0 ) {
    ATH_MSG_DEBUG("Can't find parent (no production vertex)");
    return 0;
  }

#ifdef HEPMC3
  HepMC::ConstGenParticlePtr bParent = 0;
  auto parentItr = part->production_vertex()->particles_in().begin();
  if ( parentItr == part->production_vertex()->particles_in().end() ) {
    ATH_MSG_DEBUG("Vertex has no incoming particle ");
    return 0;
  }

  for ( ;; ) {
    if (isBHadron((*parentItr)->pdg_id()) || isDHadron((*parentItr)->pdg_id()) ) {
      ATH_MSG_INFO("Found B hadron (grand) parent ");
      break;
    }

    // get parent
    if ( !(*parentItr)->production_vertex() ) { // no production vertex
      ATH_MSG_INFO("No production vertex found => interrupt ");
      break;
    }
    if ( (*parentItr)->production_vertex()->particles_in().begin() == (*parentItr)->production_vertex()->particles_in().end() ) { // no parent particle
      ATH_MSG_INFO("No parent particle found => interrupt ");
      break;
    }
    if ( (*parentItr)->production_vertex()->particles_in().begin() == parentItr ) {
      ATH_MSG_INFO("Particle is its own parent => interrupt ");
      break;
    }

    ATH_MSG_INFO("Tracing back; id = " << (*parentItr)->pdg_id() <<
                 ";  parent id = " << (*(*parentItr)->production_vertex()->particles_in().begin())->pdg_id());
    parentItr = (*parentItr)->production_vertex()->particles_in().begin();
  }


  if ( isBHadron((*parentItr)->pdg_id()) || isDHadron((*parentItr)->pdg_id()) ) {
    bParent = (*parentItr);
    ATH_MSG_INFO("Found B hadron: " << (*parentItr)->pdg_id());
  } else {
    ATH_MSG_INFO("No B hadron found among ancestors; last found was: " << (*parentItr)->pdg_id());
  }
#else

  HepMC::GenVertex::particles_in_const_iterator parentItr = part->production_vertex()->particles_in_const_begin();
  if ( parentItr == part->production_vertex()->particles_in_const_end() ) {
    ATH_MSG_DEBUG("Vertex has no incoming particle ");
    return 0;
  }

  for ( ;; ) {
    if (isBHadron((*parentItr)->pdg_id()) || isDHadron((*parentItr)->pdg_id()) ) {
      ATH_MSG_INFO("Found B hadron (grand) parent ");
      break;
    }

    // get parent
    if ( !(*parentItr)->production_vertex() ) { // no production vertex
      ATH_MSG_INFO("No production vertex found => interrupt ");
      break;
    }
    if ( (*parentItr)->production_vertex()->particles_in_const_begin() == (*parentItr)->production_vertex()->particles_in_const_end() ) { // no parent particle
      ATH_MSG_INFO("No parent particle found => interrupt ");
      break;
    }
    if ( (*parentItr)->production_vertex()->particles_in_const_begin() == parentItr ) {
      ATH_MSG_INFO("Particle is its own parent => interrupt ");
      break;
    }

    ATH_MSG_INFO("Tracing back; id = " << (*parentItr)->pdg_id() <<
                 ";  parent id = " << (*(*parentItr)->production_vertex()->particles_in_const_begin())->pdg_id());
    parentItr = (*parentItr)->production_vertex()->particles_in_const_begin();
  }

  HepMC::GenParticle* bParent = 0;
  if ( isBHadron((*parentItr)->pdg_id()) || isDHadron((*parentItr)->pdg_id()) ) {
    bParent = (*parentItr);
    ATH_MSG_INFO("Found B hadron: " << (*parentItr)->pdg_id());
  } else {
    ATH_MSG_INFO("No B hadron found among ancestors; last found was: " << (*parentItr)->pdg_id());
  }
#endif
  return bParent;
}


bool WeightedBDtoElectronFilter::PassPrescaleCheck( double etaAbs, double pt ) {
  double prescaleFactor = 1.;
  // loop eta-pt bins to get prescale factor
  for ( unsigned int iEtaBin = 0; iEtaBin < m_etaBins.size()-1; iEtaBin++ ) {
    if ( etaAbs > m_etaBins[iEtaBin] && etaAbs < m_etaBins[iEtaBin+1] ) {
      for ( unsigned int iPtBin = 0; iPtBin < m_ptBins.size()-1; iPtBin++ ) {
        if ( pt > m_ptBins[iPtBin] && pt < m_ptBins[iPtBin+1] ) {

          unsigned int prescaleIndex = iEtaBin * (m_ptBins.size()-1) + iPtBin;
          prescaleFactor = m_binPrescaleFactors[ prescaleIndex ];

          if ( prescaleFactor < 1 ) {
            ATH_MSG_DEBUG(" prescale factor < 1  (" << prescaleFactor << ") => return false ");
            return false;
          }

          // Generate random number to decide, if prescale is passed
          double r = m_rand->Rndm();
          if ( r < 1./prescaleFactor ) {
            ATH_MSG_VERBOSE(" pass prescale ");
            return true;
          } else {
            ATH_MSG_VERBOSE(" doesn't pass prescale ");
            return false;
          }
        }
      }
    }
  }

  // If we get here, something is wrong
  ATH_MSG_DEBUG(" did not find correct eta-pt bin, return false ");
  return false;
}


std::string WeightedBDtoElectronFilter::longToStr( const long n ) const {
  if (0==n) return "0";
  std::string str = "";
  for ( long m = n; m!=0; m/=10 ) str = char( '0' + std::abs(m%10) ) + str;
  if ( n<0 ) str = "-" + str;
  return str;
}


bool WeightedBDtoElectronFilter::isBBaryon(const int pID) const {
  // PdgID of B-baryon is of form ...xxx5xxx
  std::string idStr = longToStr( abs(pID) );
  char digit4 = idStr[ idStr.length() - 4 ];
  return (digit4=='5');
}

bool WeightedBDtoElectronFilter::isBMeson(const int pID) const {
  // PdgID of B-meson is of form ...xxx05xx
  std::string idStr = longToStr( abs(pID) );
  char digit3 = idStr[ idStr.length() - 3 ];
  char digit4;
  if ( idStr.length() < 4 ) { digit4 = '0'; }
  else { digit4 = idStr[ idStr.length() - 4 ]; };
  return ((digit4=='0') && (digit3=='5'));
}

bool WeightedBDtoElectronFilter::isDBaryon(const int pID) const {
  // PdgID of D-baryon is of form ...xxx4xxx
  std::string idStr = longToStr( abs(pID) );
  char digit4 = idStr[ idStr.length() - 4 ];
  return (digit4=='4');
}

bool WeightedBDtoElectronFilter::isDMeson(const int pID) const {
  // PdgID of D-meson is of form ...xxx04xx
  std::string idStr = longToStr( abs(pID) );
  char digit3 = idStr[ idStr.length() - 3 ];
  char digit4;
  if( idStr.length() < 4 ) { digit4 = '0'; }
  else { digit4 = idStr[ idStr.length() - 4 ]; };
  return ( (digit4=='0') && (digit3=='4') );
}
