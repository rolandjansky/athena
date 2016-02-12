/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloMonitoring/CaloMonToolBase.h" 
#include "xAODEventInfo/EventInfo.h"
#include "LArRecEvent/LArCollisionTime.h"
#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "TH1F.h"

CaloMonToolBase::CaloMonToolBase(const std::string& type, const std::string& name,const IInterface* parent) 
  :ManagedMonitorToolBase(type, name, parent),
   m_lb(0),
   m_passBeamBackgroundRemoval(false),
   m_h_EvtRejSumm(nullptr),
   m_BadLBTool("DQBadLBFilterTool"),
   m_ReadyFilterTool("DQAtlasReadyFilterTool")
 {
  declareProperty("useBadLBTool", m_useBadLBTool=false);
  declareProperty("BadLBTool", m_BadLBTool);
  declareProperty("useReadyFilterTool",m_useReadyFilterTool=true);
  declareProperty("ReadyFilterTool",m_ReadyFilterTool);
  declareProperty("useLArNoisyAlg",m_useLArNoisyAlg=false);
  declareProperty("useBeamBackgroundRemoval",m_useBeamBackgroundRemoval=false);
 //Key of the beam background object
  declareProperty("BeamBackgroundKey",m_beamBackgroundKey="CSCBackgroundForCaloMon");
}
CaloMonToolBase::~CaloMonToolBase() {}

StatusCode CaloMonToolBase::initialize() {

  // retrieve AtlasReadyFilter tool
  if(m_useReadyFilterTool){
   StatusCode sc = m_ReadyFilterTool.retrieve();
   if(sc.isFailure()) {
     ATH_MSG_ERROR("Could Not Retrieve AtlasReadyFilterTool " << m_ReadyFilterTool);
     m_useReadyFilterTool = false;
     return sc;
   }
   ATH_MSG_INFO("AtlasReadyFilterTool retrieved");
  }

  // retrieve BadLBFilter tool 
 if(m_useBadLBTool){
  StatusCode sc = m_BadLBTool.retrieve();
  if(sc.isFailure()){
    ATH_MSG_ERROR("Unable to retrieve the DQBadLBFilterTool");
    m_useBadLBTool = false;
    return sc;
  }
  ATH_MSG_INFO("DQBadLBFilterTool retrieved");
 }
 return StatusCode::SUCCESS;
}



StatusCode CaloMonToolBase::bookBaseHists(MonGroup* group) {
  m_h_EvtRejSumm  = new TH1I("nEvtsRejectByDifferentTool","Total Events: bin 1, ReadyFilterTool: 2, BadLBTool: 3, LArCollisionTime: 4, BeamBackground: 5, Trigger: 6, LArError: 7 ",7,1.,8.);
  m_h_EvtRejSumm->GetYaxis()->SetTitle("RejectedEvents");

  const std::array<const char*,7> binLabels={{"TotalEvents","ReadyFilterTool","BadLBTool","LArCollisionTime","BeamBackground", "Trigger","LArError"}};

  for (unsigned i=0;i<binLabels.size();++i) {
    m_h_EvtRejSumm->GetXaxis()->SetBinLabel(i+1,binLabels[i]);
  }
  return group->regHist( m_h_EvtRejSumm );
}


StatusCode CaloMonToolBase::checkFilters(bool& ifPass){

  // ATH_MSG_INFO("CaloCellVecMon::checkFilters() starts");
  StatusCode sc = StatusCode::SUCCESS;

  m_h_EvtRejSumm->Fill(1); //Counter of all events seen

  const xAOD::EventInfo* eventInfo;
  sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Event info not found !" );
    return sc;
  }
  m_lb = eventInfo->lumiBlock();
 
  if(m_useLArNoisyAlg && (eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error)) {
    ifPass = 0;
    m_h_EvtRejSumm->Fill(7);
  }
  //Fixme fill m_h_EvtRejSumm (define new value)

   
  if (m_useReadyFilterTool) {
    if (!m_ReadyFilterTool->accept()) {
      ifPass = 0;
      m_h_EvtRejSumm->Fill(2);
    }
  }
   
  if (m_useBadLBTool) {
    if (!m_BadLBTool->accept()) { 
      ifPass = 0;
      m_h_EvtRejSumm->Fill(3);
    }
  }
  
 const LArCollisionTime * larTime;
 sc = evtStore()->retrieve(larTime,"LArCollisionTime");
 if(sc.isFailure()){
   ATH_MSG_WARNING("Unable to retrieve LArCollisionTime event store");
 }
 else {
   if (larTime->timeC()!=0 && larTime->timeA()!=0 && std::fabs(larTime->timeC() - larTime->timeA())<10)  {
     ifPass = 0;
     m_h_EvtRejSumm->Fill(4);
   }
 }

 m_passBeamBackgroundRemoval=true;
 if(m_useBeamBackgroundRemoval){
   const BeamBackgroundData* beamBackgroundData;
   sc = evtStore()->retrieve(beamBackgroundData, m_beamBackgroundKey);// "BeamBackgroundData");
   if(sc.isFailure()){
     ATH_MSG_WARNING("Unable to retrieve BeamBackgroundData");
   }
   else {
     //   ATH_MSG_INFO("BeamBackgroundData is retrieved");
     if( beamBackgroundData->GetNumSegment() > 0 ) {
       m_passBeamBackgroundRemoval = false;
       m_h_EvtRejSumm->Fill(5);
       ATH_MSG_DEBUG("Identified background event");
     }
   }
 }
 ATH_MSG_VERBOSE("CaloMonBase::checkFilters() is done");
 return StatusCode::SUCCESS;
}
