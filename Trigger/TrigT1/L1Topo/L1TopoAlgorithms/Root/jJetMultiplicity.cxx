/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * jJetMultiplicity.cpp
 * Created by Carlos Moreno on 17/09/21.
 *
 * @brief algorithm that computes the multiplicity for a specified list and ET threshold
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading MinET

**********************************/

#include <cmath>

#include "L1TopoAlgorithms/jJetMultiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"

REGISTER_ALG_TCS(jJetMultiplicity)

using namespace std;


TCS::jJetMultiplicity::jJetMultiplicity(const std::string & name) : CountingAlg(name)
{
   
   
   setNumberOutputBits(12); //To-Do: Make this flexible to addapt to the menu. Each counting requires more than one bit

}

TCS::jJetMultiplicity::~jJetMultiplicity(){}


TCS::StatusCode
TCS::jJetMultiplicity::initialize() { 

  m_threshold = getThreshold();

  // book histograms
  bool isMult = true;

  std::string hname_accept = "hjJetMultiplicity_accept_EtaPt"+m_threshold->name();
  bookHist(m_histAccept, hname_accept, "ETA vs PT", 150, -100, 100, 450, 0., 300., isMult);

  hname_accept = "hjJetMultiplicity_accept_counts_"+m_threshold->name();
  bookHist(m_histAccept, hname_accept, "COUNTS", 15, 0., 10., isMult);

  return StatusCode::SUCCESS;
     
}


TCS::StatusCode
TCS::jJetMultiplicity::processBitCorrect( const TCS::InputTOBArray & input,
					 Count & count)

{
   return process(input, count);
}

TCS::StatusCode
TCS::jJetMultiplicity::process( const TCS::InputTOBArray & input,
			       Count & count )
{

  // Grab the threshold and cast it into the right type
  auto jJThr = dynamic_cast<const TrigConf::L1Threshold_jJ &>(*m_threshold);

  // Grab inputs
  const jJetTOBArray & jjets = dynamic_cast<const jJetTOBArray&>(input);

  int counting = 0; 
  
  // loop over input TOBs
  for(jJetTOBArray::const_iterator jjet = jjets.begin();
      jjet != jjets.end();
      ++jjet ) {
    
    const GenericTOB gtob(**jjet);

    // Dividing by 4 standing for converting eta from 0.025 to 0.1 granularity as it is defined in the menu as 0.1 gran.
    bool passed = gtob.Et() >= jJThr.thrValue100MeV(gtob.eta()/4);

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
