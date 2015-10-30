/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//ElectronUEMonitoring.cxx
//#include ""
#include "ElectronUEMonitoring.h"


ElectronUEMonitoring::ElectronUEMonitoring(const std::string& name, ISvcLocator* pSvcLocator) : HLT::FexAlgo(name, pSvcLocator){
  declareProperty("UECollectionKey", m_UECollectionKey="HLT_xAOD__HIEventShapeContainer_HIUE", "HIEventShapeContainer Key from where the FCalET is to be taken");

  declareProperty("EtCutLow", m_EtCutLow=10*GeV);
  declareProperty("EtCutMedium", m_EtCutMedium=20*GeV);

  
  declareMonitoredVariable("EtEFLow"        , m_EtEFLow        ,  -999.);
  declareMonitoredVariable("EtL2Low"        , m_EtL2Low        ,  -999.);
  declareMonitoredVariable("EtDiffLow"        , m_EtDiffLow        ,  -999.);

  declareMonitoredVariable("EtEFMedium"        , m_EtEFMedium        ,  -999.);
  declareMonitoredVariable("EtL2Medium"        , m_EtL2Medium        ,  -999.);
  declareMonitoredVariable("EtDiffMedium"        , m_EtDiffMedium        ,  -999.);

  declareMonitoredVariable("EtEFHigh"        , m_EtEFHigh        ,  -999.);
  declareMonitoredVariable("EtL2High"        , m_EtL2High        ,  -999.);
  declareMonitoredVariable("EtDiffHigh"        , m_EtDiffHigh        ,  -999.);

  declareMonitoredVariable("FcalEt"        , m_FcalEt        ,  -1.);



}

HLT::ErrorCode ElectronUEMonitoring::hltInitialize() { return HLT::OK; }

ElectronUEMonitoring::~ElectronUEMonitoring() {
}

HLT::ErrorCode ElectronUEMonitoring::hltFinalize( ) { return HLT::OK; }


HLT::ErrorCode ElectronUEMonitoring::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE) {

  
  const xAOD::HIEventShapeContainer* evtShape;
  StatusCode sc = store()->retrieve(evtShape, m_UECollectionKey);
  if( sc.isSuccess()){
    ATH_MSG_DEBUG("HIEventShapeContainer object retrieved successfully");
  }
  else {
    ATH_MSG_DEBUG("The HIEventShapeContainer object is inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  const xAOD::CaloClusterContainer* ef;


  //Level EF
  if(getFeature(outputTE, ef) == HLT::OK && ef !=0){
    ATH_MSG_DEBUG("ElectronContainer retrived succesfully");
  }
  else {
    ATH_MSG_DEBUG("ElectronContainer object is inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  if ( ef->size() != 1 )  {
    ATH_MSG_DEBUG("Do not know what to do with multiple clusters " << ef->size());
  }

  //Level L2  
  const xAOD::TrigEMCluster* l2;
  if(getFeature(outputTE, l2) == HLT::OK && l2 !=0){
    ATH_MSG_DEBUG("TrigElectronContainer retrived succesfully");
  }
  else {
    ATH_MSG_DEBUG("TrigElectronContainer object is inaccessible");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  // obtain highest EF cluster Et in the RoI (this is mostly belts and suspenders as there is normally only one cluster)
  double maxEFEt = -999.;
  for ( auto cl: *ef) {
    if ( cl->et() > maxEFEt ) 
      maxEFEt = cl->et();
  }
  
  double maxL2Et = l2->et();
  
  // obtain the sum of FCal Et
  auto withinFcalRange = [](float eta) { return 3.2 < std::abs(eta) and std::abs(eta) < 4.9; };


  double fcalEt = 0.;
  for ( auto &slice: *evtShape ) {
    if ( withinFcalRange(slice->etaMin()) and withinFcalRange(slice->etaMax()) ) {
      fcalEt += slice->et();
    }
  }
  // get ot to TeV scale
  m_FcalEt =  fcalEt/TeV;
  
    
  // fill the monitoring variables
  if ( maxEFEt < m_EtCutLow )  {
    m_EtEFLow = maxEFEt/GeV;
    m_EtL2Low = maxL2Et/GeV;
    m_EtDiffLow = m_EtEFLow - m_EtL2Low;
  } else if ( m_EtCutLow <= maxEFEt and maxEFEt < m_EtCutMedium )  {
    m_EtEFMedium = maxEFEt/GeV;
    m_EtL2Medium = maxL2Et/GeV;
    m_EtDiffMedium = m_EtEFMedium - m_EtL2Medium;
  } else {
    m_EtEFHigh = maxEFEt/GeV;
    m_EtL2High = maxL2Et/GeV;    
    m_EtDiffHigh = m_EtEFHigh - m_EtL2High;
  }

  //  ATH_MSG_DEBUG("Monitored FCalEt" << m_FcalEt << " EtDiff Low: " << m_EtDiffLow << " Medium: " << m_EtDiffMedium << " High: " << m_EtDiffHigh);
  
  return HLT::OK;
}

