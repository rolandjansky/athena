/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include "UltraCentralHypo.h"

UltraCentralHypo::UltraCentralHypo(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::HypoAlgo(name, pSvcLocator)
{

  declareMonitoredVariable("FCalTotalEt"        , m_Tot_Et            ,  0.);
  declareMonitoredVariable("FCalTotalEtPassing" , m_Tot_Et_passing    , -1.);

  declareProperty("FcalEtUpperBound",     m_FcalEt_max=1.e10, "Upper bound of passing values, negative means +inf.");
  declareProperty("FcalEtLowerBound",     m_FcalEt_min=-1.e10, "Lower bound of passing values, negative means -inf.");
  declareProperty("EtaMin",               m_Eta_min=-3.25, "Lower bound of slice taken into calculation");
  declareProperty("EtaMax",               m_Eta_max=3.25, "Upper bound of slice taken into calculation");
  declareProperty("isFgap",               m_isFgap=true, "Eta cuts for FGap or UCC chains");

}

HLT::ErrorCode UltraCentralHypo::hltInitialize() { return HLT::OK; }


UltraCentralHypo::~UltraCentralHypo() {
}


HLT::ErrorCode UltraCentralHypo::hltFinalize( ) { return HLT::OK; }

HLT::ErrorCode UltraCentralHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  pass = false;
  
  m_Tot_Et = 0; // this must be 0 because our loop starts from it
  m_Tot_Et_passing = -1e6; // this will only get assigned when we pass
  
  const xAOD::HIEventShapeContainer* evtShape;
  ///*
  if(getFeature(outputTE, evtShape) == HLT::OK && evtShape != 0){
    ATH_MSG_DEBUG("Got HIEventShapeContainer object successfully");
  }
  else {
    ATH_MSG_DEBUG("The HIEventShapeContainer object is inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  // */
 
  //Determine FCal ET
  int size=evtShape->size();
  for(int i=0;i<size;i++){
    const xAOD::HIEventShape *sh=evtShape->at(i);
    const float Et     =  sh->et();
    const float etaMin =  sh->etaMin();
    const float etaMax =  sh->etaMax();
    //ATH_MSG_DEBUG("ET " << Et << " etaMin " << etaMin << " etaMax " << etaMax);
    if(Et==0) continue;
    
    //For Fgap chains
    if (m_isFgap){
		if ( etaMin < m_Eta_min ) continue;
		if ( etaMax > m_Eta_max ) continue;
	}
	//For UCC chains
	else{	
		//if ( fabs(etaMin) < m_Eta_min ) continue;
		float eta=(etaMin+etaMax)/2.0;
		if(fabs(eta)<m_Eta_min) continue;//FCal Only
    }
    m_Tot_Et+=Et;
  }
  
  // now cutting
  if ( m_FcalEt_min <= m_Tot_Et and m_Tot_Et < m_FcalEt_max ) {
    pass = true;
    m_Tot_Et_passing = m_Tot_Et;
  }

  return HLT::OK;
}
