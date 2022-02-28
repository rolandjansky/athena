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
  bool isMult = true;
 
  std::string hname_accept = "hcTauMultiplicity_accept_EtaPt_"+m_threshold->name();
  bookHist(m_histAccept, hname_accept, "ETA vs PT", 150, -100, 100, 30, 0., 20., isMult);

  hname_accept = "hcTauMultiplicity_accept_counts_"+m_threshold->name();
  bookHist(m_histAccept, hname_accept, "COUNTS", 15, 0., 10., isMult);

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
  auto cTauThr = dynamic_cast<const TrigConf::L1Threshold_cTAU &>(*m_threshold);

  const cTauTOBArray & cTaus = dynamic_cast<const cTauTOBArray&>(input);

  int counting = 0;
  int count_eTaus = 0; 
  // Loop over eTau candidates
  for(cTauTOBArray::const_iterator etau_cand = cTaus.begin(); etau_cand != cTaus.end(); ++etau_cand ) {

    if((*etau_cand)->tobType() != TCS::ETAU) continue;
    
    bool matching  = false; // Is it matched to a jTau?
    bool isolation = false; // If matched: does it pass the isolation cut?
    bool passed    = false; // passed = true if (matching==true && isolation==true) || (matching==false)

    // Loop over jTau candidates
    for(cTauTOBArray::const_iterator jtau_cand = cTaus.begin(); jtau_cand != cTaus.end(); ++jtau_cand ) {
      
      if((*jtau_cand)->tobType() != TCS::JTAU) continue;

      // Matching is done comparing eta_tower and phi_tower (granularity = 0.1)
      // These coordinates represent the lower edge of the towers (both for eFEX and jFEX)
      int eTauEtaTower = (*etau_cand)->eta() - (*etau_cand)->eta()%4;  // eTau eta = 4*eta_tower + seed 
      int jTauEtaTower = (*jtau_cand)->eta();                          // jTau eta = 4*eta_tower
      unsigned int eTauPhiTower = (*etau_cand)->phi();                 // eTau phi = 2*phi_tower 
      unsigned int jTauPhiTower = (*jtau_cand)->phi();                 // jTau phi = 2*phi_tower 

      bool matching = (eTauEtaTower == jTauEtaTower) && (eTauPhiTower == jTauPhiTower);

      if (matching) {
        // Isolation condition coded as in firmware https://indico.cern.ch/event/1079697/contributions/4541419/attachments/2315137/3940824/cTAU_FirmwareAlgoProposal.pdf page 8. 
        // Using physics values for the cut
        unsigned int isolation_score = convertIsoToBit( (*etau_cand)->Et(), (*jtau_cand)->isolation() );
        isolation = isocut( TrigConf::Selection::wpToString(cTauThr.isolation()), isolation_score );
      }

    } // End of jTau loop

    
    // This is a good cTau
    if( matching && isolation ) { passed = true; }
    // This is a non-matched eTau
    if( !(matching) ) { passed = true; }

    // Dividing by 4 standing for converting eta from 0.025 to 0.1 granularity                                                                      
    // Requirement on pT based on eTau candidate 
    passed = passed && (*etau_cand)->Et() >= cTauThr.thrValue100MeV((*etau_cand)->eta()/4);
    if (passed) {
      counting++;
      fillHist2D( m_histAccept[0], (*etau_cand)->eta(), (*etau_cand)->EtDouble() );
    }
    count_eTaus++;
    
  } // End of eTau loop


  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there                                                                                                                               
  
  count.setSizeCount(counting);

  return TCS::StatusCode::SUCCESS;
  
}


unsigned int
TCS::cTauMultiplicity::convertIsoToBit(const unsigned int & etauEt, const unsigned int & jtauIso){
  unsigned int bit = 0;
  // Assign the tightest passed WP as default bit
  if( jtauIso < etauEt * 0.4 ) bit = 1; // Loose
  if( jtauIso < etauEt * 0.35) bit = 2; // Medium
  if( jtauIso < etauEt * 0.3 ) bit = 3; // Tight 
  
  return bit;
}

 
