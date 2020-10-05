///////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////

#include "JetMomentTools/JetQGTaggerVariableTool.h"
#include "PathResolver/PathResolver.h"
#include "AsgDataHandles/ReadDecorHandle.h"
#include "AsgDataHandles/WriteDecorHandle.h"


using std::string;
using xAOD::JetFourMom_t;

//**********************************************************************

JetQGTaggerVariableTool::JetQGTaggerVariableTool(const std::string& name)
: asg::AsgTool(name){
}

//**********************************************************************

StatusCode JetQGTaggerVariableTool::initialize() {
  ATH_MSG_INFO("Initializing JetQGTaggerVariableTool " << name());

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetQGTaggerVariableTool needs to have its input jet container configured!");
    return StatusCode::FAILURE;
  }

  m_nTrkKey = m_jetContainerName + "." + m_nTrkKey.key();
  m_trkWidthKey = m_jetContainerName + "." + m_trkWidthKey.key();
  m_trkC1Key = m_jetContainerName + "." + m_trkC1Key.key();
  m_nChargedKey = m_jetContainerName + "." + m_nChargedKey.key();
  m_truthPtKey = m_jetContainerName + "." + m_truthPtKey.key();
  m_truthEtaKey = m_jetContainerName + "." + m_truthEtaKey.key();

  ATH_CHECK(m_vertexContainer_key.initialize());
  ATH_CHECK(m_eventInfo_key.initialize());

  ATH_CHECK(m_nTrkKey.initialize());
  ATH_CHECK(m_trkWidthKey.initialize());
  ATH_CHECK(m_trkC1Key.initialize());
  ATH_CHECK(m_nChargedKey.initialize());
  ATH_CHECK(m_truthPtKey.initialize());
  ATH_CHECK(m_truthEtaKey.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetQGTaggerVariableTool::decorate(const xAOD::JetContainer& jetCont) const {


  SG::WriteDecorHandle<xAOD::JetContainer, int> nTrkHandle(m_nTrkKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> trkWidthHandle(m_trkWidthKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> trkC1Handle(m_trkC1Key);
  SG::WriteDecorHandle<xAOD::JetContainer, int> nChargedHandle(m_nChargedKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> truthPtHandle(m_truthPtKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> truthEtaHandle(m_truthEtaKey);

  // Get input vertex collection
  auto vertexContainer = SG::makeHandle (m_vertexContainer_key);
  if (!vertexContainer.isValid()){
    ATH_MSG_ERROR("Invalid VertexContainer datahandle: " << m_vertexContainer_key.key());
    return StatusCode::FAILURE;
  }

  auto vertices = vertexContainer.cptr();

  ATH_MSG_DEBUG("Successfully retrieved VertexContainer: " << m_vertexContainer_key.key());

  if (vertices->size() == 0 ) {
    ATH_MSG_WARNING("There are no vertices in the container. Exiting");

    for(const xAOD::Jet* jet : jetCont){
      nTrkHandle(*jet) = -1;
      trkWidthHandle(*jet) = -1.;
      trkC1Handle(*jet) = -1.;
      nChargedHandle(*jet) = -1;
      truthPtHandle(*jet) = -1.;
      truthEtaHandle(*jet) = -1.;
    }

    return StatusCode::SUCCESS;
  }

  //Find the HS vertex
  const xAOD::Vertex* HSvertex = findHSVertex(vertices);

  //Loop over the jets
  for(const xAOD::Jet* jet : jetCont){

    int nTracksCount = 0;
    float TracksWidth = 0., SumTracks_pTs = 0., TracksC1 = 0., beta = 0.2;
  
    //Get ghost-associated tracks
    std::vector<const xAOD::IParticle*> jettracks;
    jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack,jettracks);

    //Loop over the tracks
    std::vector<bool> IsGoodTrack;
    TLorentzVector tracki_TLV, trackj_TLV;
    TLorentzVector jet_TLV = jet -> p4();
    for (size_t i = 0; i < jettracks.size(); i++) {

      if(!jettracks[i]){
	IsGoodTrack.push_back(false);
	continue;
      }

      const xAOD::TrackParticle* trk = static_cast<const xAOD::TrackParticle*>(jettracks[i]);

      // only count tracks with selections                                                                                                                                                              
      // 1) pt>500 MeV                                                                                                                                                                                  
      // 2) accepted track from InDetTrackSelectionTool with CutLevel==Loose                                                                                                                            
      // 3) associated to primary vertex OR within 3mm of the primary vertex                                                                                                                            

      bool accept = (trk->pt()>500 &&
      		     m_trkSelectionTool->accept(*trk) &&
		     (m_trkVertexAssocTool->isCompatible(*trk,*HSvertex) || (!m_trkVertexAssocTool->isCompatible(*trk,*HSvertex) && fabs((trk->z0()+trk->vz()-HSvertex->z())*sin(trk->theta()))<3.))
		     );

      IsGoodTrack.push_back(accept);
      if (!accept) continue;

      nTracksCount++;
      
      tracki_TLV = trk -> p4();
      double DR_tracki_jet = tracki_TLV.DeltaR(jet_TLV);
      TracksWidth += trk -> pt() * DR_tracki_jet;
      SumTracks_pTs += trk -> pt();
      
    }// end loop over jettracks                                                                                                                                                                         
    
    if(SumTracks_pTs>0.) TracksWidth = TracksWidth / SumTracks_pTs;
    else TracksWidth = -1.;

    //Calculate C1 from tracks
    for(size_t i = 0; i < jettracks.size(); i++) {
      const xAOD::TrackParticle* trki = static_cast<const xAOD::TrackParticle*>(jettracks[i]);
      if( !( IsGoodTrack.at(i) ) ) continue;

      for(size_t j = i+1; j < jettracks.size(); j++) {
	const xAOD::TrackParticle* trkj = static_cast<const xAOD::TrackParticle*>(jettracks[j]);
	if( !( IsGoodTrack.at(j) ) ) continue;

	tracki_TLV = trki -> p4();
	trackj_TLV = trkj -> p4();
	double DR_tracki_trackj = tracki_TLV.DeltaR(trackj_TLV);
	TracksC1 += trki -> pt() * trkj -> pt() * pow( DR_tracki_trackj, beta) ;
	
      }//end loop over j 
    }//end double loop over ij                                                                                                                                                                          

    if(SumTracks_pTs>0.) TracksC1 = TracksC1 / ( pow(SumTracks_pTs, 2.) );
    else TracksC1 = -1.;

    // Add truth variables for QG tagging                                                                                                                                                               

    auto eventInfoContainer = SG::makeHandle (m_eventInfo_key);
    if (!eventInfoContainer.isValid()){
      ATH_MSG_ERROR("Invalid EventInfo datahandle: " << m_eventInfo_key.key());
      return StatusCode::FAILURE;
    }

    auto eventInfo = eventInfoContainer.cptr();

    bool isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
    int tntrk = 0;
    float truthjet_pt  = -999.0;
    float truthjet_eta = -999.0;

    if(isMC){
      const xAOD::Jet* tjet=nullptr;
      if(jet->isAvailable< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink") ){
	ATH_MSG_DEBUG("Accessing GhostTruthAssociationLink: is available");
	if(jet->auxdata< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink").isValid() ){
	  ATH_MSG_DEBUG("Accessing GhostTruthAssociationLink: is valid");
	  ElementLink<xAOD::JetContainer> truthlink = jet->auxdata< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink");
	  if(truthlink)
	    tjet = * truthlink;
	  else{
	    ATH_MSG_DEBUG("Skipping...truth link is broken");
	  }//endelse NULL pointer                                                                                                                                                                       
	}
	else {
	  ATH_MSG_DEBUG("Invalid truth link: setting weight to 1");
	} //endelse isValid                                                                                                                                                                             
      } //endif isAvailable                                                                                                                                                                             
      else {
	ATH_MSG_DEBUG("Cannot access truth Link: setting weight to 1");
      }//endelse isAvailable                                                                                                                                                                            
      
      if(tjet){
	ATH_MSG_DEBUG("Truth Jet: " << tjet->numConstituents());
	
	truthjet_pt  = tjet->pt();
	truthjet_eta = tjet->eta();
	
	//Calculate the number of charged final state particles with pT > 500 MeV
	for (size_t ind = 0; ind < tjet->numConstituents(); ind++) {
	  const xAOD::TruthParticle *part = static_cast<const xAOD::TruthParticle*>(tjet->rawConstituent(ind));
	  ATH_MSG_DEBUG("part: " << part );
	  // dont count invalid truth particles                                                                                                                                                         
	  if (!part) continue;
	  // require the particle in the final state                                                                                                                                                    
	  ATH_MSG_DEBUG("status: " << (part->status()) );
	  if( ! (part->status() == 1) ) continue;
	  // require that the particle type (e.g. production type) be valid (e.g. not primaries)                                                                                                        
	  ATH_MSG_DEBUG("barcode: " << (part->barcode()) );
	  if ((part->barcode())>2e5) continue;
	  // pt>500 MeV
	  ATH_MSG_DEBUG("pt: " << (part->pt()) );
	  if( ! (part->pt()>500.) )  continue;
	  // charged                                                                                                                                                                                    
	  ATH_MSG_DEBUG("isCharged: " << (part->isCharged()) );
	  if( !(part->isCharged()) ) continue;
	  tntrk++;
	}
      }
    }

    nTrkHandle(*jet) = nTracksCount;
    trkWidthHandle(*jet) = TracksWidth;
    trkC1Handle(*jet) = TracksC1;
    nChargedHandle(*jet) = tntrk;
    truthPtHandle(*jet) = truthjet_pt;
    truthEtaHandle(*jet) = truthjet_eta;

  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

const xAOD::Vertex* JetQGTaggerVariableTool::findHSVertex(const xAOD::VertexContainer*& vertices) const
{
  for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ) {
    if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx) {

      ATH_MSG_VERBOSE("JetQGTaggerVariableTool " << name() << " Found HS vertex at index: "<< iVertex);
      return vertices->at(iVertex);
    }
  }
  ATH_MSG_VERBOSE("There is no vertex of type PriVx. Taking default vertex.");
  return vertices->at(0);
}
