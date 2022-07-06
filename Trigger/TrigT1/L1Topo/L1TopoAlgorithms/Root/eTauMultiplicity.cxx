/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * eTauMultiplicity.cpp
 * Created by Carlos Moreno on 05/06/20.
 *
 * @brief algorithm that computes the multiplicity for a specified list and ET threshold
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading MinET

**********************************/

#include <cmath>

#include "L1TopoAlgorithms/eTauMultiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/eTauTOBArray.h"

REGISTER_ALG_TCS(eTauMultiplicity)

using namespace std;


TCS::eTauMultiplicity::eTauMultiplicity(const std::string & name) : CountingAlg(name)
{
   
   
   setNumberOutputBits(12); //To-Do: Make this flexible to addapt to the menu. Each counting requires more than one bit

}

TCS::eTauMultiplicity::~eTauMultiplicity(){}


TCS::StatusCode
TCS::eTauMultiplicity::initialize() { 

  m_threshold = getThreshold();

  // book histograms
  std::string hname_accept = "eTauMultiplicity_accept_EtaPt_"+m_threshold->name();
  bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "#eta#times40", "E_{t} [GeV]", 200, -200, 200, 100, 0, 100);

  hname_accept = "eTauMultiplicity_accept_counts_"+m_threshold->name();
  bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "counts", 15, 0, 15);

  return StatusCode::SUCCESS;
     
}


TCS::StatusCode
TCS::eTauMultiplicity::processBitCorrect( const TCS::InputTOBArray & input,
					 Count & count)

{
   return process(input, count);
}

TCS::StatusCode
TCS::eTauMultiplicity::process( const TCS::InputTOBArray & input,
			       Count & count )
{

  // Grab the threshold and cast it into the right type
  const auto& eTAUThr = dynamic_cast<const TrigConf::L1Threshold_eTAU &>(*m_threshold);

  // Grab inputs
  const eTauTOBArray & etaus = dynamic_cast<const eTauTOBArray&>(input);

  int counting = 0; 
  
  // loop over input TOBs
  for(eTauTOBArray::const_iterator etau = etaus.begin();
      etau != etaus.end();
      ++etau ) {
    
    // Menu threshold uses 0.1 eta granularity but eFex objects have 0.025 eta granularity
    // eFex eta is calculated as 4*eta_tower (0.1 gran.) + seed (0.025 gran.), eta from -25 to 24
    int eta_thr;
    if ( (*etau)->eta()%4 >= 0 ) { eta_thr = (*etau)->eta() - (*etau)->eta()%4; }
    else                         { eta_thr = (*etau)->eta() - (*etau)->eta()%4 - 4; }

    bool passed = (*etau)->Et() > eTAUThr.thrValue100MeV(eta_thr/4); // Convert eta_thr to units of 0.1 to pass as an argument

    if ( !isocut(TrigConf::Selection::wpToString(eTAUThr.rCore()), (*etau)->rCore()) ) {continue;}
    if ( !isocut(TrigConf::Selection::wpToString(eTAUThr.rHad()), (*etau)->rHad()) ) {continue;}

    if (passed) {
      counting++; 
      fillHist2D( m_histAccept[0], (*etau)->eta(), (*etau)->EtDouble() );
    }

  }

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there
  count.setSizeCount(counting);
  
  return TCS::StatusCode::SUCCESS;

}
