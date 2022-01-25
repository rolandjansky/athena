/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * XEMultiplicityAlgo.cpp
 * Created by Jack Harrison on 16/12/21.
 *
 * @brief algorithm that computes the multiplicity for a specified list and ET threshold
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading MinET

**********************************/


#include <L1TopoAlgorithms/XEMultiplicityAlgo.h>

#include "L1TopoCommon/Exception.h"   //Why do we need this?
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/jLargeRJetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include "L1TopoEvent/MetTOBArray.h"


REGISTER_ALG_TCS(XEMultiplicityAlgo)

using namespace std; 

TCS::XEMultiplicityAlgo::XEMultiplicityAlgo(const std::string & name) : CountingAlg(name) {

    setNumberOutputBits(12); //To-Do: Make this flexible to adapt to the menu. Each counting requires more than one bit

}


TCS::StatusCode TCS::XEMultiplicityAlgo::initialize(){


    m_threshold = getThreshold();

    // book histograms
    bool isMult = true; 

    std::string hname_accept = "XEMultiplicityAlgo_accept_Et_"+m_threshold->name();

    bookHist(m_histAccept, hname_accept, "Et", 200, 0, 2000, isMult);

    hname_accept = "XEMultiplicityAlgo_accept_counts_"+m_threshold->name();

    bookHist(m_histAccept, hname_accept, "COUNTS", 15, 0., 10., isMult);

    return StatusCode::SUCCESS;
    
}

TCS::StatusCode TCS::XEMultiplicityAlgo::processBitCorrect(const TCS::InputTOBArray & input,
					 Count & count){
                        return process(input, count);
}


TCS::StatusCode TCS::XEMultiplicityAlgo::process( const TCS::InputTOBArray & input,
			       Count & count )
{

  // Grab the threshold and cast it into the right type
  auto MetThr = dynamic_cast<const TrigConf::L1Threshold_jXE &>(*m_threshold);

  // Grab inputs
  const MetTOBArray & MetArray = dynamic_cast<const MetTOBArray&>(input);

  int counting = 0; 
  
  // loop over input TOBs
  for(MetTOBArray::const_iterator MET = MetArray.begin();
      MET != MetArray.end();
      ++MET ) {
    
    const GenericTOB mtob(**MET);

    // Dividing by 4 standing for converting eta from 0.025 to 0.1 granularity as it is defined in the menu as 0.1 gran.
    bool passed = mtob.Et() >= MetThr.thrValue100MeV(mtob.eta()/4);

    if (passed) {
      counting++; 
      fillHist1D(m_histAccept[0], mtob.EtDouble());
    }
  }

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there
  count.setSizeCount(counting);
  
  return TCS::StatusCode::SUCCESS;
}
