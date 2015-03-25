/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTRTHTHhypo.h"
#include "xAODTrigRinger/TrigRNNOutput.h"


TrigTRTHTHhypo::TrigTRTHTHhypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_minTRTHTHitsRoad(0),
  m_minHTratioRoad(0.),
  m_minTRTHTHitsWedge(0),
  m_minHTratioWedge(0.),
  m_doWedge(1),
  m_minCaloE(0.)
{
  declareProperty("AcceptAll",     m_acceptAll = false ); 
  declareProperty("MinTRTHTHitsRoad",  m_minTRTHTHitsRoad = 20); // Changed from 40 to 20
  declareProperty("MinHTRatioRoad",    m_minHTratioRoad = 0.37); // Changed from 0.2 to 0.37 with new algo
  declareProperty("MinTRTHTHitsWedge",  m_minTRTHTHitsWedge = 20); // Changed from 40 to 20
  declareProperty("MinHTRatioWedge",    m_minHTratioWedge = 0.37); // Changed from 0.2 to 0.37 with new algo
  declareProperty("DoWedge",    m_doWedge=1);
}

//-----------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHhypo::hltInitialize() {

  ATH_MSG_INFO ( "Initialising TrigTRTHTHhypo: " << name());


  return HLT::OK;
}

//-----------------------------------------------------------------------------
HLT::ErrorCode TrigTRTHTHhypo::hltBeginRun() {
  ATH_MSG_DEBUG ( "hltBeginRun of " << name());

  return HLT::OK;
}
//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHhypo::hltExecute(const HLT::TriggerElement* outputTE, 
						    bool& pass) {
  pass = false;

  ATH_MSG_DEBUG ( "Executing this TrigTRTHTHhypo " << name());
  

  // 
  if( m_acceptAll ){
    pass=true;
    ATH_MSG_DEBUG ( "Accepting all events in " << name());
    return HLT::OK;
  }

  //get fex output
  const xAOD::TrigRNNOutput *out = 0;
  if (getFeature(outputTE, out, "TrigTRTHTCounts") != HLT::OK ||  (out == 0)) {
    return HLT::NAV_ERROR;
  }
 
  if( msg().level() <= MSG::DEBUG){
    static SG::AuxElement::Accessor< std::vector<float> >orig("trththits");
    if( !orig.isAvailable(*out)  ){
      ATH_MSG_ERROR ( "Problem with decorator.");
      return HLT::NAV_ERROR;
    }
  } 
  std::vector<float> vec = out->auxdata< std::vector<float> >("trththits");// decoration for now.



  float trththits_road = vec.at(0);  //Changed to cut on the number httrt and fraction httrt in the cone size of 0.015 in phi
  float trththits_wedge = vec.at(1);
  ATH_MSG_DEBUG ( "trththits_road: " << trththits_road);
  ATH_MSG_DEBUG ( "trththits_wedge: " << trththits_wedge);
  
  float ratio_road = trththits_road - floor(trththits_road);
  float ratio_wedge = trththits_wedge - floor(trththits_wedge);
  if (ratio_road > m_minHTratioRoad){
    if (trththits_road>m_minTRTHTHitsRoad)
      pass = true;
  }
  else if (ratio_wedge > m_minHTratioWedge && m_doWedge){
    if (trththits_wedge>m_minTRTHTHitsWedge)
      pass = true;
  }

    ATH_MSG_DEBUG ( name() << " hypo result " << pass);

  return HLT::OK;  
}

//-----------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHhypo::hltFinalize() {
  ATH_MSG_DEBUG ( " finalizing this TrigTRTHTHhypo : " << name()); 
  return HLT::OK;  
}
