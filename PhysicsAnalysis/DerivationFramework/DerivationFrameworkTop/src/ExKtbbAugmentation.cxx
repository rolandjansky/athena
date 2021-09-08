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
#include "ParticleJetTools/ParticleJetLabelCommon.h"


namespace DerivationFramework {

ExKtbbAugmentation::ExKtbbAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p),
  secvtx_pt("SoftBVrtClusterTool_MSVTight_Vertices_pt"),
  secvtx_eta("SoftBVrtClusterTool_MSVTight_Vertices_eta"),
  secvtx_phi("SoftBVrtClusterTool_MSVTight_Vertices_phi"),
  secvtx_mass("SoftBVrtClusterTool_MSVTight_Vertices_mass"),
  secvtx_vtxx("SoftBVrtClusterTool_MSVTight_Vertices_vtxx"),
  secvtx_vtxy("SoftBVrtClusterTool_MSVTight_Vertices_vtxy"),
  secvtx_vtxz("SoftBVrtClusterTool_MSVTight_Vertices_vtxz"),
  secvtx_lxy("SoftBVrtClusterTool_MSVTight_Vertices_lxy"),
  secvtx_lxysig("SoftBVrtClusterTool_MSVTight_Vertices_lxysig"),
  secvtx_lz("SoftBVrtClusterTool_MSVTight_Vertices_lz"),
  secvtx_lzsig("SoftBVrtClusterTool_MSVTight_Vertices_lzsig"),
  secvtx_3dsig("SoftBVrtClusterTool_MSVTight_Vertices_3dsig"),
  jet_maxsd0("ExKtbb_maxsd0"),
  jet_av3sd0("ExKtbb_av3sd0"),
  jet_dexter_pbb("ExKtbb_dexter_pbb"),
  jet_dexter_pb("ExKtbb_dexter_pb"),
  jet_dexter_pl("ExKtbb_dexter_pl"),
  jet_dexter_pbb_trksflip("ExKtbb_dexter_pbb_trksflip"),
  jet_dexter_pb_trksflip("ExKtbb_dexter_pb_trksflip"),
  jet_dexter_pl_trksflip("ExKtbb_dexter_pl_trksflip"),
  jet_dexter_pbb_trksflip_svmassflip("ExKtbb_dexter_pbb_trksflip_svmassflip"),
  jet_dexter_pb_trksflip_svmassflip("ExKtbb_dexter_pb_trksflip_svmassflip"),
  jet_dexter_pl_trksflip_svmassflip("ExKtbb_dexter_pl_trksflip_svmassflip"),
  jet_dexter_pbb_negtrksflip("ExKtbb_dexter_pbb_negtrksflip"),
  jet_dexter_pb_negtrksflip("ExKtbb_dexter_pb_negtrksflip"),
  jet_dexter_pl_negtrksflip("ExKtbb_dexter_pl_negtrksflip"),
  jet_dexter_pbb_negtrksflip_svmassflip("ExKtbb_dexter_pbb_negtrksflip_svmassflip"),
  jet_dexter_pb_negtrksflip_svmassflip("ExKtbb_dexter_pb_negtrksflip_svmassflip"),
  jet_dexter_pl_negtrksflip_svmassflip("ExKtbb_dexter_pl_negtrksflip_svmassflip"),
  smalljet_largeJetLabel("LargeJetLabel"),
  jet_dexter_ghostBhadronCount("dexter_GhostBHadronsCount"),
  jet_dexter_ghostChadronCount("dexter_GhostCHadronsCount"),
  jet_dexter_TruthLabel("dexter_TruthLabel")
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
  ATH_CHECK(m_dexter.retrieve());

  return StatusCode::SUCCESS;

}



StatusCode ExKtbbAugmentation::finalize(){

  return StatusCode::SUCCESS;

}

int ExKtbbAugmentation::getDeXTerLabel(const int ghostBFinalCount,const int ghostCFinalCount) const
{

  int jet_flavor = 0;
  // jet flavor label
  // 55 - bb: nb >= 2               bb bbb bbc bbl...
  // 54 - bc: nb = 1 && nc >= 1      bc bcc bcl
  // 5  - b : nb = 1 && nc = 0      b bl bll
  // 44 - cc: nb = 0 && nc >= 2     cc ccc ccl...
  // 4  - c : nb = 0 && nc = 1      c cl cll
  // 0  - l : nb = 0 && nc = 0      light
  if (ghostBFinalCount >= 2)
    jet_flavor = 55;
  else if (ghostBFinalCount == 1 && ghostCFinalCount >= 1)
    jet_flavor = 54;
  else if (ghostBFinalCount == 1 && ghostCFinalCount == 0)
    jet_flavor = 5;
  else if (ghostBFinalCount == 0 && ghostCFinalCount >= 2)
    jet_flavor = 44;
  else if (ghostBFinalCount == 0 && ghostCFinalCount == 1)
    jet_flavor = 4;
  else if (ghostBFinalCount == 0 && ghostCFinalCount == 0)
    jet_flavor = 0;

  return jet_flavor;
}


