/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "LbyLTopoClusterHypo.h"


using CLHEP::GeV;

LbyLTopoClusterHypo::LbyLTopoClusterHypo(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Et1Threshold", m_et1Threshold = 1.5*GeV,  "Min Et of the 1st cluster (in MeV)");
  declareProperty("Et2Threshold" ,m_et2Threshold = 1.5*GeV,  "Min Et of the second cluster (in MeV)");
  declareProperty("DPhi", m_dPhiCut = 2.4, "Min Deltax phi of the pair to be accepted");

  declareMonitoredStdContainer("AllDPhi", m_allDPhi);
}

HLT::ErrorCode LbyLTopoClusterHypo::hltInitialize() { 
  ATH_MSG_DEBUG("Configuration wiht dPhi " << m_dPhiCut << " ET thresholds " << m_et1Threshold << " " << m_et2Threshold );
  return HLT::OK; 
}



LbyLTopoClusterHypo::~LbyLTopoClusterHypo() {}


HLT::ErrorCode LbyLTopoClusterHypo::hltFinalize( ) { return HLT::OK; }

HLT::ErrorCode LbyLTopoClusterHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  using namespace std;
  pass = false; // reject by default
  m_allDPhi.clear();

  const xAOD::CaloClusterContainer* clusters{0};
  if ( getFeature( outputTE, clusters ) != HLT::OK )  {
    ATH_MSG_DEBUG( "Missing clusters, rejecting" );
    return HLT::OK;
  }
  ATH_MSG_DEBUG( "Clusters present" << clusters->size() );

  auto passEt = [&]( double et1, const double  et2 ) {
    if ( et1 > m_et1Threshold and et2 > m_et2Threshold ) 
      return true;
    return et2 > m_et1Threshold and et1 > m_et2Threshold;
  };
  
   

  for ( auto cl1Iter = begin(*clusters); cl1Iter != end(*clusters); ++cl1Iter  ) {    
    for ( auto cl2Iter = cl1Iter + 1; cl2Iter != end(*clusters); ++cl2Iter ) {	
      if ( not passEt( (*cl1Iter)->et(), (*cl2Iter)->et() ) ) continue;
      
      const double dPhi = HLT::deltaPhi( (*cl1Iter)->phi(), (*cl2Iter)->phi() );
      m_allDPhi.push_back( dPhi );
      
      if ( std::fabs(dPhi) > m_dPhiCut ) {	
	pass=true;
	ATH_MSG_DEBUG( "found pair of clusters matching criteria et/phi " 
		       << (*cl1Iter)->et() << "/" <<  (*cl1Iter)->phi() << " and  "
		       << (*cl2Iter)->et() << "/" <<  (*cl2Iter)->phi() );
      }
    }
  }


  return HLT::OK;
}
