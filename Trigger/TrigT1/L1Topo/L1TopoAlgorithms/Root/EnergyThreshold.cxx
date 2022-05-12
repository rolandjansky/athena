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
#include "L1TopoEvent/jTETOBArray.h"
#include "L1TopoEvent/gXETOBArray.h"
#include "L1TopoEvent/gTETOBArray.h"
#include <cmath>


REGISTER_ALG_TCS(EnergyThreshold)

using namespace std; 

TCS::EnergyThreshold::EnergyThreshold(const std::string & name) : CountingAlg(name) {

    setNumberOutputBits(12); //To-Do: Make this flexible to adapt to the menu. Each counting requires more than one bit

}


TCS::StatusCode TCS::EnergyThreshold::initialize(){


    m_threshold = getThreshold();

    // book histograms
    std::string hname_accept = "EnergyThreshold_accept_EtaPt_"+m_threshold->name();
    bookHistMult(m_histAccept, hname_accept, "Mult_"+ m_threshold->name(), "E_{t} [GeV]", 200, 0, 1000);
  
    hname_accept = "EnergyThreshold_accept_counts_"+m_threshold->name();
    bookHistMult(m_histAccept, hname_accept, "Mult_"+m_threshold->name(), "counts", 15, 0, 15);

    return StatusCode::SUCCESS;
    
}

TCS::StatusCode TCS::EnergyThreshold::processBitCorrect(const TCS::InputTOBArray & input,
					 Count & count){
                        return process(input, count);
}


TCS::StatusCode TCS::EnergyThreshold::process( const TCS::InputTOBArray & input,
			       Count & count )
{


  if (m_threshold->type() == "jXE") {

    // Grab the threshold and cast it into the right type
    const auto& jXEThr = dynamic_cast<const TrigConf::L1Threshold_jXE &>(*m_threshold);
    // Grab inputs
    const jXETOBArray & jXEArray = dynamic_cast<const jXETOBArray&>(input);

    int counting = 0; 
    
    // Loop over input TOBs
    for(jXETOBArray::const_iterator jxe = jXEArray.begin();
        jxe != jXEArray.end();
        ++jxe ) {

      bool passed = (*jxe)->Et2() > std::pow(jXEThr.thrValue100MeV(), 2);
      if (passed) {
        counting++;
        fillHist1D(m_histAccept[0], ((*jxe)->EtDouble()));
      }
    }

    fillHist1D( m_histAccept[1], counting);
    
    // Pass counting to TCS::Count object - output bits are composed there
    count.setSizeCount(counting);

  }

  else if (m_threshold->type() == "jTE") {

    // Grab the threshold and cast it into the right type
    const auto& jTEThr = dynamic_cast<const TrigConf::L1Threshold_jTE &>(*m_threshold);
    // Grab inputs
    const jTETOBArray & jTEArray = dynamic_cast<const jTETOBArray&>(input);

    int counting = 0; 
    
    // Loop over input TOBs
    for(jTETOBArray::const_iterator jte = jTEArray.begin();
        jte != jTEArray.end();
        ++jte ) {

      bool passed = (*jte)->sumEt() > jTEThr.thrValue100MeV();
      if (passed) {
        counting++;
        fillHist1D(m_histAccept[0], ((*jte)->sumEtDouble()));
      }
    }

    fillHist1D( m_histAccept[1], counting);
    
    // Pass counting to TCS::Count object - output bits are composed there
    count.setSizeCount(counting);

  }

  else if (m_threshold->type() == "gXE") {

    // Grab the threshold and cast it into the right type
    const auto& gXEThr = dynamic_cast<const TrigConf::L1Threshold_gXE &>(*m_threshold);
    // Grab inputs
    const gXETOBArray & gXEArray = dynamic_cast<const gXETOBArray&>(input);

    int counting = 0; 
    
    // Loop over input TOBs
    for(gXETOBArray::const_iterator gxe = gXEArray.begin();
        gxe != gXEArray.end();
        ++gxe ) {

      bool passed = (*gxe)->Et2() > std::pow(gXEThr.thrValue100MeV(), 2);
      if (passed) {
        counting++;
        fillHist1D(m_histAccept[0], ((*gxe)->EtDouble()));
      }
    }

    fillHist1D( m_histAccept[1], counting);
    
    // Pass counting to TCS::Count object - output bits are composed there
    count.setSizeCount(counting);

  }

  else if (m_threshold->type() == "gTE") {

    // Grab the threshold and cast it into the right type
    const auto& gTEThr = dynamic_cast<const TrigConf::L1Threshold_gTE &>(*m_threshold);
    // Grab inputs
    const gTETOBArray & gTEArray = dynamic_cast<const gTETOBArray&>(input);

    int counting = 0; 
    
    // Loop over input TOBs
    for(gTETOBArray::const_iterator gte = gTEArray.begin();
        gte != gTEArray.end();
        ++gte ) {

      bool passed = (*gte)->sumEt() > gTEThr.thrValue100MeV();
      if (passed) {
        counting++;
        fillHist1D(m_histAccept[0], ((*gte)->sumEtDouble()));
      }
    }

    fillHist1D( m_histAccept[1], counting);
    
    // Pass counting to TCS::Count object - output bits are composed there
    count.setSizeCount(counting);

  }

  else { std::cout << "WARNING: threshold type " << m_threshold->type() << " not included in EnergyThreshold algorithm. Skipping." << std::endl; }

  return TCS::StatusCode::SUCCESS;
}
