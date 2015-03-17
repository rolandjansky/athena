/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include "VnHypo.h"
#include "VnHypoHelper.h"

VnHypo::VnHypo(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::HypoAlgo(name, pSvcLocator) {

  declareMonitoredVariable("TotalEt"        , m_Tot_Et            ,  0.);
  declareMonitoredVariable("TotalEtPassing" , m_Tot_Et_passing    , -1.);
  declareMonitoredVariable("icent"          , m_icent             , -1 );
  declareMonitoredVariable("icentPassing"   , m_icent_passing     , -1.);
  declareMonitoredVariable("q"              , m_qn                ,  0.);
  declareMonitoredVariable("qPassing"       , m_qn_passing        , -1.);


  declareProperty("ThresholdVnBin", m_ThresholdVnBin=VnHypoHelper::TOP_02_5, "The Vn percentile bin on which cut is performed"); 
  declareProperty("FlowHarmonic"  , m_FlowHarmonic  =2                     , "The Order of the flow harmonic (2/3)"); 
}

HLT::ErrorCode VnHypo::hltInitialize() { 
   if( m_ThresholdVnBin>VnHypoHelper::TOP_00_0){
      ATH_MSG_ERROR("Threshold bin is out of range");
      return HLT::ERROR;
   }
   if( m_FlowHarmonic!=2 && m_FlowHarmonic!=3 ){
      ATH_MSG_ERROR("Wrong order or harmonic used ::"<<m_FlowHarmonic);
      return HLT::ERROR;
   }
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
 /*
   if(evtStore()->retrieve(evtShape).isFailure())
   {
      ATH_MSG_ERROR(" Could not retrieve HIEventShapeContainer");
      return HLT::ERROR;
   }
// */

   //Determine FCal ET and q-vectors
   double qn_x=0,qn_y=0;
   int FlowHarmonic_index=m_FlowHarmonic-1;//The index in the vectors starts from 0 for q1
   int size=evtShape->size();
   for(int i=0;i<size;i++){
      const xAOD::HIEventShape *sh=evtShape->at(i);
      float Et     =  sh->Et();
      if(Et==0) continue;

      float eta=(sh->etaMin()+sh->etaMax())/2.0;
      if(fabs(eta)<3.2) continue;//FCal Only

      const std::vector<float> &c1=sh->Et_cos();
      const std::vector<float> &s1=sh->Et_sin();

      qn_x+=c1[FlowHarmonic_index];
      qn_y+=s1[FlowHarmonic_index];
      m_Tot_Et+=Et;
   }

   m_icent=VnHypoHelper::GetCentBin(m_Tot_Et/1.0e6,VnHypoHelper::DATA_2011);//1.0e6 is TeV conversion
   if(m_icent==-1) return HLT::OK;

   m_qn=sqrt(qn_x*qn_x + qn_y*qn_y)/m_Tot_Et;


  // now cutting
  float Vn_Threshold=1000;//large number so trigger fails by default
  if     (m_FlowHarmonic==2) Vn_Threshold=VnHypoHelper::FCal_Combined_Thresholds_q2[m_icent][m_ThresholdVnBin];
  else if(m_FlowHarmonic==3) Vn_Threshold=VnHypoHelper::FCal_Combined_Thresholds_q3[m_icent][m_ThresholdVnBin];

  if ( m_qn > Vn_Threshold) {
    pass            = true;
    m_Tot_Et_passing=m_Tot_Et;
    m_qn_passing    =m_qn;
    m_icent_passing =m_icent;
  }
 
  return HLT::OK;
}
