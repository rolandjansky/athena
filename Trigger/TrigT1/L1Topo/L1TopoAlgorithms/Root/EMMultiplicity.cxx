/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * EMMultiplicity.cpp
 * Created by Carlos Moreno on 05/06/20.
 *
 * @brief algorithm that computes the multiplicity for a specified list and ET threshold
 * line 1: 0 or 1, line 1 and 2 : 2 or more, uses 2 bits
 *
 * @param NumberLeading MinET

**********************************/

#include <cmath>

#include "L1TopoAlgorithms/EMMultiplicity.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Count.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"

REGISTER_ALG_TCS(EMMultiplicity)

using namespace std;


TCS::EMMultiplicity::EMMultiplicity(const std::string & name) : CountingAlg(name)
{
   
   
   setNumberOutputBits(12); //To-Do: Make this flexible to addapt to the menu. Each counting requires more than one bit

}

TCS::EMMultiplicity::~EMMultiplicity(){}


TCS::StatusCode
TCS::EMMultiplicity::initialize() { 

  m_threshold = getThreshold();

  // book histograms
  std::string hname_accept = "hEMMultiplicity_accept_"+m_threshold->name();
  bookHist(m_histAccept, hname_accept, "eta vs pT", 100, -49., 49., 25, 0., 25.);

  hname_accept = "hEMMultiplicity_accept_counts_"+m_threshold->name();
  bookHist(m_histAccept, hname_accept, "Counts", 10, 0., 10. );

  return StatusCode::SUCCESS;
     
}


TCS::StatusCode
TCS::EMMultiplicity::processBitCorrect( const TCS::InputTOBArray & input,
					 Count & count)

{
           return process(input, count);
}

TCS::StatusCode
TCS::EMMultiplicity::process( const TCS::InputTOBArray & input,
			       Count & count )
{

  // Grab the threshold and cast it into the right type
  auto eEMThr = dynamic_cast<const TrigConf::L1Threshold_eEM &>(*m_threshold);

  // TO-DO: Add isolation cuts - need to be implemented in the L1Calo EDM first
  //cout << "reta: " << TrigConf::Selection::wpToString(eEMThr.reta()) << endl;
  //cout << "rhad: " << TrigConf::Selection::wpToString(eEMThr.rhad()) << endl;
  //cout << "wstot: " << TrigConf::Selection::wpToString(eEMThr.wstot()) << endl;

  // Grab inputs
  const ClusterTOBArray & clusters = dynamic_cast<const ClusterTOBArray&>(input);

  int counting = 0;
  float MEV = 1000.;
  
  // loop over input TOBs
  for(ClusterTOBArray::const_iterator cl = clusters.begin();
      cl != clusters.end();
      ++cl ) {
    
    const GenericTOB gtob(**cl);

    bool passed = false;
    for(auto & rv : eEMThr.thrValues())
      if ( (gtob.eta() < rv.etaMax()) && (gtob.eta() >= rv.etaMin()) && (gtob.Et()/MEV> static_cast<unsigned int>(rv.value())) ) passed = true;
      
    if (passed) {
      counting++; 
      fillHist2D( m_histAccept[0], gtob.eta(), gtob.Et()/MEV );
    }

  }

  fillHist1D( m_histAccept[1], counting);
  
  // Pass counting to TCS::Count object - output bits are composed there
  count.setSizeCount(counting);
  
  return TCS::StatusCode::SUCCESS;

}
