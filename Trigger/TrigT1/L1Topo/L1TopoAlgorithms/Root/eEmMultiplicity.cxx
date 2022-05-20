/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * eEmMultiplicity.cpp
 * Created by Carlos Moreno on 05/06/20.
 *
 * @brief algorithm that computes the multiplicity for a specified list and ET threshold
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading MinET

**********************************/

#include <cmath>

#include "L1TopoAlgorithms/eEmMultiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/eEmTOBArray.h"

REGISTER_ALG_TCS(eEmMultiplicity)

using namespace std;


TCS::eEmMultiplicity::eEmMultiplicity(const std::string & name) : CountingAlg(name)
{
   
   
   setNumberOutputBits(12); //To-Do: Make this flexible to addapt to the menu. Each counting requires more than one bit

}

TCS::eEmMultiplicity::~eEmMultiplicity(){}


TCS::StatusCode
TCS::eEmMultiplicity::initialize() { 

  m_threshold = getThreshold();

  // book histograms
  std::string hname_accept = "eEmMultiplicity_accept_EtaPt_"+m_threshold->name();
  bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "#eta#times40", "E_{t} [GeV]", 200, -200, 200, 100, 0, 100);

  hname_accept = "eEmMultiplicity_accept_counts_"+m_threshold->name();
  bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "counts", 15, 0, 15);

  return StatusCode::SUCCESS;
     
}


TCS::StatusCode
TCS::eEmMultiplicity::processBitCorrect( const TCS::InputTOBArray & input,
					 Count & count)

{
   return process(input, count);
}

TCS::StatusCode
TCS::eEmMultiplicity::process( const TCS::InputTOBArray & input,
			       Count & count )
{

  // Grab the threshold and cast it into the right type
  const auto& eEMThr = dynamic_cast<const TrigConf::L1Threshold_eEM &>(*m_threshold);

  // Grab inputs
  const eEmTOBArray & eems = dynamic_cast<const eEmTOBArray&>(input);

  int counting = 0; 
  
  // loop over input TOBs
  for(eEmTOBArray::const_iterator eem = eems.begin();
      eem != eems.end();
      ++eem ) {
    
    // Menu threshold uses 0.1 eta granularity but eFex objects have 0.025 eta granularity
    // eFex eta is calculated as 4*eta_tower (0.1 gran.) + seed (0.025 gran.), eta from -25 to 24
    int eta_thr;
    if ( (*eem)->eta()%4 >= 0 ) { eta_thr = (*eem)->eta() - (*eem)->eta()%4; }
    else                        { eta_thr = (*eem)->eta() - (*eem)->eta()%4 - 4; }

    bool passed = (*eem)->Et() > eEMThr.thrValue100MeV(eta_thr/4); // Convert eta_thr to units of 0.1 to pass as an argument

    if ( !isocut(TrigConf::Selection::wpToString(eEMThr.reta()), (*eem)->Reta()) ) {continue;}
    if ( !isocut(TrigConf::Selection::wpToString(eEMThr.rhad()), (*eem)->Rhad()) ) {continue;}
    if ( !isocut(TrigConf::Selection::wpToString(eEMThr.wstot()), (*eem)->Wstot()) ) {continue;}

    if (passed) {
      counting++; 
      fillHist2D( m_histAccept[0], (*eem)->eta(), (*eem)->EtDouble() );
    }

  }

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there
  count.setSizeCount(counting);
  
  return TCS::StatusCode::SUCCESS;

}
