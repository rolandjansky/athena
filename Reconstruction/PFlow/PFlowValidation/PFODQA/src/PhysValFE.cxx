/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValFE.h"
#include "xAODPFlow/FlowElementContainer.h"

PhysValFE::PhysValFE (const std::string& type, const std::string& name, const IInterface* parent ) : 
  ManagedMonitorToolBase( type, name, parent ), 
  m_useNeutralFE(false)
{
  declareProperty("useNeutralFE", m_useNeutralFE, "Select whether to use neutral or charged FE");
}

PhysValFE::~PhysValFE() {}

StatusCode PhysValFE::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());  
  ATH_CHECK(m_vertexContainerReadHandleKey.initialize());
  ATH_CHECK(m_FEContainerHandleKey.initialize());  

  ATH_CHECK(m_muonChargedFEReadHandleKey.initialize());
  ATH_CHECK(m_muonNeutralFEReadHandleKey.initialize());

  ATH_CHECK(m_electronChargedFEReadHandleKey.initialize());
  ATH_CHECK(m_electronNeutralFEReadHandleKey.initialize());

  ATH_CHECK(m_photonChargedFEReadHandleKey.initialize());
  ATH_CHECK(m_photonNeutralFEReadHandleKey.initialize());

  ATH_CHECK(m_tauJetChargedFEReadHandleKey.initialize());
  ATH_CHECK(m_tauJetNeutralFEReadHandleKey.initialize());
  
  ATH_CHECK(m_eventInfoReadHandleKey.initialize());
  
  return StatusCode::SUCCESS;
}


StatusCode PhysValFE::bookHistograms(){
    
  std::string theName = "PFlow/FlowElement/"+m_FEContainerHandleKey.key();  
  std::vector<HistData> hists;
  std::vector<HistData> additional_hists;  
  if (!m_useNeutralFE){
    m_FEChargedValidationPlots.reset(new PFOChargedValidationPlots(0,theName,theName));
    m_FEChargedValidationPlots->setDetailLevel(100);
    m_FEChargedValidationPlots->initialize();
    hists = m_FEChargedValidationPlots->retrieveBookedHistograms();
    m_LeptonLinkerPlots_CFE.reset(new LeptonCFEValidationPlots(0,theName,theName));
    m_LeptonLinkerPlots_CFE->setDetailLevel(100);
    m_LeptonLinkerPlots_CFE->initialize();
    m_LeptonLinkerPlots_CFE->retrieveBookedHistograms();
    additional_hists=m_LeptonLinkerPlots_CFE->retrieveBookedHistograms();
   
  }
  else if (m_useNeutralFE){
    m_FENeutralValidationPlots.reset(new PFONeutralValidationPlots(0,theName, theName));
    m_FENeutralValidationPlots->setDetailLevel(100);
    m_FENeutralValidationPlots->initialize();
    hists = m_FENeutralValidationPlots->retrieveBookedHistograms();

    m_LeptonLinkerPlots_NFE.reset(new LeptonNFEValidationPlots(0,theName,theName));
    m_LeptonLinkerPlots_NFE->setDetailLevel(100);
    m_LeptonLinkerPlots_NFE->initialize();
    additional_hists=m_LeptonLinkerPlots_NFE->retrieveBookedHistograms();    

  }

  hists.insert(hists.end(),additional_hists.begin(),additional_hists.end()); // append lepton-FE linker plots to collection of hists  

  for (auto hist : hists) {
    ATH_MSG_DEBUG("Processing histogram named: "<<hist.first->GetName()<<" (title) "<<hist.first->GetTitle());
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }
  
  return StatusCode::SUCCESS;     
   
}

