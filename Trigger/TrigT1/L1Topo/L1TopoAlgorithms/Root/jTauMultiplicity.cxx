/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * jTauMultiplicity.cpp
 * Created by Carlos Moreno on 05/06/20.
 *
 * @brief algorithm that computes the multiplicity for a specified list and ET threshold
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading MinET

**********************************/

#include <cmath>

#include "L1TopoAlgorithms/jTauMultiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jTauTOBArray.h"

REGISTER_ALG_TCS(jTauMultiplicity)

using namespace std;


TCS::jTauMultiplicity::jTauMultiplicity(const std::string & name) : CountingAlg(name)
{
   
   
   setNumberOutputBits(12); //To-Do: Make this flexible to addapt to the menu. Each counting requires more than one bit

}

TCS::jTauMultiplicity::~jTauMultiplicity(){}


TCS::StatusCode
TCS::jTauMultiplicity::initialize() { 

  m_threshold = getThreshold();

  // book histograms
  std::string hname_accept = "jTauMultiplicity_accept_EtaPt_"+m_threshold->name();
  bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "#eta#times40", "E_{t} [GeV]", 200, -200, 200, 100, 0, 100);

  hname_accept = "jTauMultiplicity_accept_counts_"+m_threshold->name();
  bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "counts", 15, 0, 15);

  return StatusCode::SUCCESS;
     
}


TCS::StatusCode
TCS::jTauMultiplicity::processBitCorrect( const TCS::InputTOBArray & input,
					 Count & count)

{
   return process(input, count);
}

TCS::StatusCode
TCS::jTauMultiplicity::process( const TCS::InputTOBArray & input,
			       Count & count )
{

  // Grab the threshold and cast it into the right type
  const auto& jTAUThr = dynamic_cast<const TrigConf::L1Threshold_jTAU &>(*m_threshold);

  // Grab inputs
  const jTauTOBArray & jtaus = dynamic_cast<const jTauTOBArray&>(input);

  int counting = 0; 
  
  // loop over input TOBs
  for(jTauTOBArray::const_iterator jtau = jtaus.begin();
      jtau != jtaus.end();
      ++jtau ) {
    
    // Dividing by 4 standing for converting eta from 0.025 to 0.1 granularity as it is defined in the menu as 0.1 gran.
    bool passed = (*jtau)->Et() > jTAUThr.thrValue100MeV((*jtau)->eta()/4);

    if ( !isocut(TrigConf::Selection::wpToString(jTAUThr.isolation()), convertIsoToBit(*jtau)) ) {continue;}

    if (passed) {
      counting++; 
      fillHist2D( m_histAccept[0], (*jtau)->eta(), (*jtau)->EtDouble() );
    }

  }

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there
  count.setSizeCount(counting);
  
  return TCS::StatusCode::SUCCESS;

}

unsigned int
TCS::jTauMultiplicity::convertIsoToBit(const TCS::jTauTOB * jtau) const {
  unsigned int bit = 0;

  // Assign the tightest accept WP as default bit
  // TO DO: read WPs from the menu
  if( jtau->EtIso() < jtau->Et() * 0.4 ) bit = 1; // Loose
  if( jtau->EtIso() < jtau->Et() * 0.35) bit = 2; // Medium
  if( jtau->EtIso() < jtau->Et() * 0.3 ) bit = 3; // Tight 
  
  return bit;
}