StatusCode ExKtbbAugmentation::addBranches() const{

  const xAOD::EventInfo* eventInfo;

  if (evtStore()->retrieve(eventInfo,m_eventInfoName).isFailure()) {
    ATH_MSG_ERROR("could not retrieve event info " <<m_eventInfoName);
    return StatusCode::FAILURE;
  }

  const SG::AuxElement::ConstAccessor<float> tc_lvt_px("tc_lvt_px");
  const SG::AuxElement::ConstAccessor<float> tc_lvt_py("tc_lvt_py");
  const SG::AuxElement::ConstAccessor<float> tc_lvt_pz("tc_lvt_pz");
  const SG::AuxElement::ConstAccessor<float> tc_lvt_ee("tc_lvt_ee");

  BTagTrackAugmenter  track_augmenter;

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
    smalljet_largeJetLabel(*smalljet) = -1;
  }

  for (const auto jet : *jets) {

    std::vector<const xAOD::TruthParticle *> jetlabelpartsb;
    std::vector<const xAOD::TruthParticle *> jetlabelpartsc;    

    auto constVector = jet->constituentLinks();
    for (auto constituent : constVector)
      smalljet_largeJetLabel(*smalljets->at(constituent.index())) = jet->index();

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
          // Add Truth hadron labeling to jets
          if(m_isMC){
            const auto& b_links = subjet->auxdata<std::vector<ElementLink<xAOD::IParticleContainer> > >("GhostBHadronsFinal");
            const auto& c_links = subjet->auxdata<std::vector<ElementLink<xAOD::IParticleContainer> > >("GhostCHadronsFinal");
            for (const auto &b_el : b_links)
            {
              const auto *bhadron = dynamic_cast<const xAOD::TruthParticle *>(*b_el);
              if (bhadron->p4().DeltaR(subjet->p4()) < 0.3)
                jetlabelpartsb.push_back(bhadron);
            }
            for (const auto &c_el : c_links)
            {
              const auto *chadron = dynamic_cast<const xAOD::TruthParticle *>(*c_el);
              if (chadron->p4().DeltaR(subjet->p4()) < 0.3)
                jetlabelpartsc.push_back(chadron);
            }
          }
          // For track sd0      
	  auto constVector = subjet->getConstituents();
	  std::vector<double> sd0;
	  for (const auto constituent : constVector) {
	    if(constituent->rawConstituent() == 0) {
	      ATH_MSG_WARNING("Empty pointer to track constituent.");
	    } else {
	      const xAOD::TrackParticle* track = dynamic_cast<const xAOD::TrackParticle*>(constituent->rawConstituent());
	      if (track != 0) {
          double ip3d_sd0 = track_augmenter.get_signed_ip(*track, *subjet).ip3d_signed_d0_significance;
          sd0.push_back(ip3d_sd0);

        }
	    }
	  }
	  std::sort(sd0.begin(), sd0.end(),
		    [](float sd01, float sd02) {
		      return (std::abs(sd01) > std::abs(sd02));
		    }
		    );
	  if (sd0.size() > 2) {
	    jet_maxsd0(*subjet) = sd0[0];
	    jet_av3sd0(*subjet) = (sd0[0]+sd0[1]+sd0[2])/3.;
	  } else if (sd0.size() > 1) {
	    jet_maxsd0(*subjet) = sd0[0];
	    jet_av3sd0(*subjet) = (sd0[0]+sd0[1])/2.;
	  } else if (sd0.size() > 0) {
	    jet_maxsd0(*subjet) = sd0[0];
	    jet_av3sd0(*subjet) = sd0[0];
	  } else {
	    jet_maxsd0(*subjet) = -999.;
	    jet_av3sd0(*subjet) = -999.;
	  }

          ExKtSubjets.push_back(subjet);
        }
      }
    }
    if(m_isMC){
      using ParticleJetTools::childrenRemoved;
      childrenRemoved(jetlabelpartsb, jetlabelpartsb);
      childrenRemoved(jetlabelpartsb, jetlabelpartsc);
      childrenRemoved(jetlabelpartsc, jetlabelpartsc);    
      int ghostBTotalCount = jetlabelpartsb.size();
      int ghostCTotalCount = jetlabelpartsc.size();

      jet_dexter_ghostBhadronCount(*jet) = ghostBTotalCount;
      jet_dexter_ghostChadronCount(*jet) = ghostCTotalCount;

      jet_dexter_TruthLabel(*jet) = getDeXTerLabel(ghostBTotalCount, ghostCTotalCount);
    }

    ATH_MSG_VERBOSE("Adding DexTer scores to AntiKt8 jets");

    m_dexter->setProperty("negativeTagMode", "");
    ATH_CHECK(m_dexter.retrieve());
    auto scores = m_dexter->getScores(*jet);
    jet_dexter_pbb(*jet) = scores.at("dexter_pbb");
    jet_dexter_pb(*jet) = scores.at("dexter_pb");
    jet_dexter_pl(*jet) = scores.at("dexter_pl");

    m_dexter->setProperty("negativeTagMode", "TrksFlip");
    ATH_CHECK(m_dexter.retrieve());
    auto scores_TrksFlip = m_dexter->getScores(*jet);
    jet_dexter_pbb_trksflip(*jet) = scores_TrksFlip.at("dexter_pbb");
    jet_dexter_pb_trksflip(*jet) = scores_TrksFlip.at("dexter_pb");
    jet_dexter_pl_trksflip(*jet) = scores_TrksFlip.at("dexter_pl");

    m_dexter->setProperty("negativeTagMode", "SVMassTrksFlip");
    ATH_CHECK(m_dexter.retrieve());
    auto scores_SVMassTrksFlip = m_dexter->getScores(*jet);
    jet_dexter_pbb_trksflip_svmassflip(*jet) = scores_SVMassTrksFlip.at("dexter_pbb");
    jet_dexter_pb_trksflip_svmassflip(*jet) = scores_SVMassTrksFlip.at("dexter_pb");
    jet_dexter_pl_trksflip_svmassflip(*jet) = scores_SVMassTrksFlip.at("dexter_pl");

    m_dexter->setProperty("negativeTagMode", "NegTrksFlip");
    ATH_CHECK(m_dexter.retrieve());
    auto scores_NegTrksFlip = m_dexter->getScores(*jet);
    jet_dexter_pbb_negtrksflip(*jet) = scores_NegTrksFlip.at("dexter_pbb");
    jet_dexter_pb_negtrksflip(*jet) = scores_NegTrksFlip.at("dexter_pb");
    jet_dexter_pl_negtrksflip(*jet) = scores_NegTrksFlip.at("dexter_pl");

    m_dexter->setProperty("negativeTagMode", "SVMassNegTrksFlip");
    ATH_CHECK(m_dexter.retrieve());
    auto scores_SVMassNegTrksFlip = m_dexter->getScores(*jet);
    jet_dexter_pbb_negtrksflip_svmassflip(*jet) = scores_SVMassNegTrksFlip.at("dexter_pbb");
    jet_dexter_pb_negtrksflip_svmassflip(*jet) = scores_SVMassNegTrksFlip.at("dexter_pb");
    jet_dexter_pl_negtrksflip_svmassflip(*jet) = scores_SVMassNegTrksFlip.at("dexter_pl");

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
					 return (SecVtxVector.DeltaR(j1->p4()) < SecVtxVector.DeltaR(j2->p4()));
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
      secvtx_pt(*ExKtSubjets[i]) = std::move(vtx_pt[i]);
      secvtx_eta(*ExKtSubjets[i]) = std::move(vtx_eta[i]);
      secvtx_phi(*ExKtSubjets[i]) = std::move(vtx_phi[i]);
      secvtx_mass(*ExKtSubjets[i]) = std::move(vtx_mass[i]);
      secvtx_vtxx(*ExKtSubjets[i]) = std::move(vtx_vtxx[i]);
      secvtx_vtxy(*ExKtSubjets[i]) = std::move(vtx_vtxy[i]);
      secvtx_vtxz(*ExKtSubjets[i]) = std::move(vtx_vtxz[i]);
      secvtx_lxy(*ExKtSubjets[i]) = std::move(vtx_lxy[i]);
      secvtx_lxysig(*ExKtSubjets[i]) = std::move(vtx_lxysig[i]);
      secvtx_lz(*ExKtSubjets[i]) = std::move(vtx_lz[i]);
      secvtx_lzsig(*ExKtSubjets[i]) = std::move(vtx_lzsig[i]);
      secvtx_3dsig(*ExKtSubjets[i]) = std::move(vtx_3dsig[i]);
    }
  } 
  return StatusCode::SUCCESS;

}



} /// namespace
