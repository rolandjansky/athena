/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  TopoCore
//

#include <cmath>

#include "L1TopoAlgorithms/cTauMultiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/cTauTOBArray.h"

REGISTER_ALG_TCS(cTauMultiplicity)

// constructor
TCS::cTauMultiplicity::cTauMultiplicity(const std::string & name) : CountingAlg(name) {
  setNumberOutputBits(12); //To-Do: Make this flexible to addapt to the menu. Each counting requires more than one bit       
}


// destructor
TCS::cTauMultiplicity::~cTauMultiplicity() {}


TCS::StatusCode
TCS::cTauMultiplicity::initialize() {

  m_threshold = getThreshold();
  
  // book histograms
  std::string hname_accept = "cTauMultiplicity_accept_EtaPt_"+m_threshold->name();
  bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "#eta#times40", "E_{t} [GeV]", 200, -200, 200, 100, 0, 100);

  hname_accept = "cTauMultiplicity_accept_counts_"+m_threshold->name();
  bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "counts", 15, 0, 15);

  // cTau TOB monitoring histograms
  bookHistMult(m_histcTauEt, "cTauTOBEt", "Matched cTau TOB Et", "E_{t} [GeV]", 200, 0, 200);
  bookHistMult(m_histcTauPhiEta, "cTauTOBPhiEta", "Matched cTau TOB location", "#eta#times40", "#phi#times20", 200, -200, 200, 128, 0, 128);
  bookHistMult(m_histcTauEtEta, "cTauTOBEtEta", "Matched cTau TOB Et vs eta", "#eta#times40", "E_{t} [GeV]", 200, -200, 200, 200, 0, 200);
  bookHistMult(m_histcTauIso, "cTauTOBIso", "Matched cTau isolation", "isolation", 200, 0, 10);
  bookHistMult(m_histcTauIsoScore, "cTauTOBIsoScore", "Matched cTau isolation score", "isolation score", 4, 0, 4);

  return StatusCode::SUCCESS;
}


// To be implemented 
TCS::StatusCode
TCS::cTauMultiplicity::processBitCorrect( const TCS::InputTOBArray & input, Count & count)
{
  return process(input, count);
}



TCS::StatusCode
TCS::cTauMultiplicity::process( const TCS::InputTOBArray & input, Count & count )
{

  // Grab the threshold and cast it into the right type                                                                                                                                                 
  const auto& cTauThr = dynamic_cast<const TrigConf::L1Threshold_cTAU &>(*m_threshold);

  const cTauTOBArray & cTaus = dynamic_cast<const cTauTOBArray&>(input);

  int counting = 0;
  // Loop over eTau candidates
  for(cTauTOBArray::const_iterator etauCand = cTaus.begin(); etauCand != cTaus.end(); ++etauCand ) {

    if((*etauCand)->tobType() != TCS::ETAU) continue;
    
    bool accept = false;     // accept = true if (isMatched==true && isIsolated==true) || (isMatched==false)
    bool isMatched  = false; // Is the eTau matched to a jTau?
    bool isIsolated = false; // If matched: does the resulting cTau pass the isolation cut?
    float isolation = 0;              // cTau isolation (0 if no match is found)
    unsigned int isolation_score = 0; // cTau isolation score (0 if no match is found)

    // Loop over jTau candidates
    for(cTauTOBArray::const_iterator jtauCand = cTaus.begin(); jtauCand != cTaus.end(); ++jtauCand ) {
      
      if((*jtauCand)->tobType() != TCS::JTAU) continue;

      isMatched = cTauMatching( *etauCand, *jtauCand );

      if (isMatched) {
        // Isolation condition coded as in firmware https://indico.cern.ch/event/1079697/contributions/4541419/attachments/2315137/3940824/cTAU_FirmwareAlgoProposal.pdf page 8. 
        // Using physics values for the cut 
        isolation = static_cast<float>((*jtauCand)->EtIso()) / static_cast<float>((*etauCand)->Et()); // Internal variable for checks
        isolation_score = convertIsoToBit( *etauCand, *jtauCand );
        isIsolated = isocut( TrigConf::Selection::wpToString(cTauThr.isolation()), isolation_score );
        break; // Break loop when a match is found
      }

    } // End of jTau loop

    // Fill cTau TOB histograms before threshold cuts (matched cTaus only)
    if (isMatched) {
      fillHist1D( m_histcTauEt[0], (*etauCand)->EtDouble() );
      fillHist2D( m_histcTauPhiEta[0], (*etauCand)->eta(), (*etauCand)->phi() );
      fillHist2D( m_histcTauEtEta[0], (*etauCand)->eta(), (*etauCand)->EtDouble() );
      fillHist1D( m_histcTauIso[0], isolation );
      fillHist1D( m_histcTauIsoScore[0], isolation_score );
    }

    // This is a good cTau
    if ( isMatched && isIsolated ) { accept = true; }
    // This is a non-matched eTau
    if ( not isMatched ) { accept = true; }

    // Dividing by 4 standing for converting eta from 0.025 to 0.1 granularity                                                                      
    // Requirement on pT based on eTau candidate 
    accept = accept && (*etauCand)->Et() >= cTauThr.thrValue100MeV((*etauCand)->eta()/4);
    if (accept) {
      counting++;
      fillHist2D( m_histAccept[0], (*etauCand)->eta(), (*etauCand)->EtDouble() );
    }
    
  } // End of eTau loop

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there                                                                                                                               
  
  count.setSizeCount(counting);

  return TCS::StatusCode::SUCCESS;
  
}


