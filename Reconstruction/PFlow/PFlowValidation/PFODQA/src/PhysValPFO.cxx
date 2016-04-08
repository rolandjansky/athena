/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValPFO.h"
#include "xAODPFlow/PFOContainer.h"

PhysValPFO::PhysValPFO (const std::string& type, const std::string& name, const IInterface* parent ) : ManagedMonitorToolBase( type, name, parent ), m_retrievePFOTool("RetrievePFOTool",this), m_useLCScale(false), m_useNeutralPFO(false), m_fillEMHistograms(false) {
  declareProperty( "PFOContainerName", m_PFOContainerName="");
  declareProperty("RetrievePFOTool",m_retrievePFOTool,"Name of PFO getter");
  declareProperty("useLCScale",m_useLCScale, " Select which PFO setup to use - LC or EM ");
  declareProperty("useNeutralPFO", m_useNeutralPFO, "Select whether to use neutral or charged PFO");
  declareProperty("fillEMHistograms", m_fillEMHistograms, "Select whether to plot EM scale 4-vector");
}

PhysValPFO::~PhysValPFO() {}

StatusCode PhysValPFO::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_CHECK(m_retrievePFOTool.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode PhysValPFO::bookHistograms(){

  std::string scale = "EM";
  std::string type = "charged";

  if (true == m_useLCScale) scale = "LC";

  if (true == m_useNeutralPFO) type = "neutral";
  else scale = "";

  std::string theName = "PFlow/PFO_JetETMiss/JetETMiss_"+scale+"_"+type;

  m_PFOValidationPlots.reset(new PFOValidationPlots(0,theName, theName));
  m_PFOValidationPlots->setDetailLevel(100);
  m_PFOValidationPlots->initialize();
  std::vector<HistData> hists = m_PFOValidationPlots->retrieveBookedHistograms();

  for (auto hist : hists) {
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }
  
  return StatusCode::SUCCESS;     
   
}

StatusCode PhysValPFO::fillHistograms(){

  const xAOD::PFOContainer* thePFOContainer = NULL;

  if (false == m_useNeutralPFO) thePFOContainer = m_retrievePFOTool->retrievePFO(CP::EM,CP::charged);
  else{
    if (false == m_useLCScale) thePFOContainer = m_retrievePFOTool->retrievePFO(CP::EM,CP::neutral);
    else thePFOContainer = m_retrievePFOTool->retrievePFO(CP::LC,CP::neutral);
  }

  if (!thePFOContainer){
    if (msgLvl(MSG::WARNING))  msg(MSG::WARNING) << " Have NULL pointer to xAOD::PFOContainer " << endreq;
    return StatusCode::SUCCESS;
  }
  
  xAOD::PFOContainer::const_iterator firstPFO = thePFOContainer->begin();
  xAOD::PFOContainer::const_iterator lastPFO = thePFOContainer->end();

  for (; firstPFO != lastPFO; ++firstPFO) {
    const xAOD::PFO* thePFO = *firstPFO;
    m_PFOValidationPlots->fill(*thePFO, m_fillEMHistograms);
  }

  return StatusCode::SUCCESS;

}

StatusCode PhysValPFO::procHistograms(){
   return StatusCode::SUCCESS;
}
