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
    m_dec_calibpt(0),
    m_dec_calibeta(0),
    m_dec_calibphi(0),
    m_dec_calibm(0),
    m_jetCalibTool(""),
    m_docalib(false),
    m_dec_jvt(0),
    m_jvtTool(""),
    m_dojvt(false),
    m_dobtag(false),
    m_jetTrackSumMomentsTool(""),
    m_decoratetracksum(false)
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
  }

  StatusCode JetAugmentationTool::initialize()
  {
    ATH_MSG_INFO("Initialising JetAugmentationTool");

    if(!m_jetCalibTool.empty()) {
      CHECK(m_jetCalibTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with calibration \"" << m_momentPrefix+m_calibMomentKey << "\"");
      m_docalib = true;

      m_dec_calibpt  = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_calibMomentKey+"_pt");
      m_dec_calibeta = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_calibMomentKey+"_eta");
      m_dec_calibphi = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_calibMomentKey+"_phi");
      m_dec_calibm   = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_calibMomentKey+"_m");

      if(!m_jvtTool.empty()) {
	CHECK(m_jvtTool.retrieve());
	ATH_MSG_INFO("Augmenting jets with updated JVT \"" << m_momentPrefix+m_jvtMomentKey << "\"");
	m_dojvt = true;

	m_dec_jvt  = new SG::AuxElement::Decorator<float>(m_momentPrefix+m_jvtMomentKey);

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

    if(!m_jetTrackSumMomentsTool.empty()) {
      CHECK(m_jetTrackSumMomentsTool.retrieve());
      ATH_MSG_INFO("Augmenting jets with track sum moments \"" << m_momentPrefix << "TrackSumMass,Pt\"");
      m_decoratetracksum = true;
      m_dec_tracksummass = new SG::AuxElement::Decorator<float>(m_momentPrefix+"TrackSumMass");
      m_dec_tracksumpt   = new SG::AuxElement::Decorator<float>(m_momentPrefix+"TrackSumPt");
    }

    return StatusCode::SUCCESS;
  }
    
  StatusCode JetAugmentationTool::finalize()
  {

    if(m_docalib) {
      delete m_dec_calibpt;
      delete m_dec_calibeta;
      delete m_dec_calibphi;
      delete m_dec_calibm;
    }

    if(m_dojvt) {
      delete m_dec_jvt;
    }

    if(m_dobtag) {
      for(const auto& pdec : m_dec_btag) delete pdec;
    }

    if(m_decoratetracksum){
      delete m_dec_tracksummass;
      delete m_dec_tracksumpt;
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
      if(m_jetTrackSumMomentsTool->modify(*jets_copy) )
      {
        ATH_MSG_WARNING("Problems calculating TrackSumMass and TrackSumPt");
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
    }

    return StatusCode::SUCCESS;
  }
}
