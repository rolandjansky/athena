#include "DerivationFrameworkTop/ExKtbbAugmentation.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "FlavorTagDiscriminants/BTagTrackAugmenter.h"


namespace DerivationFramework {

  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_pt("SoftBVrtClusterTool_MSVTight_Vertices_pt");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_eta("SoftBVrtClusterTool_MSVTight_Vertices_eta");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_phi("SoftBVrtClusterTool_MSVTight_Vertices_phi");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_mass("SoftBVrtClusterTool_MSVTight_Vertices_mass");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_vtxx("SoftBVrtClusterTool_MSVTight_Vertices_vtxx");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_vtxy("SoftBVrtClusterTool_MSVTight_Vertices_vtxy");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_vtxz("SoftBVrtClusterTool_MSVTight_Vertices_vtxz");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_lxy("SoftBVrtClusterTool_MSVTight_Vertices_lxy");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_lxysig("SoftBVrtClusterTool_MSVTight_Vertices_lxysig");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_lz("SoftBVrtClusterTool_MSVTight_Vertices_lz");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_lzsig("SoftBVrtClusterTool_MSVTight_Vertices_lzsig");
  SG::AuxElement::Decorator<std::vector<float> > ExKtbbAugmentation::ExKtbbDecorators::secvtx_3dsig("SoftBVrtClusterTool_MSVTight_Vertices_3dsig");
  SG::AuxElement::Decorator<float> ExKtbbAugmentation::ExKtbbDecorators::jet_maxsd0("ExKtbb_maxsd0");
  SG::AuxElement::Decorator<float> ExKtbbAugmentation::ExKtbbDecorators::jet_av3sd0("ExKtbb_av3sd0");
  SG::AuxElement::Decorator<int> ExKtbbAugmentation::ExKtbbDecorators::smalljet_largeJetLabel("LargeJetLabel");

ExKtbbAugmentation::ExKtbbAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p)
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("secvtxName",m_secvtxName="SoftBVrtClusterTool_MSVTight_Vertices");
    declareProperty("LargeJetCollectionName",m_largeJetCollectionName="AntiKt8EMPFlowJets");
    declareProperty("SmallJetCollectionName",m_smallJetCollectionName="AntiKt4EMPFlowJets");
    declareProperty("ExktJetCollectionName",m_exktJetCollectionName="ExKt2GASubJets");
    declareProperty("PrimaryVerticesCollectionName",m_primaryVerticesCollectionName="PrimaryVertices");

    declareProperty("isMC",m_isMC=false);
}



ExKtbbAugmentation::~ExKtbbAugmentation(){}



StatusCode ExKtbbAugmentation::initialize(){

  ATH_MSG_INFO("Initializing ExKtbbAugmentation tool... " );

  return StatusCode::SUCCESS;

}



StatusCode ExKtbbAugmentation::finalize(){

  return StatusCode::SUCCESS;

}


