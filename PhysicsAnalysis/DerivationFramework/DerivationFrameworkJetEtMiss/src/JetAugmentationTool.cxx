/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JetAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Teng Jian Khoo (teng.jian.khoo@cern.ch)
//

#include "JetAugmentationTool.h"
#include "xAODCore/ShallowCopy.h"

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

      m_dec_calibpt  = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_calibMomentKey+"_pt");
      m_dec_calibeta = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_calibMomentKey+"_eta");
      m_dec_calibphi = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_calibMomentKey+"_phi");
      m_dec_calibm   = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_calibMomentKey+"_m");

      if(!m_jvtTool.empty()) {
	CHECK(m_jvtTool.retrieve());
	ATH_MSG_INFO("Augmenting jets with updated JVT \"" << m_momentPrefix+m_jvtMomentKey << "\"");
	m_dojvt = true;

	m_dec_jvt  = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_jvtMomentKey);
        m_dec_passJvt  = std::make_unique< SG::AuxElement::Decorator<char> >(m_momentPrefix+"pass"+m_jvtMomentKey);

	if(!m_btagSelTools.empty()) {
	  size_t ibtag(0);
	  for(const auto& tool : m_btagSelTools) {
	    CHECK(tool.retrieve());
	    ATH_MSG_INFO("Augmenting jets with B-tag working point \"" << m_momentPrefix+m_btagWP[ibtag] << "\"");
	    m_dec_btag.push_back(new SG::AuxElement::Decorator<float>(m_momentPrefix+m_btagWP[ibtag]));
	    m_dobtag = true;
	    ++ibtag;
	  }
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
      m_dec_tracksummass = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+"TrackSumMass");
      m_dec_tracksumpt   = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+"TrackSumPt");
    }

    // This tool creates the GhostTruthAssociation decorations recommended for truth matching //
    if(!m_jetPtAssociationTool.empty()){
      CHECK(m_jetPtAssociationTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with GhostTruthAssociation moments Link and Fraction");
      m_decorateptassociation = true;
      m_dec_GhostTruthAssociationFraction = std::make_unique< SG::AuxElement::Decorator<float> >("GhostTruthAssociationFraction");
      m_dec_GhostTruthAssociationLink     = std::make_unique< SG::AuxElement::Decorator< ElementLink<xAOD::JetContainer> > >("GhostTruthAssociationLink");
    }

    if(!m_trkSelectionTool.empty()) {
      CHECK( m_trkSelectionTool.retrieve() );
      if(!m_trkVtxAssociationTool.empty()){
	CHECK( m_trkVtxAssociationTool.retrieve() );

	if(!m_qgTool.empty()){
	  CHECK(m_qgTool.retrieve());
	  m_decorateQGVariables = true;
	  m_dec_AssociatedNTracks     = std::make_unique< SG::AuxElement::Decorator<int> >(m_momentPrefix + "QGTagger_NTracks");
	  m_dec_AssociatedTracksWidth = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix + "QGTagger_TracksWidth");
	  m_dec_AssociatedTracksC1    = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix + "QGTagger_TracksC1");
	  m_dec_Associated_truthjet_nCharged = std::make_unique< SG::AuxElement::Decorator<int> >(m_momentPrefix + "QGTagger_truthjet_nCharged");
	  m_dec_Associated_truthjet_pt       = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix + "QGTagger_truthjet_pt");
	  m_dec_Associated_truthjet_eta      = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix + "QGTagger_truthjet_eta");
	}
      }
    }
    
    if(!m_jetTruthLabelingTool.empty()) {
      CHECK(m_jetTruthLabelingTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with truthlabeling");
      m_decoratetruthlabel = true;
      m_truthLabelName = m_jetTruthLabelingTool->getLargeRJetTruthLabelName();
      m_dec_Label = std::make_unique< SG::AuxElement::Decorator<int> >(m_truthLabelName);
      m_dec_dRW = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_W");
      m_dec_dRZ = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_Z");
      m_dec_dRTop = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_Top");
      m_dec_dRH = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_H");
      m_dec_NB = std::make_unique< SG::AuxElement::Decorator<int> >(m_truthLabelName+"_NB");
      m_dec_TruthJetMass = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_TruthJetMass");
    }

    return StatusCode::SUCCESS;
  }
    
  StatusCode JetAugmentationTool::finalize()
  {

    if(m_dobtag) {
      for(const auto& pdec : m_dec_btag) delete pdec;
    }

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
    if( !m_decorateptassociation || jets_copy->size() == 0 || m_dec_GhostTruthAssociationFraction->isAvailable(*jets_copy->at(0)) ) {
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

	// generate static decorators to avoid multiple lookups	
	(*m_dec_calibpt)(jet_orig)  = jet->pt();
	(*m_dec_calibeta)(jet_orig) = jet->eta();
	(*m_dec_calibphi)(jet_orig) = jet->phi();
	(*m_dec_calibm)(jet_orig)   = jet->m();

	ATH_MSG_VERBOSE("Calibrated jet pt: " << (*m_dec_calibpt)(jet_orig) );

	if(m_dojvt) {

	  (*m_dec_jvt)(jet_orig) = m_jvtTool->updateJvt(*jet);
	  ATH_MSG_VERBOSE("Calibrated JVT: " << (*m_dec_jvt)(jet_orig) );
	  
	  bool passJVT_tool = m_jetJvtEfficiencyTool->passesJvtCut(jet_orig);
	  (*m_dec_passJvt)(jet_orig) = passJVT_tool;

	  if(m_dobtag) {
	    bool passJVT = jet->pt()>50e3 || fabs(jet->eta())>2.4 || (*m_dec_jvt)(jet_orig)>0.64;
	    size_t ibtag(0);
	    for(const auto& tool : m_btagSelTools) {
	      (*m_dec_btag[ibtag])(jet_orig) = jet->pt()>20e3 && fabs(jet->eta())<2.5 && passJVT && tool->accept(*jet);
	      ATH_MSG_VERBOSE("Btag working point \"" << m_btagWP[ibtag] << "\" " << ((*m_dec_btag[ibtag])(jet_orig) ? "passed." : "failed."));
	      ++ibtag;
	    }
	  }
	}
      }

      if(m_decoratetracksum) {  
	(*m_dec_tracksummass)(jet_orig) = jet->getAttribute<float>("TrackSumMass");
	(*m_dec_tracksumpt)(jet_orig)   = jet->getAttribute<float>("TrackSumPt");
	ATH_MSG_VERBOSE("TrackSumMass: " << (*m_dec_tracksummass)(jet_orig) );
	ATH_MSG_VERBOSE("TrackSumPt: "   << (*m_dec_tracksummass)(jet_orig) );
      }

      if(m_decorateptassociation && isMissingPtAssociation){
        if(m_dec_GhostTruthAssociationFraction->isAvailable(*jet)){
	  (*m_dec_GhostTruthAssociationFraction)(jet_orig) = jet->getAttribute<float>("GhostTruthAssociationFraction");
	  ATH_MSG_VERBOSE("GhostTruthAssociationFraction: " << (*m_dec_GhostTruthAssociationFraction)(jet_orig) );
	}
	if(m_dec_GhostTruthAssociationLink->isAvailable(*jet)){
	  (*m_dec_GhostTruthAssociationLink)(jet_orig) = jet->getAttribute< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink");
	  ATH_MSG_VERBOSE("GhostTruthAssociationLink: " << (*m_dec_GhostTruthAssociationLink)(jet_orig) );
	}
      }
      
      if(m_decoratetruthlabel){
        if(m_dec_Label->isAvailable(*jet)) (*m_dec_Label)(jet_orig)  = (*m_dec_Label)(*jet);
        if(m_dec_dRW->isAvailable(*jet)) (*m_dec_dRW)(jet_orig)  = (*m_dec_dRW)(*jet);
        if(m_dec_dRZ->isAvailable(*jet)) (*m_dec_dRZ)(jet_orig)  = (*m_dec_dRZ)(*jet);
        if(m_dec_dRTop->isAvailable(*jet)) (*m_dec_dRTop)(jet_orig)  = (*m_dec_dRTop)(*jet);
        if(m_dec_dRH->isAvailable(*jet)) (*m_dec_dRH)(jet_orig)  = (*m_dec_dRH)(*jet);
        if(m_dec_NB->isAvailable(*jet)) (*m_dec_NB)(jet_orig)  = (*m_dec_NB)(*jet);
        if(m_dec_TruthJetMass->isAvailable(*jet)) (*m_dec_TruthJetMass)(jet_orig)  = (*m_dec_TruthJetMass)(*jet);
      }

      if(m_decorateQGVariables){
	if(m_dec_AssociatedNTracks->isAvailable(*jet)) (*m_dec_AssociatedNTracks)(jet_orig) = (*m_dec_AssociatedNTracks)(*jet);
	if(m_dec_AssociatedTracksWidth->isAvailable(*jet)) (*m_dec_AssociatedTracksWidth)(jet_orig) = (*m_dec_AssociatedTracksWidth)(*jet);
	if(m_dec_AssociatedTracksC1->isAvailable(*jet)) (*m_dec_AssociatedTracksC1)(jet_orig) = (*m_dec_AssociatedTracksC1)(*jet);
	if(m_dec_Associated_truthjet_nCharged->isAvailable(*jet)) (*m_dec_Associated_truthjet_nCharged)(jet_orig) = (*m_dec_Associated_truthjet_nCharged)(*jet);
	if(m_dec_Associated_truthjet_pt->isAvailable(*jet)) (*m_dec_Associated_truthjet_pt)(jet_orig) = (*m_dec_Associated_truthjet_pt)(*jet);
	if(m_dec_Associated_truthjet_eta->isAvailable(*jet)) (*m_dec_Associated_truthjet_eta)(jet_orig) = (*m_dec_Associated_truthjet_eta)(*jet);
      }

    }

    return StatusCode::SUCCESS;
  }
}
