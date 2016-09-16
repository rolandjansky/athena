/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include "VnHypo.h"
#include "VnHypoHelper.h"

VnHypo::VnHypo(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::HypoAlgo(name, pSvcLocator),
    m_decision(2, 0) // 2 times 0
{


  declareMonitoredVariable("TotalEt"        , m_Tot_Et            ,  0.);
  declareMonitoredVariable("TotalEtPassing" , m_Tot_Et_passing    , -1.);  
  declareMonitoredVariable("icent"          , m_icent             , -1 );
  declareMonitoredVariable("icentPassing"   , m_icent_passing     , -1.);
  declareMonitoredVariable("qnx"            , m_qnx              , -1);
  declareMonitoredVariable("qny"            , m_qny               , -1);
  declareMonitoredVariable("q"              , m_qn                ,  0.);
  declareMonitoredVariable("qPassing"       , m_qn_passing        , -1.);
  declareMonitoredStdContainer("decision"       , m_decision);

  declareProperty("CentralityBins",  m_helper.centcuts, "Bins of FcalEt for which the thresholds are set");
  declareProperty("QThresholds",  m_helper.thresholds, "Values of thresholds above which the hypo accepts, size must match CentralityBins");
  declareProperty("QxShifts",  m_helper.qxshifts, "Values of q shifts along the x coordinate");
  declareProperty("QyShifts",  m_helper.qyshifts, "Values of q shifts along the y coordinate");
  declareProperty("FlowHarmonic"  , m_FlowHarmonic  =2                     , "The Order of the flow harmonic (2/3)");
  declareProperty("UpperLimit", m_upperLimit=false, "Apply q < threshold if true");
  declareProperty("MinEta", m_minEta=-5, "Minimum eta requires from ES shape slice to be included");
  declareProperty("MaxEta", m_maxEta=5,   "Maximum eta requires from ES shape slice to be included");
}

HLT::ErrorCode VnHypo::hltInitialize() {
  if ( not m_helper.valid() ) {
    ATH_MSG_ERROR("VnHypoHelper missocnfigured, likely centrality and vn thresholds vectors missmatch");
    return HLT::ERROR;      
  }
  
  std::transform( m_helper.centcuts.begin(), m_helper.centcuts.end(),
		  m_helper.centcuts.begin(), [](float x){ return x*1.e6; } ); // convert thresholds to MeV 
    
  return HLT::OK; 
}


VnHypo::~VnHypo() {
}


HLT::ErrorCode VnHypo::hltFinalize( ) { return HLT::OK; }

HLT::ErrorCode VnHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  pass = false;

  m_Tot_Et = 0; // this must be 0 because our loop starts from it
  m_qn     = 0;
  m_icent  =-1;
  m_Tot_Et_passing = -1; // this will only get assigned when we pass
  m_qn_passing     = -1;
  m_icent_passing  = -1;
  m_decision[0] = -1;
  m_decision[1] = -1;

  const xAOD::HIEventShapeContainer* evtShape;
  if(getFeature(outputTE, evtShape) == HLT::OK && evtShape != 0){
    ATH_MSG_DEBUG("Got HIEventShapeContainer object successfully");
  }
  else {
    ATH_MSG_DEBUG("The HIEventShapeContainer object is inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
   
  //Determine FCal ET and q-vectors
  m_qnx=m_qny=0;
  int FlowHarmonic_index=m_FlowHarmonic-1;//The index in the vectors starts from 0 for q1
  int size=evtShape->size();
  for(int i=0;i<size;i++){
    const xAOD::HIEventShape *sh=evtShape->at(i);
    float Et     =  sh->et();
    if(Et==0) continue;

    float eta=(sh->etaMin()+sh->etaMax())*0.5;
    if ( std::fabs(eta)<3.2 ) continue;//FCal Only
    if ( eta < m_minEta ) continue; // relevant for assumetric cuts only
    if ( eta > m_maxEta ) continue; // relevant for assumetric cuts only

    const std::vector<float> &c1=sh->etCos();
    const std::vector<float> &s1=sh->etSin();

    m_qnx+=c1[FlowHarmonic_index];
    m_qny+=s1[FlowHarmonic_index];
    m_Tot_Et+=Et;
  }
  
  m_icent=m_helper.getCentBin(m_Tot_Et);
  if(m_icent == -1) return HLT::OK;

  m_qnx /= m_Tot_Et;
  m_qny /= m_Tot_Et;
  m_qnx -= m_helper.getQxShift(m_icent);
  m_qny -= m_helper.getQyShift(m_icent);
  m_qn = hypot(m_qnx, m_qny);
   
  m_decision[0] = 0;
  // now cutting
  float Vn_Threshold=m_helper.getThreshold(m_icent);
  //  ATH_MSG_INFO("Et " << m_Tot_Et << " icent " << m_icent << " threshold " << Vn_Threshold);
  if ( (m_upperLimit == true and m_qn < Vn_Threshold)
       or (m_upperLimit == false and m_qn > Vn_Threshold) ) {
    pass             = true;
    m_Tot_Et_passing = m_Tot_Et;
    m_qn_passing     = m_qn;
    m_icent_passing  = m_icent;
  }
  m_decision[1] = (pass ? 1 : -1);
  return HLT::OK;
}