unsigned int
TCS::cTauMultiplicity::convertIsoToBit(const TCS::cTauTOB * etauCand, const TCS::cTauTOB * jtauCand) const {
  unsigned int bit = 0;

  // Assign the tightest accept WP as default bit
  if( jtauCand->EtIso() < etauCand->Et() * 0.4 ) bit = 1; // Loose
  if( jtauCand->EtIso() < etauCand->Et() * 0.35) bit = 2; // Medium
  if( jtauCand->EtIso() < etauCand->Et() * 0.3 ) bit = 3; // Tight 
  
  return bit;
}


bool
TCS::cTauMultiplicity::cTauMatching(const TCS::cTauTOB * etauCand, const TCS::cTauTOB * jtauCand) const {

  bool matching  = false; 

  // Matching is done comparing eta_tower and phi_tower (granularity = 0.1)
  // These coordinates represent the lower edge of the towers (both for eFEX and jFEX)

  // eTau eta = 4*eta_tower + seed, eta from -25 to 24
  int eTauEtaTower;
  if (etauCand->eta()%4 >= 0 ) { eTauEtaTower = etauCand->eta() - etauCand->eta()%4; }
  else                         { eTauEtaTower = etauCand->eta() - etauCand->eta()%4 - 4; }

  int jTauEtaTower = jtauCand->eta();              // jTau eta = 4*eta_tower
  unsigned int eTauPhiTower = etauCand->phi();     // eTau phi = 2*phi_tower 
  unsigned int jTauPhiTower = jtauCand->phi();     // jTau phi = 2*phi_tower 

  matching = (eTauEtaTower == jTauEtaTower) && (eTauPhiTower == jTauPhiTower);

  return matching;

}


#ifndef TRIGCONF_STANDALONE
size_t
TCS::cTauMultiplicity::cTauMatching(const xAOD::eFexTauRoI & eTau, const xAOD::jFexTauRoIContainer & jTauRoIs) {

  // Return the index of the matched jTau if existent (otherwise return std::numeric_limits<size_t>::max())
  size_t i_matched{std::numeric_limits<size_t>::max()};
  size_t i_jTau{0};

  for (const xAOD::jFexTauRoI* jTau : jTauRoIs) {

    // eFEX: etaTower = iEta, phiTower = iPhi
    // jFEX: etaTower = globalEta, phiTower = globalPhi
    bool matching = ( eTau.iEta() == jTau->globalEta() ) && ( static_cast<unsigned int>(eTau.iPhi()) == jTau->globalPhi() );

    if ( matching ) {
      i_matched = i_jTau;
      break; // Break the loop when a match is found
    }
    ++i_jTau;
  }

  return i_matched;

}
#endif
