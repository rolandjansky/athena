/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

///////////////////////////////////////////////////////////////////
// JetAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Teng Jian Khoo (teng.jian.khoo@cern.ch)
//

#include "JetAugmentationTool.h"
#include "xAODEventInfo/EventInfo.h"
#include <xAODTruth/TruthParticle.h>
#include "xAODCore/ShallowCopy.h"
#include "JetAnalysisInterfaces/IJetJvtEfficiency.h"

namespace DerivationFramework {

  JetAugmentationTool::JetAugmentationTool(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p),
    dec_calibpt(0),
    dec_calibeta(0),
    dec_calibphi(0),
    dec_calibm(0),
    m_jetCalibTool(""),
    m_docalib(false),
    dec_jvt(0),
    dec_passJvt(0),
    m_jvtTool(""),
    m_jetJvtEfficiencyTool(""),
    m_dojvt(false),
    m_fjvtTool(""),
    m_dofjvt(false),
    m_customSumTrkTool(""),
    m_docustomSumTrk(false),
    m_customJvfTool(""),
    m_docustomJvf(false),
    m_MVfJvtTool(""),                 
    m_doMVfJvt(false),                
    m_dobtag(false),
    m_jetTrackSumMomentsTool(""),
    m_decoratetracksum(false),
    m_jetOriginCorrectionTool(""),
    m_decorateorigincorrection(false),
    dec_origincorrection(0),
    dec_originpt(0),
    dec_origineta(0),
    dec_originphi(0),
    dec_originm(0),
    m_jetPtAssociationTool(""),
    m_decorateptassociation(false),
    m_decorateQGVariables(false), // QGTaggerTool ---
    dec_AssociatedNTracks(0),
    dec_AssociatedTracksWidth(0),
    dec_AssociatedTracksC1(0),
    dec_Associated_truthjet_nCharged(0),
    dec_Associated_truthjet_pt(0),
    dec_Associated_truthjet_eta(0),
    m_trkSelectionTool(""),
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

    declareProperty("fJvtMomentKey",   m_fjvtMomentKey = "fJvt");
    declareProperty("JetForwardPFlowJvtTool",     m_fjvtTool);

    declareProperty("customSumTrkMomentKey",   m_customSumTrkMomentKey = "customSumPtTrkPt500");
    declareProperty("CustomSumTrkTool",     m_customSumTrkTool);

    declareProperty("customJvfMomentKey",   m_customJvfMomentKey = "customJVFCorr");
    declareProperty("CustomJvfTool",     m_customJvfTool);

    declareProperty("MVfJvtMomentKey", m_MVfJvtMomentKey = "MVfJVT");
    declareProperty("JetForwardJvtToolBDT", m_MVfJvtTool);
    
