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
  m_doWedge(0),
  m_doRoad(0),
  m_minCaloE(0.)
{
  declareProperty("AcceptAll",     m_acceptAll = false ); 
  declareProperty("MinTRTHTHitsRoad",  m_minTRTHTHitsRoad = 20); 
  declareProperty("MinHTRatioRoad",    m_minHTratioRoad = 0.4); 
  declareProperty("MinTRTHTHitsWedge",  m_minTRTHTHitsWedge = 20); 
  declareProperty("MinHTRatioWedge",    m_minHTratioWedge = 0.45);
  declareProperty("DoWedge",    m_doWedge=1);
  declareProperty("DoRoad", m_doRoad=0);
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



  float fHT_road = vec.at(1);
  float fHT_wedge = vec.at(3);
  float trththits_road = vec.at(0);
  float trththits_wedge = vec.at(2);

  ATH_MSG_DEBUG ( "trththits_road: " << trththits_road);
  ATH_MSG_DEBUG ( "trththits_wedge: " << trththits_wedge);
  ATH_MSG_DEBUG ( "fHT_road: " << fHT_road); 
  ATH_MSG_DEBUG ( "fHT_wedge: " << fHT_wedge);

  if (fHT_road > m_minHTratioRoad && m_doRoad){
    if (trththits_road>m_minTRTHTHitsRoad)
      pass = true;
  }
  if (fHT_wedge > m_minHTratioWedge && m_doWedge){
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
