/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * EnergyThreshold.cpp
 * Created by Jack Harrison on 16/12/21.
 *
 * @brief algorithm that computes the multiplicity for a specified list and ET threshold
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading MinET

**********************************/


#include <L1TopoAlgorithms/EnergyThreshold.h>
#include "L1TopoCommon/Exception.h" 
#include "L1TopoInterfaces/Count.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jXETOBArray.h"
#include <cmath>


REGISTER_ALG_TCS(EnergyThreshold)

using namespace std; 

TCS::EnergyThreshold::EnergyThreshold(const std::string & name) : CountingAlg(name) {

    setNumberOutputBits(12); //To-Do: Make this flexible to adapt to the menu. Each counting requires more than one bit

}


TCS::StatusCode TCS::EnergyThreshold::initialize(){


    m_threshold = getThreshold();

    // book histograms
    bool isMult = true; 

    std::string hname_accept = "EnergyThreshold_accept_ET_"+m_threshold->name();

    bookHist(m_histAccept, hname_accept, "ET", 200, 0, 200, isMult);

    hname_accept = "EnergyThreshold_accept_counts_"+m_threshold->name();

    bookHist(m_histAccept, hname_accept, "COUNTS", 15, 0., 10., isMult);

    return StatusCode::SUCCESS;
    
}

TCS::StatusCode TCS::EnergyThreshold::processBitCorrect(const TCS::InputTOBArray & input,
					 Count & count){
                        return process(input, count);
}


TCS::StatusCode TCS::EnergyThreshold::process( const TCS::InputTOBArray & input,
			       Count & count )
{


  // Grab the threshold and cast it into the right type
  auto jXEThr = dynamic_cast<const TrigConf::L1Threshold_jXE &>(*m_threshold);
  // Grab inputs
  const jXETOBArray & jXEArray = dynamic_cast<const jXETOBArray&>(input);

  int counting = 0; 
  
  // loop over input TOBs
  for(jXETOBArray::const_iterator jxe = jXEArray.begin();
      jxe != jXEArray.end();
      ++jxe ) {

    bool passed = (*jxe)->Et() >= jXEThr.thrValue100MeV();
    if (passed) {
      counting++;
      fillHist1D(m_histAccept[0], ((*jxe)->Et()/10.));
    }
  }

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there
  count.setSizeCount(counting);
  
  return TCS::StatusCode::SUCCESS;
}