    declareProperty("JetBtagTools",   m_btagSelTools);
    declareProperty("JetBtagWPs",     m_btagWP);
    declareProperty("JetTrackSumMomentsTool", m_jetTrackSumMomentsTool);
    declareProperty("JetPtAssociationTool", m_jetPtAssociationTool);
    declareProperty("JetOriginCorrectionTool",m_jetOriginCorrectionTool);
    declareProperty("TrackSelectionTool", m_trkSelectionTool); // QGTaggerTool ---
    declareProperty("JetTruthLabelingTool", m_jetTruthLabelingTool);
  }

  StatusCode JetAugmentationTool::initialize()
  {
    ATH_MSG_INFO("Initialising JetAugmentationTool");

        if(!m_customSumTrkTool.empty()) {
          CHECK(m_customSumTrkTool.retrieve());
          m_docustomSumTrk = true;
          dec_customSumTrk  = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_customSumTrkMomentKey);
        }
        if(!m_customJvfTool.empty()) {
          CHECK(m_customJvfTool.retrieve());
          m_docustomJvf = true;
          dec_customJvf  = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_customJvfMomentKey);
        }

    if(!m_jetCalibTool.empty()) {
      CHECK(m_jetCalibTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with calibration \"" << m_momentPrefix+m_calibMomentKey << "\"");
      m_docalib = true;

      dec_calibpt  = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_calibMomentKey+"_pt");
      dec_calibeta = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_calibMomentKey+"_eta");
      dec_calibphi = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_calibMomentKey+"_phi");
      dec_calibm   = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_calibMomentKey+"_m");


      if(!m_jvtTool.empty()) {
        CHECK(m_jvtTool.retrieve());
        ATH_MSG_INFO("Augmenting jets with updated JVT \"" << m_momentPrefix+m_jvtMomentKey << "\"");
        m_dojvt = true;

        dec_jvt  = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_jvtMomentKey);
        dec_passJvt  = new SG::AuxElement::Decorator<char>(m_momentPrefix+"pass"+m_jvtMomentKey);

        if(!m_MVfJvtTool.empty()) {
          CHECK(m_MVfJvtTool.retrieve());
          ATH_MSG_INFO("Augmenting jets with MV-fJVT value \"" << m_momentPrefix+m_MVfJvtMomentKey << "\"");
          m_doMVfJvt = true;

          dec_MVfJvt                    = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_MVfJvtMomentKey);
          /* MVfJvt inputs. The tagger also requires jet timing and width that are in (resp.) jet & MET cp smartslimming lists
             Last needed variable is fjvt that is recomputed in analyses in case the moment below are required */
          dec_MVfJvt_Sumcle             = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_MVfJvtMomentKey+"_Sumcle");
          dec_MVfJvt_SumclIso           = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_MVfJvtMomentKey+"_SumclIso");
          dec_MVfJvt_SumclEMprob        = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_MVfJvtMomentKey+"_SumclEMprob");
          dec_MVfJvt_LeadclWidth        = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_MVfJvtMomentKey+"_LeadclWidth");
          dec_MVfJvt_LeadclSecondLambda = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_MVfJvtMomentKey+"_LeadclSecondLambda");
        }

        if(!m_fjvtTool.empty()) {
          CHECK(m_fjvtTool.retrieve());
          ATH_MSG_INFO("Augmenting (PFlow) jets with fJVT \"" << m_momentPrefix+m_fjvtMomentKey << "\"");
          m_dofjvt = true;

          dec_fjvt  = std::make_unique< SG::AuxElement::Decorator<float> >(m_momentPrefix+m_fjvtMomentKey);
        }


        if(!m_btagSelTools.empty()) {
          size_t ibtag(0);
          for(const auto& tool : m_btagSelTools) {
            CHECK(tool.retrieve());
            ATH_MSG_INFO("Augmenting jets with B-tag working point \"" << m_momentPrefix+m_btagWP[ibtag] << "\"");
            dec_btag.push_back(new SG::AuxElement::Decorator<float>(m_momentPrefix+m_btagWP[ibtag]));
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
      dec_tracksummass = new SG::AuxElement::Decorator<float>(m_momentPrefix+"TrackSumMass");
      dec_tracksumpt   = new SG::AuxElement::Decorator<float>(m_momentPrefix+"TrackSumPt");
    }

    // This tool creates the GhostTruthAssociation decorations recommended for truth matching //
    if(!m_jetPtAssociationTool.empty())
    {
      CHECK(m_jetPtAssociationTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with GhostTruthAssociation moments Link and Fraction");
      m_decorateptassociation = true;
      dec_GhostTruthAssociationFraction = new SG::AuxElement::Decorator<float>("GhostTruthAssociationFraction");
      dec_GhostTruthAssociationLink     = new SG::AuxElement::Decorator< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink");
    }

    // Here it for ntracks decoration --- QGTaggerTool ---
    // set up InDet selection tool
    if(!m_trkSelectionTool.empty()) {
      CHECK( m_trkSelectionTool.retrieve() );
      m_decorateQGVariables = true; 
      dec_AssociatedNTracks     = new SG::AuxElement::Decorator<int>(m_momentPrefix + "QGTagger_NTracks");
      dec_AssociatedTracksWidth = new SG::AuxElement::Decorator<float>(m_momentPrefix + "QGTagger_TracksWidth");
      dec_AssociatedTracksC1    = new SG::AuxElement::Decorator<float>(m_momentPrefix + "QGTagger_TracksC1");
      dec_Associated_truthjet_nCharged = new SG::AuxElement::Decorator<int>(m_momentPrefix + "QGTagger_truthjet_nCharged");
      dec_Associated_truthjet_pt       = new SG::AuxElement::Decorator<float>(m_momentPrefix + "QGTagger_truthjet_pt");
      dec_Associated_truthjet_eta      = new SG::AuxElement::Decorator<float>(m_momentPrefix + "QGTagger_truthjet_eta");
    } // now works


    if(!m_jetOriginCorrectionTool.empty()) {
      CHECK(m_jetOriginCorrectionTool.retrieve());
      ATH_MSG_DEBUG("Augmenting jets with origin corrections \"" << m_momentPrefix << "Origin\"");
      m_decorateorigincorrection = true;
      dec_origincorrection = new SG::AuxElement::Decorator<ElementLink<xAOD::VertexContainer>>(m_momentPrefix+"JetOriginConstitScaleMomentum_OriginVertex");
      dec_originpt  = new SG::AuxElement::Decorator<float>(m_momentPrefix+"JetOriginConstitScaleMomentum_pt");
      dec_origineta = new SG::AuxElement::Decorator<float>(m_momentPrefix+"JetOriginConstitScaleMomentum_eta");
      dec_originphi = new SG::AuxElement::Decorator<float>(m_momentPrefix+"JetOriginConstitScaleMomentum_phi");
      dec_originm   = new SG::AuxElement::Decorator<float>(m_momentPrefix+"JetOriginConstitScaleMomentum_m");  
    }

    if(!m_jetTruthLabelingTool.empty()) {
      CHECK(m_jetTruthLabelingTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with truthlabeling");
      m_decoratetruthlabel = true;
      m_truthLabelName = m_jetTruthLabelingTool->getLargeRJetTruthLabelName();
      dec_Label = std::make_unique< SG::AuxElement::Decorator<int> >(m_truthLabelName);
      dec_dRW = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_W");
      dec_dRZ = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_Z");
      dec_dRTop = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_Top");
      dec_dRH = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_dR_H");
      dec_NB = std::make_unique< SG::AuxElement::Decorator<int> >(m_truthLabelName+"_NB");
      dec_TruthJetMass = std::make_unique< SG::AuxElement::Decorator<float> >(m_truthLabelName+"_TruthJetMass");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetAugmentationTool::finalize()
  {

    if(m_docalib) {
      delete dec_calibpt;
      delete dec_calibeta;
      delete dec_calibphi;
      delete dec_calibm;
    }

    if(m_dojvt) {
      delete dec_jvt;
      delete dec_passJvt;
    }

    if(m_dobtag) {
      for(const auto& pdec : dec_btag) delete pdec;
    }

    if(m_decoratetracksum){
      delete dec_tracksummass;
      delete dec_tracksumpt;
    }

    if(m_decorateptassociation)
    {
      delete dec_GhostTruthAssociationFraction;
      delete dec_GhostTruthAssociationLink;
    }

    // QGTaggerTool ---
    if(m_decorateQGVariables)
    {
      delete dec_AssociatedNTracks;
      delete dec_AssociatedTracksWidth;
      delete dec_AssociatedTracksC1;
      delete dec_Associated_truthjet_nCharged;
      delete dec_Associated_truthjet_pt;
      delete dec_Associated_truthjet_eta;
    }

    if(m_decorateorigincorrection){
      delete dec_origincorrection;
      delete dec_originpt;
      delete dec_originphi;
      delete dec_origineta;
      delete dec_originm; 
    }

    return StatusCode::SUCCESS;
  }

  StatusCode JetAugmentationTool::addBranches() const
  {

    // retrieve container
    const xAOD::JetContainer* jets(0);
    if( evtStore()->retrieve( jets, m_containerName ).isFailure() ) {
      ATH_MSG_ERROR("Couldn't retrieve jets with key: " << m_containerName );
      return StatusCode::FAILURE;
    }

    // make a shallow copy of the jets
    std::pair<xAOD::JetContainer*,xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*jets);
    std::unique_ptr<xAOD::JetContainer> jets_copy(shallowcopy.first);
    std::unique_ptr<xAOD::ShallowAuxContainer> jets_copyaux(shallowcopy.second);

    // if we have a calibration tool, apply the calibration
    if(m_docalib) {
      if(m_jetCalibTool->modify(*jets_copy) ) {
        ATH_MSG_ERROR("Problem applying jet calibration");
        return StatusCode::FAILURE;
      }
    }

    if(m_decoratetracksum){
      if( m_jetTrackSumMomentsTool->modify(*jets_copy) )
      {
        ATH_MSG_ERROR("Problems calculating TrackSumMass and TrackSumPt");
        return StatusCode::FAILURE;
      }
    }

    if(m_decorateorigincorrection){
      if(m_jetOriginCorrectionTool->modify(*jets_copy))
      {
        ATH_MSG_ERROR("Problem applying the origin correction tool");
        return StatusCode::FAILURE;
      }
    }

    // Check if GhostTruthAssociation decorations already exist for first jet, and if so skip them //
    bool isMissingPtAssociation = true;
    if( !m_decorateptassociation || jets_copy->size() == 0 || dec_GhostTruthAssociationFraction->isAvailable(*jets_copy->at(0)) ) {
      isMissingPtAssociation = false;
      //ATH_MSG_WARNING("decoratept: " << !m_decorateptassociation << " copy: " << (jets_copy->size() == 0) << " " << dec_GhostTruthAssociationFraction->isAvailable(*jets_copy->at(0)));
    }

    if(m_decorateptassociation && isMissingPtAssociation){
      ATH_MSG_DEBUG("tool running" );
      if( m_jetPtAssociationTool->modify(*jets_copy) )
      {
        ATH_MSG_ERROR("Problem running the JetPtAssociationTool");
        return StatusCode::FAILURE;
      }
    }

    if(m_decoratetruthlabel){
      if( m_jetTruthLabelingTool->modify(*jets_copy) )
      {
        ATH_MSG_ERROR("Problems applying large-R jet truth labels");
        return StatusCode::FAILURE;
      }
    }

    // JVT has to be updated here to get correct calibrated values of (MV)fJVT
    if(m_docalib && m_dojvt && (m_doMVfJvt || m_dofjvt) ){

      for(const xAOD::Jet *jet : *jets_copy) { 
        (*dec_jvt)(*jet) = m_jvtTool->updateJvt(*jet);
      }

      if(m_doMVfJvt){
        if(m_MVfJvtTool->modify(*jets_copy))
        {
          ATH_MSG_ERROR("Problem with MVfJvtTool modify function");
          return StatusCode::FAILURE;
        }
      }

      if(m_dofjvt){
        if(m_fjvtTool->modify(*jets_copy))
        {
          ATH_MSG_ERROR("Problem computing fJVT");
          return StatusCode::FAILURE;
        }
      }

    }
    if(m_docustomSumTrk){
      if(m_customSumTrkTool->modify(*jets_copy))
      {
        ATH_MSG_ERROR("Problem computing custom jvt");
        return StatusCode::FAILURE;
      }
    }
    if(m_docustomJvf){
      if(m_customJvfTool->modify(*jets_copy))
      {
        ATH_MSG_ERROR("Problem computing custom jvt");
        return StatusCode::FAILURE;
      }
    }

    // loop over the copies
    for(const xAOD::Jet *jet : *jets_copy) {
      // get the original jet so we can decorate it
      const xAOD::Jet& jet_orig( *(*jets)[jet->index()] );

          if(m_docustomSumTrk) {
            (*dec_customSumTrk)(jet_orig) = jet->auxdata<float>(m_customSumTrkMomentKey);
          }
          if(m_docustomJvf) {
            (*dec_customJvf)(jet_orig) = jet->auxdata<float>(m_customJvfMomentKey);
          }

      if(m_docalib) {
        // generate static decorators to avoid multiple lookups
        (*dec_calibpt)(jet_orig)  = jet->pt();
        (*dec_calibeta)(jet_orig) = jet->eta();
        (*dec_calibphi)(jet_orig) = jet->phi();
        (*dec_calibm)(jet_orig)   = jet->m();

        ATH_MSG_VERBOSE("Calibrated jet pt: " << (*dec_calibpt)(jet_orig) );

        if(m_dojvt) {
          if(!m_doMVfJvt && !m_dofjvt)(*dec_jvt)(jet_orig) = m_jvtTool->updateJvt(*jet);
          else (*dec_jvt)(jet_orig) = (*dec_jvt)(*jet);
          ATH_MSG_VERBOSE("Calibrated JVT: " << (*dec_jvt)(jet_orig) );
          bool passJVT = m_jetJvtEfficiencyTool->passesJvtCut(jet_orig);
          (*dec_passJvt)(jet_orig) = passJVT;

          if(m_doMVfJvt) {   
            (*dec_MVfJvt)(jet_orig)                    = jet->auxdata<float>("MVfJVT");
            (*dec_MVfJvt_Sumcle)(jet_orig)             = jet->auxdata<float>("Sumcle");
            (*dec_MVfJvt_SumclIso)(jet_orig)           = jet->auxdata<float>("SumclIso");
            (*dec_MVfJvt_SumclEMprob)(jet_orig)        = jet->auxdata<float>("SumclEMprob");
            (*dec_MVfJvt_LeadclWidth)(jet_orig)        = jet->auxdata<float>("LeadclWidth");
            (*dec_MVfJvt_LeadclSecondLambda)(jet_orig) = jet->auxdata<float>("LeadclSecondLambda");
            ATH_MSG_VERBOSE("What goes in decoration:  m_cle = " << jet->auxdata<float>("Sumcle")
                << " || m_cliso = "                  << jet->auxdata<float>("SumclIso")
                << " || m_clemprob = "               << jet->auxdata<float>("SumclEMprob")
                << " || m_cletawidth = "             << jet->auxdata<float>("LeadclWidth")
                << " || m_cllambda2 = "              << jet->auxdata<float>("LeadclSecondLambda")  );

          }

          if(m_dofjvt) {
            ATH_MSG_DEBUG( "fJvt value = " << jet->auxdata<float>(m_fjvtMomentKey) );
            (*dec_fjvt)(jet_orig) = jet->auxdata<float>(m_fjvtMomentKey);
          }

          if(m_dobtag) {
            size_t ibtag(0);
            for(const auto& tool : m_btagSelTools) {
              (*dec_btag[ibtag])(jet_orig) = std::abs(jet->eta())<2.7 && passJVT && tool->accept(*jet);
              ATH_MSG_VERBOSE("Btag working point \"" << m_btagWP[ibtag] << "\" " << ((*dec_btag[ibtag])(jet_orig) ? "passed." : "failed."));
              ++ibtag;
            }
          }
        }
      }

      if(m_decoratetracksum) {
        (*dec_tracksummass)(jet_orig) = jet->getAttribute<float>("TrackSumMass");
        (*dec_tracksumpt)(jet_orig)   = jet->getAttribute<float>("TrackSumPt");
        ATH_MSG_VERBOSE("TrackSumMass: " << (*dec_tracksummass)(jet_orig) );
        ATH_MSG_VERBOSE("TrackSumPt: "   << (*dec_tracksummass)(jet_orig) );
      }

      if(m_decorateorigincorrection) {  
        (*dec_originpt)(jet_orig)  = (*dec_originpt)(*jet);
        (*dec_origineta)(jet_orig) = (*dec_origineta)(*jet);
        (*dec_originphi)(jet_orig) = (*dec_originphi)(*jet);
        (*dec_originm)(jet_orig)   = (*dec_originm)(*jet);
        (*dec_origincorrection)(jet_orig) = jet->getAttribute<ElementLink<xAOD::VertexContainer> >("OriginVertex");
        ATH_MSG_VERBOSE("OriginCorrection: " << (*dec_origincorrection)(jet_orig) );
      }

      if(m_decorateptassociation && isMissingPtAssociation){
        (*dec_GhostTruthAssociationFraction)(jet_orig) = jet->getAttribute<float>("GhostTruthAssociationFraction");
        ATH_MSG_VERBOSE("GhostTruthAssociationFraction: " << (*dec_GhostTruthAssociationFraction)(jet_orig) );
        (*dec_GhostTruthAssociationLink)(jet_orig) = jet->getAttribute< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink");
        ATH_MSG_VERBOSE("GhostTruthAssociationLink: " << (*dec_GhostTruthAssociationLink)(jet_orig) );
      }

      // QGTaggerTool ---
      if(m_decorateQGVariables)
      {
        ATH_MSG_DEBUG("Test Decorate QG ");
        std::vector<const xAOD::IParticle*> jettracks;
        jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack,jettracks);

        int nTracksCount = 0;
        double TracksWidth = 0., SumTracks_pTs = 0., TracksC1 = 0., beta = 0.2;
        bool invalidJet = false;

        const xAOD::Vertex *pv = 0;
        const xAOD::VertexContainer* vxCont = 0;
        if(evtStore()->retrieve( vxCont, "PrimaryVertices" ).isFailure()){
          ATH_MSG_WARNING("Unable to retrieve primary vertex container PrimaryVertices");
          nTracksCount = -1;
          TracksWidth = -1.;
          TracksC1 = -1.;
          invalidJet = true;
        }
        else if(vxCont->empty()){
          ATH_MSG_WARNING("Event has no primary vertices!");
          nTracksCount = -1;
          TracksWidth = -1.;
          TracksC1 = -1.;
          invalidJet = true;
        }
        else{
          for(const auto& vx : *vxCont){
            // take the first vertex in the list that is a primary vertex
            if(vx->vertexType()==xAOD::VxType::PriVtx){
              pv = vx;
              break;
            }
          }
        }

        std::vector<bool> IsGoodTrack;
        TLorentzVector tracki_TLV, trackj_TLV;
        TLorentzVector jet_TLV = jet -> p4();
        for (size_t i = 0; i < jettracks.size(); i++) {

          if(invalidJet) continue;

          const xAOD::TrackParticle* trk = static_cast<const xAOD::TrackParticle*>(jettracks[i]);

          // only count tracks with selections
          // 1) pt>500 MeV
          // 2) accepted track from InDetTrackSelectionTool with CutLevel==Loose
          // 3) associated to primary vertex OR within 3mm of the primary vertex
          bool accept = (trk->pt()>500 &&
              m_trkSelectionTool->accept(*trk) &&
              (trk->vertex()==pv || (!trk->vertex() && fabs((trk->z0()+trk->vz()-pv->z())*sin(trk->theta()))<3.))
              );	    

          ATH_MSG_DEBUG("Test Decorate QG: trkSelTool output " << m_trkSelectionTool->accept(*trk) );

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

        for(size_t i = 0; i < jettracks.size(); i++) {
          if(invalidJet) continue;
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
        const xAOD::EventInfo* eventInfo = nullptr;
        ATH_CHECK(evtStore()->retrieve(eventInfo,"EventInfo"));
        bool isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
        int tntrk = 0;
        float truthjet_pt  = -999.0;
        float truthjet_eta = -999.0;
        if(isMC){
          const xAOD::Jet* tjet=nullptr;
          //tjet = * (jet->getAttribute< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink"));
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
        }// end truth QG tagging

        (*dec_AssociatedNTracks)(jet_orig)       = nTracksCount;
        (*dec_AssociatedTracksWidth)(jet_orig)   = TracksWidth;
        (*dec_AssociatedTracksC1)(jet_orig)      = TracksC1;
        (*dec_Associated_truthjet_nCharged)(jet_orig) = tntrk;
        (*dec_Associated_truthjet_pt)(jet_orig)       = truthjet_pt;
        (*dec_Associated_truthjet_eta)(jet_orig)      = truthjet_eta;
      }// end if m_decorateQGVariables

      if(m_decoratetruthlabel){
        if(dec_Label->isAvailable(*jet)) (*dec_Label)(jet_orig)  = (*dec_Label)(*jet);
        if(dec_dRW->isAvailable(*jet)) (*dec_dRW)(jet_orig)  = (*dec_dRW)(*jet);
        if(dec_dRZ->isAvailable(*jet)) (*dec_dRZ)(jet_orig)  = (*dec_dRZ)(*jet);
        if(dec_dRTop->isAvailable(*jet)) (*dec_dRTop)(jet_orig)  = (*dec_dRTop)(*jet);
        if(dec_dRH->isAvailable(*jet)) (*dec_dRH)(jet_orig)  = (*dec_dRH)(*jet);
        if(dec_NB->isAvailable(*jet)) (*dec_NB)(jet_orig)  = (*dec_NB)(*jet);
        if(dec_TruthJetMass->isAvailable(*jet)) (*dec_TruthJetMass)(jet_orig)  = (*dec_TruthJetMass)(*jet);
      }

    }//end loop on jets copies

    return StatusCode::SUCCESS;
  }//end addBranches

}
