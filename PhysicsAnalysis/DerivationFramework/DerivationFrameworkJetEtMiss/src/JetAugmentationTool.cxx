/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Teng Jian Khoo (teng.jian.khoo@cern.ch)
//

#include "JetAugmentationTool.h"
#include "xAODCore/ShallowCopy.h"
#include "StoreGate/WriteDecorHandle.h"

namespace DerivationFramework {

  JetAugmentationTool::JetAugmentationTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_jetCalibTool(""),
    m_docalib(false),
    m_jvtTool(""),
    m_jetJvtEfficiencyTool(""),
    m_dojvt(false),
    m_dobtag(false),
    m_jetTrackSumMomentsTool(""),
    m_decoratetracksum(false),
    m_jetPtAssociationTool(""),
    m_decorateptassociation(false),
    m_trkSelectionTool(""),
    m_trkVtxAssociationTool(""),
    m_qgTool(""),
    m_decorateQGVariables(false),
    m_jetTruthLabelingTool(""),
    m_decoratetruthlabel(false)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("MomentPrefix",   m_momentPrefix = "DFCommonJets_");
    declareProperty("InputJets",      m_containerName = "AntiKt4EMTopoJets");
    // declareProperty("CalibScale",     m_calibScale = "JetGSCScaleMomentum");
    declareProperty("CalibMomentKey", m_calibMomentKey = "Calib");
    declareProperty("JetCalibTool",   m_jetCalibTool);
    declareProperty("JvtMomentKey",   m_jvtMomentKey = "Jvt");
    declareProperty("JetJvtTool",     m_jvtTool);
    declareProperty("JetJvtEffTool",  m_jetJvtEfficiencyTool);
    declareProperty("JetBtagTools",   m_btagSelTools);
    declareProperty("JetBtagWPs",     m_btagWP);
    declareProperty("JetTrackSumMomentsTool", m_jetTrackSumMomentsTool);
    declareProperty("JetPtAssociationTool", m_jetPtAssociationTool);
    declareProperty("TrackSelectionTool", m_trkSelectionTool);
    declareProperty("TrackVertexAssociationTool", m_trkVtxAssociationTool);
    declareProperty("JetTruthLabelingTool", m_jetTruthLabelingTool);
    declareProperty("JetQGTaggerTool", m_qgTool);
  }

  StatusCode JetAugmentationTool::initialize()
  {
    ATH_MSG_INFO("Initialising JetAugmentationTool");

    if(!m_jetCalibTool.empty()) {
      CHECK(m_jetCalibTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with calibration \"" << m_momentPrefix+m_calibMomentKey << "\"");

      m_docalib = true;

      m_calibpt_key = m_containerName + "." + m_momentPrefix + m_calibMomentKey + "_pt";
      m_calibeta_key = m_containerName + "." + m_momentPrefix + m_calibMomentKey + "_eta";
      m_calibphi_key = m_containerName + "." + m_momentPrefix + m_calibMomentKey + "_phi";
      m_calibm_key = m_containerName + "." + m_momentPrefix + m_calibMomentKey + "_m";

      ATH_CHECK(m_calibpt_key.initialize());
      ATH_CHECK(m_calibeta_key.initialize());
      ATH_CHECK(m_calibphi_key.initialize());
      ATH_CHECK(m_calibm_key.initialize());

      if(!m_jvtTool.empty()) {
	CHECK(m_jvtTool.retrieve());
	ATH_MSG_INFO("Augmenting jets with updated JVT \"" << m_momentPrefix+m_jvtMomentKey << "\"");
	m_dojvt = true;

	m_jvt_key = m_containerName + "." + m_momentPrefix + m_jvtMomentKey;
	m_passJvt_key = m_containerName + "." + m_momentPrefix + "pass" + m_jvtMomentKey;

	ATH_CHECK(m_jvt_key.initialize());
	ATH_CHECK(m_passJvt_key.initialize());

	if(!m_btagSelTools.empty()) {
	  size_t ibtag(0);
	  for(const auto& tool : m_btagSelTools) {
	    CHECK(tool.retrieve());
	    ATH_MSG_INFO("Augmenting jets with B-tag working point \"" << m_momentPrefix+m_btagWP[ibtag] << "\"");
	    m_dec_btag_keys.emplace_back( m_containerName + "." + m_momentPrefix+m_btagWP[ibtag] );
	    m_dobtag = true;
	    ++ibtag;
	  }

	  ATH_CHECK(m_dec_btag_keys.initialize());
	}
      }
    }

    if(!m_jetJvtEfficiencyTool.empty()) {
      CHECK(m_jetJvtEfficiencyTool.retrieve());
      ATH_MSG_INFO("Jvt efficiency tool initialized \"" << m_momentPrefix+"pass"+m_jvtMomentKey << "\"");
    }

    if(!m_jetTrackSumMomentsTool.empty()) {
      CHECK(m_jetTrackSumMomentsTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with track sum moments \"" << m_momentPrefix << "TrackSumMass,Pt\"");
      m_decoratetracksum = true;
      m_acc_tracksummass = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_momentPrefix+"TrackSumMass");
      m_acc_tracksumpt   = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_momentPrefix+"TrackSumPt");

      m_tracksummass_key = m_containerName + "." + m_momentPrefix+"TrackSumMass";
      m_tracksumpt_key = m_containerName + "." + m_momentPrefix+"TrackSumPt";

      ATH_CHECK(m_tracksummass_key.initialize());
      ATH_CHECK(m_tracksumpt_key.initialize());
    }

    // This tool creates the GhostTruthAssociation decorations recommended for truth matching //
    if(!m_jetPtAssociationTool.empty()){
      CHECK(m_jetPtAssociationTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with GhostTruthAssociation moments Link and Fraction");
      m_decorateptassociation = true;
      m_acc_GhostTruthAssociationFraction = std::make_unique< SG::AuxElement::ConstAccessor<float> >("GhostTruthAssociationFraction");
      m_acc_GhostTruthAssociationLink     = std::make_unique< SG::AuxElement::ConstAccessor< ElementLink<xAOD::JetContainer> > >("GhostTruthAssociationLink");
      
      m_GhostTruthAssociationFraction_key = m_containerName + ".GhostTruthAssociationFraction";
      m_GhostTruthAssociationLink_key = m_containerName + ".GhostTruthAssociationLink";

      ATH_CHECK(m_GhostTruthAssociationFraction_key.initialize());
      ATH_CHECK(m_GhostTruthAssociationLink_key.initialize());

    }

    if(!m_trkSelectionTool.empty()) {
      CHECK( m_trkSelectionTool.retrieve() );
      if(!m_trkVtxAssociationTool.empty()){
	CHECK( m_trkVtxAssociationTool.retrieve() );

	if(!m_qgTool.empty()){
	  CHECK(m_qgTool.retrieve());
	  m_decorateQGVariables = true;

	  m_acc_AssociatedNTracks     = std::make_unique< SG::AuxElement::ConstAccessor<int> >(m_momentPrefix + "QGTagger_NTracks");
	  m_acc_AssociatedTracksWidth = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_momentPrefix + "QGTagger_TracksWidth");
	  m_acc_AssociatedTracksC1    = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_momentPrefix + "QGTagger_TracksC1");
	  m_acc_Associated_truthjet_nCharged = std::make_unique< SG::AuxElement::ConstAccessor<int> >(m_momentPrefix + "QGTagger_truthjet_nCharged");
	  m_acc_Associated_truthjet_pt       = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_momentPrefix + "QGTagger_truthjet_pt");
	  m_acc_Associated_truthjet_eta      = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_momentPrefix + "QGTagger_truthjet_eta");

	  m_associatedNTracks_key = m_containerName + "." + m_momentPrefix + "QGTagger_NTracks";
	  m_associatedTracksWidth_key = m_containerName + "." + m_momentPrefix + "QGTagger_TracksWidth";
	  m_associatedTracksC1_key =m_containerName + "." + m_momentPrefix + "QGTagger_TracksC1";
	  m_associated_truthjet_nCharged_key =m_containerName + "." + m_momentPrefix + "QGTagger_truthjet_nCharged";
	  m_associated_truthjet_pt_key =m_containerName + "." + m_momentPrefix + "QGTagger_truthjet_pt";
	  m_associated_truthjet_eta_key =m_containerName + "." + m_momentPrefix + "QGTagger_truthjet_eta";
	  
	  ATH_CHECK(m_associatedNTracks_key.initialize());
	  ATH_CHECK(m_associatedTracksWidth_key.initialize());
	  ATH_CHECK(m_associatedTracksC1_key.initialize());
	  ATH_CHECK(m_associated_truthjet_nCharged_key.initialize());
	  ATH_CHECK(m_associated_truthjet_pt_key.initialize());
	  ATH_CHECK(m_associated_truthjet_eta_key.initialize());

	}
      }
    }
    
    if(!m_jetTruthLabelingTool.empty()) {
      CHECK(m_jetTruthLabelingTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with truthlabeling");
      m_decoratetruthlabel = true;
      m_truthLabelName = m_jetTruthLabelingTool->getLargeRJetTruthLabelName();

      m_acc_label = std::make_unique< SG::AuxElement::ConstAccessor<int> >(m_truthLabelName);
      m_acc_dRW = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_truthLabelName+"_dR_W");
      m_acc_dRZ = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_truthLabelName+"_dR_Z");
      m_acc_dRTop = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_truthLabelName+"_dR_Top");
      m_acc_dRH = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_truthLabelName+"_dR_H");
      m_acc_NB = std::make_unique< SG::AuxElement::ConstAccessor<int> >(m_truthLabelName+"_NB");
      m_acc_truthJetMass = std::make_unique< SG::AuxElement::ConstAccessor<float> >(m_truthLabelName+"_TruthJetMass");

      m_truthLabel_key = m_containerName + "." + m_truthLabelName;
      m_truthLabel_dRW_key = m_containerName + "." + m_truthLabelName+"_dR_W";
      m_truthLabel_dRZ_key = m_containerName + "." + m_truthLabelName+"_dR_Z";
      m_truthLabel_dRH_key = m_containerName + "." + m_truthLabelName+"_dR_H";
      m_truthLabel_dRTop_key = m_containerName + "." + m_truthLabelName+"_dR_Top";
      m_truthLabel_NB_key = m_containerName + "." + m_truthLabelName+"_NB";
      m_truthLabel_truthJetMass_key = m_containerName + "." + m_truthLabelName+"_TruthJetMass";

      ATH_CHECK(m_truthLabel_key.initialize());
      ATH_CHECK(m_truthLabel_dRW_key.initialize());
      ATH_CHECK(m_truthLabel_dRZ_key.initialize());
      ATH_CHECK(m_truthLabel_dRH_key.initialize());
      ATH_CHECK(m_truthLabel_dRTop_key.initialize());
      ATH_CHECK(m_truthLabel_NB_key.initialize());
      ATH_CHECK(m_truthLabel_truthJetMass_key.initialize());
    }

    return StatusCode::SUCCESS;
  }
    
  StatusCode JetAugmentationTool::finalize()
  {

    return StatusCode::SUCCESS;
  }

  StatusCode JetAugmentationTool::addBranches() const
  {
    // retrieve container
    const xAOD::JetContainer* jets(0);
    if( evtStore()->retrieve( jets, m_containerName ).isFailure() ) {
      ATH_MSG_WARNING ("Couldn't retrieve jets with key: " << m_containerName );
      return StatusCode::FAILURE;
    }

    // make a shallow copy of the jets
    std::pair<xAOD::JetContainer*,xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*jets);
    std::unique_ptr<xAOD::JetContainer> jets_copy(shallowcopy.first);
    std::unique_ptr<xAOD::ShallowAuxContainer> jets_copyaux(shallowcopy.second);

      // if we have a calibration tool, apply the calibration
    if(m_docalib) {
      if((m_jetCalibTool->modify(*jets_copy)).isFailure()) {
	ATH_MSG_WARNING("Problem applying jet calibration");
	return StatusCode::FAILURE;
      }
    }

    if(m_decoratetracksum){
      if((m_jetTrackSumMomentsTool->modify(*jets_copy)).isFailure())
	{
	  ATH_MSG_WARNING("Problems calculating TrackSumMass and TrackSumPt");
	  return StatusCode::FAILURE;
	}
    }

    // Check if GhostTruthAssociation decorations already exist for first jet, and if so skip them
    bool isMissingPtAssociation = true;
    if( !m_decorateptassociation || jets_copy->size() == 0 || m_acc_GhostTruthAssociationFraction->isAvailable(*jets_copy->at(0)) ) {
      isMissingPtAssociation = false;
    }

    if(m_decorateptassociation && isMissingPtAssociation){
      if((m_jetPtAssociationTool->modify(*jets_copy)).isFailure())
	{
	  ATH_MSG_ERROR("Problem running the JetPtAssociationTool");
	  return StatusCode::FAILURE;
	}
    }

    if(m_decoratetruthlabel){
      if((m_jetTruthLabelingTool->modify(*jets_copy)).isFailure() )
	{
          ATH_MSG_ERROR("Problems applying large-R jet truth labels");
          return StatusCode::FAILURE;
        }
    }

    if(m_decorateQGVariables){
      if((m_qgTool->decorate(*jets_copy)).isFailure() )
	{
	  ATH_MSG_ERROR("Problems applying the q/g tagging label");
	  return StatusCode::FAILURE;
	}
    }

    // loop over the copies
    for(const auto& jet : *jets_copy) {
      // get the original jet so we can decorate it
      const xAOD::Jet& jet_orig( *(*jets)[jet->index()] );

      if(m_docalib) {

	SG::WriteDecorHandle<xAOD::JetContainer, float> calibpt_handle(m_calibpt_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> calibeta_handle(m_calibeta_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> calibphi_handle(m_calibphi_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> calibm_handle(m_calibm_key);

	// generate static decorators to avoid multiple lookups	
	calibpt_handle(jet_orig)  = jet->pt();
	calibeta_handle(jet_orig) = jet->eta();
	calibphi_handle(jet_orig) = jet->phi();
	calibm_handle(jet_orig)   = jet->m();

	ATH_MSG_VERBOSE("Calibrated jet pt: " << jet->pt() );

	if(m_dojvt) {

	  SG::WriteDecorHandle<xAOD::JetContainer, float> jvt_handle(m_jvt_key);
	  SG::WriteDecorHandle<xAOD::JetContainer, char> passJvt_handle(m_passJvt_key);

	  float jvt_value = m_jvtTool->updateJvt(*jet);
	  jvt_handle(jet_orig)= jvt_value;
	  ATH_MSG_VERBOSE("Calibrated JVT: " << jvt_value);
	  
	  bool passJVT = m_jetJvtEfficiencyTool->passesJvtCut(jet_orig);
	  passJvt_handle(jet_orig) = passJVT;
	  
	  if(m_dobtag) {
	    size_t ibtag(0);
	    for(const auto& tool : m_btagSelTools) {
	      SG::WriteDecorHandle<xAOD::JetContainer, char> dec_btag_handle(m_dec_btag_keys.at(ibtag));
	      bool passWP = std::abs(jet->eta()) < 2.7 && passJVT && tool->accept(*jet);
	      dec_btag_handle(jet_orig) = passWP;
	      ATH_MSG_VERBOSE("Btag working point \"" << m_btagWP[ibtag] << "\" " << (passWP ? "passed." : "failed."));
	      ++ibtag;
	    }
	  }
	}
      }

      if(m_decoratetracksum) {  
	SG::WriteDecorHandle<xAOD::JetContainer, float> tracksummass_handle(m_tracksummass_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> tracksumpt_handle(m_tracksumpt_key);

	tracksummass_handle(jet_orig) = (*m_acc_tracksummass)(*jet);
	tracksumpt_handle(jet_orig)   = (*m_acc_tracksumpt)(*jet);

	ATH_MSG_VERBOSE("TrackSumMass: " << (*m_acc_tracksummass)(jet_orig) );
	ATH_MSG_VERBOSE("TrackSumPt: "   << (*m_acc_tracksummass)(jet_orig) );
      }

      if(m_decorateptassociation && isMissingPtAssociation){

	SG::WriteDecorHandle<xAOD::JetContainer, float> ghostTruthAssocFrac_handle(m_GhostTruthAssociationFraction_key);
	SG::WriteDecorHandle<xAOD::JetContainer, ElementLink<xAOD::JetContainer> > ghostTruthAssocLink_handle(m_GhostTruthAssociationLink_key);

        if(m_acc_GhostTruthAssociationFraction->isAvailable(*jet)){
	  ghostTruthAssocFrac_handle(jet_orig) = (*m_acc_GhostTruthAssociationFraction)(*jet);
	  ATH_MSG_INFO("GhostTruthAssociationFraction: " << (*m_acc_GhostTruthAssociationFraction)(jet_orig) );
	}
	if(m_acc_GhostTruthAssociationLink->isAvailable(*jet)){
	  ghostTruthAssocLink_handle(jet_orig) = (*m_acc_GhostTruthAssociationLink)(*jet);
	  ATH_MSG_INFO("GhostTruthAssociationLink: " << (*m_acc_GhostTruthAssociationLink)(jet_orig) );
	}
      }
      
      if(m_decoratetruthlabel){

	SG::WriteDecorHandle<xAOD::JetContainer, float> truthLabel_dRW_handle(m_truthLabel_dRW_key);
	SG::WriteDecorHandle<xAOD::JetContainer, int> truthLabel_handle(m_truthLabel_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> truthLabel_dRZ_handle(m_truthLabel_dRZ_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> truthLabel_dRH_handle(m_truthLabel_dRH_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> truthLabel_dRTop_handle(m_truthLabel_dRTop_key);
	SG::WriteDecorHandle<xAOD::JetContainer, int> truthLabel_NB_handle(m_truthLabel_NB_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> truthLabel_truthJetMass_handle(m_truthLabel_truthJetMass_key);
	
	if(m_acc_label->isAvailable(*jet)) truthLabel_handle(jet_orig) = (*m_acc_label)(*jet);
	if(m_acc_dRW->isAvailable(*jet)) truthLabel_dRW_handle(jet_orig) = (*m_acc_dRW)(*jet);
	if(m_acc_dRZ->isAvailable(*jet)) truthLabel_dRZ_handle(jet_orig) = (*m_acc_dRZ)(*jet);
	if(m_acc_dRH->isAvailable(*jet)) truthLabel_dRH_handle(jet_orig) = (*m_acc_dRH)(*jet);
	if(m_acc_dRTop->isAvailable(*jet)) truthLabel_dRTop_handle(jet_orig) = (*m_acc_dRTop)(*jet);
	if(m_acc_NB->isAvailable(*jet)) truthLabel_NB_handle(jet_orig) = (*m_acc_NB)(*jet);
	if(m_acc_truthJetMass->isAvailable(*jet)) truthLabel_truthJetMass_handle(jet_orig) = (*m_acc_truthJetMass)(*jet);
	  
      }

      if(m_decorateQGVariables){

	SG::WriteDecorHandle<xAOD::JetContainer, int> associatedNTracks_handle(m_associatedNTracks_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> associatedTracksWidth_handle(m_associatedTracksWidth_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> associatedTracksC1_handle(m_associatedTracksC1_key);
	SG::WriteDecorHandle<xAOD::JetContainer, int> associated_truthjet_nCharged_handle(m_associated_truthjet_nCharged_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> associated_truthjet_pt_handle(m_associated_truthjet_pt_key);
	SG::WriteDecorHandle<xAOD::JetContainer, float> associated_truthjet_eta_handle(m_associated_truthjet_eta_key);

	if(m_acc_AssociatedNTracks->isAvailable(*jet)) associatedNTracks_handle(jet_orig) = (*m_acc_AssociatedNTracks)(*jet);
	if(m_acc_AssociatedTracksWidth->isAvailable(*jet)) associatedTracksWidth_handle(jet_orig) = (*m_acc_AssociatedTracksWidth)(*jet);
	if(m_acc_AssociatedTracksC1->isAvailable(*jet)) associatedTracksC1_handle(jet_orig) = (*m_acc_AssociatedTracksC1)(*jet);
	if(m_acc_Associated_truthjet_nCharged->isAvailable(*jet)) associated_truthjet_nCharged_handle(jet_orig) = (*m_acc_Associated_truthjet_nCharged)(*jet);
	if(m_acc_Associated_truthjet_pt->isAvailable(*jet)) associated_truthjet_pt_handle(jet_orig) = (*m_acc_Associated_truthjet_pt)(*jet);
	if(m_acc_Associated_truthjet_eta->isAvailable(*jet)) associated_truthjet_eta_handle(jet_orig) = (*m_acc_Associated_truthjet_eta)(*jet);
      }
      
    }

    return StatusCode::SUCCESS;
  }
}
