/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloMonToolBase.h" 
#include "TH1F.h"

CaloMonToolBase::CaloMonToolBase(const std::string& type, const std::string& name,const IInterface* parent) 
  :ManagedMonitorToolBase(type, name, parent),
   m_lb(0),
   m_passBeamBackgroundRemoval(false),
   m_h_EvtRejSumm(nullptr),
   m_BadLBTool(this, "BadLBTool", "DQBadLBFilterTool"),
   m_ReadyFilterTool(this, "ReadyFilterTool", "DQAtlasReadyFilterTool")
 {
  declareProperty("useBadLBTool", m_useBadLBTool=false);

  declareProperty("useReadyFilterTool",m_useReadyFilterTool=true);

  declareProperty("useLArCollisionFilterTool",m_useCollisionFilterTool=true);

  declareProperty("useLArNoisyAlg",m_useLArNoisyAlg=false);

  declareProperty("useBeamBackgroundRemoval",m_useBeamBackgroundRemoval=false);
}
CaloMonToolBase::~CaloMonToolBase() {}

StatusCode CaloMonToolBase::initialize() {

  // initialize keys
  ATH_CHECK( m_EventInfoKey.initialize() );
  ATH_CHECK( m_LArCollisionTimeKey.initialize(m_useCollisionFilterTool) );
  ATH_CHECK( m_beamBackgroundKey.initialize() );

  // retrieve AtlasReadyFilter tool
  if(m_useReadyFilterTool){
   StatusCode sc = m_ReadyFilterTool.retrieve();
   if(sc.isFailure()) {
     ATH_MSG_ERROR("Could Not Retrieve AtlasReadyFilterTool " << m_ReadyFilterTool);
     m_useReadyFilterTool = false;
     return sc;
   }
  }
  else {
    m_ReadyFilterTool.disable();
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
 else {
   m_BadLBTool.disable();
 } 
 return StatusCode::SUCCESS;
}



StatusCode CaloMonToolBase::bookBaseHists(MonGroup* group) {
  m_h_EvtRejSumm  = new TH1I("nEvtsRejectByDifferentTool","Total Events: bin 1, ATLAS Ready: 2, && Good LAr LB: 3, && No LAr collision: 4, && No Beam Background: 5, && No Trigger Filer: 6, && No LArError: 7 ",7,1.,8.);
  m_h_EvtRejSumm->GetYaxis()->SetTitle("RejectedEvents");

  const std::array<const char*,7> binLabels={{"TotalEvents","ATLAS Ready","with Good LAr LB","with No LAr Collision","with No Beam Background", "with No Trigger Filter","with No LArError"}};

  for (unsigned i=0;i<binLabels.size();++i) {
    m_h_EvtRejSumm->GetXaxis()->SetBinLabel(i+1,binLabels[i]);
  }
  if (!m_useReadyFilterTool){
    m_h_EvtRejSumm->GetXaxis()->SetBinLabel(2,"ATLAS Ready-OFF");
  }
  if (!m_useBadLBTool){
    m_h_EvtRejSumm->GetXaxis()->SetBinLabel(3,"Good LAr LB-OFF");
  }
  if (!m_useCollisionFilterTool){
    m_h_EvtRejSumm->GetXaxis()->SetBinLabel(4,"LAr collision-OFF");
  }
  if (!m_useBeamBackgroundRemoval){
    m_h_EvtRejSumm->GetXaxis()->SetBinLabel(5,"Beam backgr.-OFF");
  }
  if (!m_useLArNoisyAlg){
    m_h_EvtRejSumm->GetXaxis()->SetBinLabel(7,"LAr Error Veto-OFF");
  }

  return group->regHist( m_h_EvtRejSumm );
}


StatusCode CaloMonToolBase::checkFilters(bool& ifPass){

  // ATH_MSG_INFO("CaloCellVecMon::checkFilters() starts");
  StatusCode sc = StatusCode::SUCCESS;

  m_h_EvtRejSumm->Fill(1); //Counter of all events seen

  ifPass = 0;   
  if (m_useReadyFilterTool) {
    if (m_ReadyFilterTool->accept()) {
      ifPass = 1;
      m_h_EvtRejSumm->Fill(2); //All events with ATLAS Ready
    }
  }
  else{
    m_h_EvtRejSumm->Fill(2); //ATLAS ready not activated
    ifPass = 1;
  }

  SG::ReadHandle<xAOD::EventInfo> eventInfo{m_EventInfoKey};
  if (!eventInfo.isValid()) {
    ATH_MSG_ERROR("Event info not found !" );
    return StatusCode::SUCCESS;
  }

  m_lb = eventInfo->lumiBlock();
  if (m_useBadLBTool) {
    if (m_BadLBTool->accept()) { 
      ifPass = ifPass && 1;
      if(ifPass) m_h_EvtRejSumm->Fill(3); //All events with ATLAS Ready and Good LB
    }
    else {
      ifPass = 0;
    }
  }
  else{
    if(ifPass) m_h_EvtRejSumm->Fill(3); 
  }

  // Filter the events identfied as collision by the LAr system
  // Useful in CosmicCalo to reject collision candidates
  if (m_useCollisionFilterTool){
    SG::ReadHandle<LArCollisionTime> larTime{m_LArCollisionTimeKey};
    if(!larTime.isValid()){
      ATH_MSG_WARNING("Unable to retrieve LArCollisionTime event store");
      if(ifPass) m_h_EvtRejSumm->Fill(4); 
    }
    else {
      if (larTime->timeC()!=0 && larTime->timeA()!=0 && std::fabs(larTime->timeC() - larTime->timeA())<10)  {
	ifPass = 0;
      }
      else { 
	ifPass = ifPass && 1;
	if(ifPass) m_h_EvtRejSumm->Fill(4); //All events with ATLAS Ready and Good LB and Good LAr collision time
      }
    }
  }
  else{
    if(ifPass) m_h_EvtRejSumm->Fill(4); 
  }


  m_passBeamBackgroundRemoval=true;
  if(m_useBeamBackgroundRemoval){
    SG::ReadHandle<BeamBackgroundData> beamBackgroundData{m_beamBackgroundKey};
    if(!beamBackgroundData.isValid()){
      ATH_MSG_WARNING("Unable to retrieve BeamBackgroundData");
    }
    else {
      if( beamBackgroundData->GetNumSegment() > 0 ) {
        m_passBeamBackgroundRemoval = false;
	ifPass = 0;
        ATH_MSG_DEBUG("Identified background event");
      }
      else {
        m_passBeamBackgroundRemoval = true;
        ifPass = ifPass && 1;
        if(ifPass) m_h_EvtRejSumm->Fill(5); //All events with ATLAS Ready and Good LB and Good LAr collision time and not Beam Background
      }
    }
  }
  else {  // Do not use BeamBackgroundRemoval
    ifPass = ifPass && 1;
    if(ifPass) m_h_EvtRejSumm->Fill(5); //All events with ATLAS Ready and Good LB and Good LAr collision time and not Beam Background
  
  }

  std::string TheTrigger;
  if (m_triggerChainProp == "") {  
    TheTrigger="NoTrigSel";
    ifPass = ifPass && 1; // No Trigger Filter check
    if(ifPass) m_h_EvtRejSumm->Fill(6); //All events with ATLAS Ready and Good LB and Good LAr collision time and not Beam Background and Trigger Filter pass
  }
  else {
    TheTrigger = m_triggerChainProp; // Trigger Filter not implemented ++ FIXME ==
    if(ifPass) m_h_EvtRejSumm->Fill(6); 
  }

  if(m_useLArNoisyAlg && (eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error)) {
    ifPass = 0;
  }
  else {
    ifPass = ifPass && 1;
    if(ifPass) m_h_EvtRejSumm->Fill(7); //All events with ATLAS Ready and Good LB and Good LAr collision time and not Beam Background and Trigger Filter pass and no Lar Error
  }

  ATH_MSG_VERBOSE("CaloMonBase::checkFilters() is done");
  return StatusCode::SUCCESS;
}
