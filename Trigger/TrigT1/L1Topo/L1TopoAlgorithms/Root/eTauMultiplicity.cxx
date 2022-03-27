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
#include "L1TopoEvent/GenericTOB.h"

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
    
    const GenericTOB gtob(**etau);

    // Dividing by 4 standing for converting eta from 0.025 to 0.1 granularity as it is defined in the menu as 0.1 gran.
    bool passed = gtob.Et() >= eTAUThr.thrValue100MeV(gtob.eta()/4);

    if ( !isocut(TrigConf::Selection::wpToString(eTAUThr.rCore()), gtob.rCore()) ) {continue;}
    if ( !isocut(TrigConf::Selection::wpToString(eTAUThr.rHad()), gtob.rHad()) ) {continue;}

    if (passed) {
      counting++; 
      fillHist2D( m_histAccept[0], gtob.eta(), gtob.EtDouble() );
    }

  }

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there
  count.setSizeCount(counting);
  
  return TCS::StatusCode::SUCCESS;

}