StatusCode PhysValFE::fillHistograms(){

  const xAOD::Vertex* theVertex = nullptr;
  
  if (!m_useNeutralFE){
    SG::ReadHandle<xAOD::VertexContainer> vertexContainerReadHandle(m_vertexContainerReadHandleKey);
    if(!vertexContainerReadHandle.isValid()){
      ATH_MSG_WARNING("Invalid ReadHandle for xAOD::VertexContainer with key: " << vertexContainerReadHandle.key());
    }
    else {
      //Vertex finding logic based on logic in JetRecTools/PFlowPseudoJetGetter tool
      //Usually the 0th vertex is the primary one, but this is not always the case. So we will choose the first vertex of type PriVtx
      for (auto vertex : *vertexContainerReadHandle) {
	      if (xAOD::VxType::PriVtx == vertex->vertexType() ) {
	      theVertex = vertex;
	      break;
	      }//If we have a vertex of type primary vertex
      }//iterate over the vertices and check their type

      if (nullptr == theVertex) ATH_MSG_WARNING("Did not find either a PriVtx or a NoVtx in this event");
      
    }//if valid read handle
  }

  SG::ReadHandle<xAOD::EventInfo> eventInfoReadHandle(m_eventInfoReadHandleKey);
  if (!eventInfoReadHandle.isValid()){
    ATH_MSG_WARNING("Invalid ReadHandle for xAOD::EventInfo with key: " << eventInfoReadHandle.key());
  }

  SG::ReadHandle<xAOD::FlowElementContainer> FEContainerReadHandle(m_FEContainerHandleKey);
  if(!FEContainerReadHandle.isValid()){
     ATH_MSG_WARNING("Invalid ReadHandle for xAOD::FlowElementContainer with key: " << FEContainerReadHandle.key());
     return StatusCode::SUCCESS;
  }

  for (auto theFE : *FEContainerReadHandle){
    if(theFE){
       if (!m_useNeutralFE) m_FEChargedValidationPlots->fill(*theFE,theVertex,*eventInfoReadHandle);
       else if (m_useNeutralFE) m_FENeutralValidationPlots->fill(*theFE,*eventInfoReadHandle);
    }
    else ATH_MSG_WARNING("Invalid pointer to xAOD::FlowElement");
  }  

  if(!m_useNeutralFE){
    SG::ReadDecorHandle<xAOD::MuonContainer,std::vector<ElementLink<xAOD::FlowElementContainer> > > MuonContainerReadDecorHandle(m_muonChargedFEReadHandleKey);
    if(!MuonContainerReadDecorHandle.isValid()){
      ATH_MSG_WARNING("Muon readhandle is a dud");    
    }
    else{
      for (auto Muon: *MuonContainerReadDecorHandle) m_LeptonLinkerPlots_CFE->fill(*Muon,*eventInfoReadHandle);
    } 
  }
  else{
    SG::ReadDecorHandle<xAOD::MuonContainer, std::vector<ElementLink<xAOD::FlowElementContainer> > > MuonContainerReadDecorHandle(m_muonNeutralFEReadHandleKey);
    if(!MuonContainerReadDecorHandle.isValid()){
      ATH_MSG_WARNING("Muon readhandle is a dud");    
    }
    else{
      for (auto Muon: *MuonContainerReadDecorHandle) m_LeptonLinkerPlots_NFE->fill(*Muon,*eventInfoReadHandle);
    } 
  }

  if(!m_useNeutralFE){
    SG::ReadDecorHandle<xAOD::ElectronContainer,std::vector<ElementLink<xAOD::FlowElementContainer> > > ElectronContainerReadDecorHandle(m_electronChargedFEReadHandleKey);
    if(!ElectronContainerReadDecorHandle.isValid()){
      ATH_MSG_WARNING("Electron readhandle is a dud");    
    }
    else{
      for (auto Electron: *ElectronContainerReadDecorHandle) m_LeptonLinkerPlots_CFE->fill(*Electron,*eventInfoReadHandle);
    } 
  }
  else{
    SG::ReadDecorHandle<xAOD::ElectronContainer, std::vector<ElementLink<xAOD::FlowElementContainer> > > ElectronContainerReadDecorHandle(m_electronNeutralFEReadHandleKey);
    if(!ElectronContainerReadDecorHandle.isValid()){
      ATH_MSG_WARNING("Electron readhandle is a dud");    
    }
    else{
      for (auto Electron: *ElectronContainerReadDecorHandle) m_LeptonLinkerPlots_NFE->fill(*Electron,*eventInfoReadHandle);
    } 
  }

  if(!m_useNeutralFE){
    SG::ReadDecorHandle<xAOD::PhotonContainer,std::vector<ElementLink<xAOD::FlowElementContainer> > > PhotonContainerReadDecorHandle(m_photonChargedFEReadHandleKey);
    if(!PhotonContainerReadDecorHandle.isValid()){
      ATH_MSG_WARNING("Photon readhandle is a dud");    
    }
    else{
      for (auto Photon: *PhotonContainerReadDecorHandle) m_LeptonLinkerPlots_CFE->fill(*Photon,*eventInfoReadHandle);
    } 
  }
  else{
    SG::ReadDecorHandle<xAOD::PhotonContainer, std::vector<ElementLink<xAOD::FlowElementContainer> > > PhotonContainerReadDecorHandle(m_photonNeutralFEReadHandleKey);
    if(!PhotonContainerReadDecorHandle.isValid()){
      ATH_MSG_WARNING("Photon readhandle is a dud");    
    }
    else{
      for (auto Photon: *PhotonContainerReadDecorHandle) m_LeptonLinkerPlots_NFE->fill(*Photon,*eventInfoReadHandle);
    } 
  }

  if(!m_useNeutralFE){
    SG::ReadDecorHandle<xAOD::TauJetContainer,std::vector<ElementLink<xAOD::FlowElementContainer> > > TauJetContainerReadDecorHandle(m_tauJetChargedFEReadHandleKey);
    if(!TauJetContainerReadDecorHandle.isValid()){
      ATH_MSG_WARNING("TauJet readhandle is a dud");    
    }
    else{
      for (auto TauJet: *TauJetContainerReadDecorHandle) m_LeptonLinkerPlots_CFE->fill(*TauJet,*eventInfoReadHandle);
    } 
  }
  else{
    SG::ReadDecorHandle<xAOD::TauJetContainer, std::vector<ElementLink<xAOD::FlowElementContainer> > > TauJetContainerReadDecorHandle(m_tauJetNeutralFEReadHandleKey);
    if(!TauJetContainerReadDecorHandle.isValid()){
      ATH_MSG_WARNING("TauJet readhandle is a dud");    
    }
    else{
      for (auto TauJet: *TauJetContainerReadDecorHandle) m_LeptonLinkerPlots_NFE->fill(*TauJet,*eventInfoReadHandle);
    } 
  }  
  
  return StatusCode::SUCCESS;

}

StatusCode PhysValFE::procHistograms(){
   return StatusCode::SUCCESS;
}


