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
#include "JetJvtEfficiency/JetJvtEfficiency.h"

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
    m_jetJvtEfficiencyTool("CP::JetJvtEfficiency/JetJvtEfficiencyTool"),
    m_dojvt(false),
    m_dobtag(false),
    m_jetTrackSumMomentsTool(""),
    m_decoratetracksum(false),
    m_jetPtAssociationTool(""),
    m_decorateptassociation(false),
    m_jetOriginCorrectionTool(""),
    m_decorateorigincorrection(false),
    dec_originpt(0),
    dec_origineta(0),
    dec_originphi(0),
    dec_originm(0),
    dec_origincorrection(0)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("MomentPrefix",   m_momentPrefix = "DFCommonJets_");
    declareProperty("InputJets",      m_containerName = "AntiKt4EMTopoJets");
    // declareProperty("CalibScale",     m_calibScale = "JetGSCScaleMomentum");
    declareProperty("CalibMomentKey", m_calibMomentKey = "Calib");
    declareProperty("JetCalibTool",   m_jetCalibTool);
    declareProperty("JvtMomentKey",   m_jvtMomentKey = "Jvt");
    declareProperty("JetJvtTool",     m_jvtTool);
    declareProperty("JetBtagTools",   m_btagSelTools);
    declareProperty("JetBtagWPs",     m_btagWP);
    declareProperty("JetTrackSumMomentsTool", m_jetTrackSumMomentsTool);
    declareProperty("JetPtAssociationTool", m_jetPtAssociationTool);
    declareProperty("JetOriginCorrectionTool",m_jetOriginCorrectionTool);
  }

  StatusCode JetAugmentationTool::initialize()
  {
    ATH_MSG_INFO("Initialising JetAugmentationTool");

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
    if(!m_jetPtAssociationTool.empty()) {
      CHECK(m_jetPtAssociationTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with GhostTruthAssociation moments Link and Fraction");
      m_decorateptassociation = true;
      dec_GhostTruthAssociationFraction = new SG::AuxElement::Decorator<float>("GhostTruthAssociationFraction");
      dec_GhostTruthAssociationLink     = new SG::AuxElement::Decorator< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink");
    }

    if(!m_jetOriginCorrectionTool.empty()) {
      CHECK(m_jetOriginCorrectionTool.retrieve());
      ATH_MSG_DEBUG("Augmenting jets with origin corrections \"" << m_momentPrefix << "Origin\"");
      m_decorateorigincorrection = true;
      dec_origincorrection = new SG::AuxElement::Decorator<ElementLink<xAOD::VertexContainer>>(m_momentPrefix+"JetOriginConstitMomentumScale_OriginVertex");
      dec_originpt  = new SG::AuxElement::Decorator<float>(m_momentPrefix+"JetOriginConstitMomentumScale_pt");
      dec_origineta = new SG::AuxElement::Decorator<float>(m_momentPrefix+"JetOriginConstitMomentumScale_eta");
      dec_originphi = new SG::AuxElement::Decorator<float>(m_momentPrefix+"JetOriginConstitMomentumScale_phi");
      dec_originm   = new SG::AuxElement::Decorator<float>(m_momentPrefix+"JetOriginConstitMomentumScale_m");  
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

    if(m_decorateptassociation){
      delete dec_GhostTruthAssociationFraction;
      delete dec_GhostTruthAssociationLink;
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
      ATH_MSG_WARNING ("Couldn't retrieve jets with key: " << m_containerName );
      return StatusCode::FAILURE;
    }

    // make a shallow copy of the jets
    std::pair<xAOD::JetContainer*,xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*jets);
    std::unique_ptr<xAOD::JetContainer> jets_copy(shallowcopy.first);
    std::unique_ptr<xAOD::ShallowAuxContainer> jets_copyaux(shallowcopy.second);

      // if we have a calibration tool, apply the calibration
    if(m_docalib) {
      if(m_jetCalibTool->modify(*jets_copy) ) {
        ATH_MSG_WARNING("Problem applying jet calibration");
        return StatusCode::FAILURE;
      }
    }

    if(m_decoratetracksum){
      if( m_jetTrackSumMomentsTool->modify(*jets_copy) )
      {
        ATH_MSG_WARNING("Problems calculating TrackSumMass and TrackSumPt");
        return StatusCode::FAILURE;
      }
    }

    if(m_decorateorigincorrection){
      if(m_jetOriginCorrectionTool->modify(*jets_copy))
	{
	  ATH_MSG_WARNING("Problem applying the origin correction tool");
	  return StatusCode::FAILURE;
	}
    }

    // Check if GhostTruthAssociation decorations already exist for first jet, and if so skip them //
    bool isMissingPtAssociation = true;
    if( !m_decorateptassociation || jets_copy->size() == 0 || dec_GhostTruthAssociationFraction->isAvailable(*jets_copy->at(0)) ) {
      isMissingPtAssociation = false;
    }

    if(m_decorateptassociation && isMissingPtAssociation){
      if( m_jetPtAssociationTool->modify(*jets_copy) )
      {
        ATH_MSG_WARNING("Problem running the JetPtAssociationTool");
        return StatusCode::FAILURE;
      }
    }

    // loop over the copies
    for(const auto& jet : *jets_copy) {
      // get the original jet so we can decorate it
      const xAOD::Jet& jet_orig( *(*jets)[jet->index()] );

      if(m_docalib) {
        // generate static decorators to avoid multiple lookups
        (*dec_calibpt)(jet_orig)  = jet->pt();
        (*dec_calibeta)(jet_orig) = jet->eta();
        (*dec_calibphi)(jet_orig) = jet->phi();
        (*dec_calibm)(jet_orig)   = jet->m();

        ATH_MSG_VERBOSE("Calibrated jet pt: " << (*dec_calibpt)(jet_orig) );

        if(m_dojvt) {
          (*dec_jvt)(jet_orig) = m_jvtTool->updateJvt(*jet);
          ATH_MSG_VERBOSE("Calibrated JVT: " << (*dec_jvt)(jet_orig) );
          bool passJVT = m_jetJvtEfficiencyTool->passesJvtCut(jet_orig);
                (*dec_passJvt)(jet_orig) = passJVT;

          if(m_dobtag) {
            size_t ibtag(0);
            for(const auto& tool : m_btagSelTools) {
              (*dec_btag[ibtag])(jet_orig) = jet->pt()>20e3 && fabs(jet->eta())<2.5 && passJVT && tool->accept(*jet);
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
	(*dec_originpt)(jet_orig)  = jet->pt();
        (*dec_origineta)(jet_orig) = jet->eta();
        (*dec_originphi)(jet_orig) = jet->phi();
        (*dec_originm)(jet_orig)   = jet->m();
        (*dec_origincorrection)(jet_orig) = jet->getAttribute<ElementLink<xAOD::VertexContainer> >("OriginVertex");
	ATH_MSG_VERBOSE("OriginCorrection: " << (*dec_origincorrection)(jet_orig) );
      }

      if(m_decorateptassociation && isMissingPtAssociation){
        (*dec_GhostTruthAssociationFraction)(jet_orig) = jet->getAttribute<float>("GhostTruthAssociationFraction");
        ATH_MSG_VERBOSE("GhostTruthAssociationFraction: " << (*dec_GhostTruthAssociationFraction)(jet_orig) );
        (*dec_GhostTruthAssociationLink)(jet_orig) = jet->getAttribute< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink");
        ATH_MSG_VERBOSE("GhostTruthAssociationLink: " << (*dec_GhostTruthAssociationLink)(jet_orig) );
      }
    }

    return StatusCode::SUCCESS;
  }
}
