/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  TopoCore
//

#include <cmath>

#include "L1TopoAlgorithms/cTauMultiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/cTauTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"

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

  // fill output array with GenericTOB buildt from ctaus
  for(cTauTOBArray::const_iterator etau_cand = cTaus.begin(); etau_cand != cTaus.end(); ++etau_cand ) {
    for(cTauTOBArray::const_iterator jtau_cand = cTaus.begin(); jtau_cand != cTaus.end(); ++jtau_cand ) {
      
      const GenericTOB gtob_etau(**etau_cand);
      const GenericTOB gtob_jtau(**jtau_cand);
      
      if((*etau_cand)->tobType() != TCS::ETAU) continue;
      if((*jtau_cand)->tobType() != TCS::JTAU) continue;
      
      bool matching = (gtob_etau.eta() == gtob_jtau.eta()) && (gtob_etau.phi() == gtob_jtau.phi());
      // Isolation condition coded as in firmware https://indico.cern.ch/event/1079697/contributions/4541419/attachments/2315137/3940824/cTAU_FirmwareAlgoProposal.pdf page 8. 
      // Using physics values for the cut
      unsigned int isolation_score = convertIsoToBit(gtob_etau,gtob_jtau);
      bool isolation = isocut( TrigConf::Selection::wpToString(cTauThr.isolation()), isolation_score );

      bool passed = false;
      
      // This is a good ctau
      if( matching && isolation )
	{
	  passed = true;
	}
      // In case of non matched tau passes
      if( !(matching) )
	{
	  passed = true;
	}

      // Dividing by 4 standing for converting eta from 0.025 to 0.1 granularity as it is defined in the menu as 0.1 gran.                                                                              
      // Requirement on pT based on etau candidate 
      passed = passed && gtob_etau.Et() >= cTauThr.thrValue100MeV(gtob_etau.eta()/4);
      
      if (passed) {
	counting++;
	fillHist2D( m_histAccept[0], gtob_etau.eta(), gtob_etau.EtDouble() );
      }
    }
    
    fillHist1D( m_histAccept[1], counting);
    
    // Pass counting to TCS::Count object - output bits are composed there                                                                                                                               
    
    count.setSizeCount(counting);

  }
  
  return TCS::StatusCode::SUCCESS;
  
}


unsigned int
TCS::cTauMultiplicity::convertIsoToBit(const TCS::GenericTOB & input_etau, const TCS::GenericTOB & input_jtau){
  unsigned int bit = 0;
  // Assign the tightest passed WP as default bit
  if(input_jtau.jtauiso() < input_etau.Et()* 0.4 ) bit = 1; // Loose
  if(input_jtau.jtauiso() < input_etau.Et()* 0.35) bit = 2; // Medium
  if(input_jtau.jtauiso() < input_etau.Et()* 0.3 ) bit = 3; // Tight 
  
  return bit;
}

 