StatusCode ExKtbbAugmentation::addBranches() const{

  const xAOD::EventInfo* eventInfo;

  if (evtStore()->retrieve(eventInfo,m_eventInfoName).isFailure()) {
    ATH_MSG_ERROR("could not retrieve event info " <<m_eventInfoName);
    return StatusCode::FAILURE;
  }

  static SG::AuxElement::ConstAccessor<float> tc_lvt_px("tc_lvt_px");
  static SG::AuxElement::ConstAccessor<float> tc_lvt_py("tc_lvt_py");
  static SG::AuxElement::ConstAccessor<float> tc_lvt_pz("tc_lvt_pz");
  static SG::AuxElement::ConstAccessor<float> tc_lvt_ee("tc_lvt_ee");

  BTagTrackAugmenter  track_augmenter;
  static SG::AuxElement::ConstAccessor<float> d0("btagIp_d0");
  static SG::AuxElement::ConstAccessor<float> d0s("btagIp_d0Uncertainty");

  const xAOD::VertexContainer *mprimvtx;
  if (evtStore()->retrieve(mprimvtx,m_primaryVerticesCollectionName).isFailure()) {
    ATH_MSG_ERROR("could not retrieve PV " <<m_eventInfoName);
    return StatusCode::FAILURE;
  }

  const xAOD::Vertex* primVtx = nullptr;
  for( const auto vtx : *mprimvtx ) {
    if (vtx->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
    primVtx = vtx;
  }
  
  const xAOD::VertexContainer *msecvtx;
  if (evtStore()->retrieve(msecvtx,m_secvtxName).isFailure()) {
    ATH_MSG_ERROR("could not retrieve MSV " <<m_eventInfoName);
    return StatusCode::FAILURE;
  }

  std::vector<const xAOD::Vertex*> MSVs;

  const xAOD::JetContainer* jets = evtStore()->retrieve< const xAOD::JetContainer >( m_largeJetCollectionName );
  const xAOD::JetContainer* smalljets = evtStore()->retrieve< const xAOD::JetContainer >( m_smallJetCollectionName );

  for (const auto smalljet: *smalljets) {
    tj_decorators.smalljet_largeJetLabel(*smalljet) = -1;
  }

  for (const auto jet : *jets) {

    auto constVector = jet->constituentLinks();
    for (auto constituent : constVector)
      tj_decorators.smalljet_largeJetLabel(*smalljets->at(constituent.index())) = jet->index();

    std::vector<const xAOD::Jet*> ExKtSubjets;
    auto trackJets = jet->auxdata<std::vector<ElementLink<xAOD::IParticleContainer> > >(m_exktJetCollectionName);
    for(auto tjet: trackJets) {
      if(!tjet.toTransient()) {
        ATH_MSG_WARNING("Unable to reset element link. You may crash soon ...");
      }

      if(!tjet.isValid()) {
        ATH_MSG_ERROR("Invalid link to subjet through link! I will not continue.");
	return StatusCode::FAILURE;
      } else {
        const xAOD::Jet* subjet = dynamic_cast<const xAOD::Jet*>(*tjet);
        if(subjet == 0){
          ATH_MSG_ERROR("Empty pointer to track subjet! You will crash soon...");
	  return StatusCode::FAILURE;
        } else {
	  auto constVector = subjet->getConstituents();
	  std::vector<float> sd0;
	  for (const auto constituent : constVector) {
	    if(constituent->rawConstituent() == 0) {
	      ATH_MSG_WARNING("Empty pointer to track constituent.");
	    } else {
	      const xAOD::TrackParticle* track = dynamic_cast<const xAOD::TrackParticle*>(constituent->rawConstituent());
	      if (track != 0) {
		track_augmenter.augment_with_ip(*track, *subjet);
		sd0.push_back(d0(*track)/d0s(*track));
	      }
	    }
	  }
	  std::sort(sd0.begin(), sd0.end(),
		    [](float sd01, float sd02) {
		      return (std::abs(sd01) > std::abs(sd02));
		    }
		    );
	  if (sd0.size() > 2) {
	    tj_decorators.jet_maxsd0(*subjet) = sd0[0];
	    tj_decorators.jet_av3sd0(*subjet) = (sd0[0]+sd0[1]+sd0[2])/3.;
	  } else if (sd0.size() > 1) {
	    tj_decorators.jet_maxsd0(*subjet) = sd0[0];
	    tj_decorators.jet_av3sd0(*subjet) = (sd0[0]+sd0[1])/2.;
	  } else if (sd0.size() > 0) {
	    tj_decorators.jet_maxsd0(*subjet) = sd0[0];
	    tj_decorators.jet_av3sd0(*subjet) = sd0[0];
	  } else {
	    tj_decorators.jet_maxsd0(*subjet) = -999.;
	    tj_decorators.jet_av3sd0(*subjet) = -999.;
	  }

          ExKtSubjets.push_back(subjet);
        }
      }
    }

    if (ExKtSubjets.size() == 0) continue;
        
    std::vector<std::vector<float> > vtx_pt(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_eta(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_phi(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_mass(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_vtxx(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_vtxy(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_vtxz(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_lxy(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_lxysig(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_lz(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_lzsig(ExKtSubjets.size());
    std::vector<std::vector<float> > vtx_3dsig(ExKtSubjets.size());
    std::vector<int> ExKtJet_index;

    // Loop over secondary vertices and calculate displacement with respect to track jet kt-closest to it
    // These quantities are stored in temporary vectors defined above.

    for(xAOD::Vertex *secVtx: *msecvtx) { 

      TLorentzVector SecVtxVector;
      SecVtxVector.SetPxPyPzE(tc_lvt_px(*secVtx), tc_lvt_py(*secVtx), tc_lvt_pz(*secVtx), tc_lvt_ee(*secVtx));

      if(SecVtxVector.DeltaR(jet->p4()) > 0.8) continue;

      auto trackJet = std::min_element(ExKtSubjets.begin(), ExKtSubjets.end(), 
				       [&SecVtxVector](const xAOD::Jet* j1, const xAOD::Jet* j2){
					 return (j1->pt()*SecVtxVector.DeltaR(j1->p4()) < j2->pt()*SecVtxVector.DeltaR(j2->p4()));
				       }
				       );
      
      float Lx = secVtx->position().x()-primVtx->position().x();
      float Ly = secVtx->position().y()-primVtx->position().y();
      float Lz = secVtx->position().z()-primVtx->position().z();

      Amg::Vector3D jetDirection((*trackJet)->p4().Px(),(*trackJet)->p4().Py(),(*trackJet)->p4().Pz());
      Amg::Vector3D dL(Lx,Ly,Lz);

      AmgSymMatrix(3) covariance = secVtx->covariancePosition() + primVtx->covariancePosition(); 
      AmgSymMatrix(3) invert_covariance = covariance.inverse();

      const float z0_sig = std::abs(Lz)/std::sqrt(covariance(2,2));
      const float Lxy = std::hypot(Lx,Ly);
      const float Lxy_sig = std::sqrt(Lx*Lx*invert_covariance(0,0) + 2*Lx*Ly*invert_covariance(0,1) + Ly*Ly*invert_covariance(1,1));
  
      const double decaylength = dL.norm();
      double decaylength_err = dL.dot(covariance*dL);
      decaylength_err /= dL.squaredNorm();
      double decaylength_significance = 0.;
      if (decaylength_err != 0.) decaylength_significance = decaylength/decaylength_err;
      double L_proj_jetDir = jetDirection.dot(dL);
      if (L_proj_jetDir < 0.) decaylength_significance *= -1.;

      // Assign calculated values to different track jets
      size_t ExKt_index = std::distance(ExKtSubjets.begin(), trackJet);
      vtx_pt[ExKt_index].push_back(SecVtxVector.Pt());
      vtx_eta[ExKt_index].push_back(SecVtxVector.Eta());
      vtx_phi[ExKt_index].push_back(SecVtxVector.Phi());
      vtx_mass[ExKt_index].push_back(SecVtxVector.M());
      vtx_vtxx[ExKt_index].push_back((*secVtx).x());
      vtx_vtxy[ExKt_index].push_back((*secVtx).y());
      vtx_vtxz[ExKt_index].push_back((*secVtx).z());
      vtx_lxy[ExKt_index].push_back(std::abs(Lxy));
      vtx_lxysig[ExKt_index].push_back(Lxy_sig);
      vtx_lz[ExKt_index].push_back(std::abs(Lz));
      vtx_lzsig[ExKt_index].push_back(z0_sig);
      vtx_3dsig[ExKt_index].push_back(decaylength_significance);
      
    }
    
    // Move the values to the decorator of the appropriate trackjet.
    for (size_t i=0; i<ExKtSubjets.size(); i++) {
      tj_decorators.secvtx_pt(*ExKtSubjets[i]) = std::move(vtx_pt[i]);
      tj_decorators.secvtx_eta(*ExKtSubjets[i]) = std::move(vtx_eta[i]);
      tj_decorators.secvtx_phi(*ExKtSubjets[i]) = std::move(vtx_phi[i]);
      tj_decorators.secvtx_mass(*ExKtSubjets[i]) = std::move(vtx_mass[i]);
      tj_decorators.secvtx_vtxx(*ExKtSubjets[i]) = std::move(vtx_vtxx[i]);
      tj_decorators.secvtx_vtxy(*ExKtSubjets[i]) = std::move(vtx_vtxy[i]);
      tj_decorators.secvtx_vtxz(*ExKtSubjets[i]) = std::move(vtx_vtxz[i]);
      tj_decorators.secvtx_lxy(*ExKtSubjets[i]) = std::move(vtx_lxy[i]);
      tj_decorators.secvtx_lxysig(*ExKtSubjets[i]) = std::move(vtx_lxysig[i]);
      tj_decorators.secvtx_lz(*ExKtSubjets[i]) = std::move(vtx_lz[i]);
      tj_decorators.secvtx_lzsig(*ExKtSubjets[i]) = std::move(vtx_lzsig[i]);
      tj_decorators.secvtx_3dsig(*ExKtSubjets[i]) = std::move(vtx_3dsig[i]);
    }
  } 
  return StatusCode::SUCCESS;

}



} /// namespace
