///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetForwardPFlowJvtTool.cxx
// Implementation file for class JetForwardPFlowJvtTool
// Author: Matt Klein<matthew.henry.klein@cern.ch>
///////////////////////////////////////////////////////////////////

// JetForwardPFlowJvtTool includes
#include "JetMomentTools/JetForwardPFlowJvtTool.h"

// Jet EDM
#include "xAODJet/JetAttributes.h"

//
#include "JetRec/JetRecTool.h"
#include "JetRec/JetFinder.h"
// Shallow copy
//#include "xAODCore/ShallowCopy.h"

    static SG::AuxElement::Decorator<char>  isHS("isJvtHS");
    static SG::AuxElement::Decorator<char>  isPU("isJvtPU");
    static SG::AuxElement::Decorator<float>  fjvt_dec("fJvt");

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  JetForwardPFlowJvtTool::JetForwardPFlowJvtTool(const std::string& name) :
    AsgTool(name),
    m_fjvtThresh(15e3)
  {
    declareProperty("OverlapDec",         m_orLabel          = ""               );
    declareProperty("OutputDec",          m_outLabel         = "passFJVT"       );
    declareProperty("OutputDecFjvt",      m_outLabelFjvt     = "passOnlyFJVT"   );
    declareProperty("OutputDecTiming",    m_outLabelTiming   = "passOnlyTiming" );
    declareProperty("VertexContainer",    m_verticesName     = "PrimaryVertices");
    declareProperty("EtaThresh",          m_etaThresh        = 2.5              );
    declareProperty("TimingCut",          m_timingCut        = 10.              );
    declareProperty("ForwardMinPt",       m_forwardMinPt     = 20e3             );
    declareProperty("ForwardMaxPt",       m_forwardMaxPt     = 60e3             );
    declareProperty("CentralMinPt",       m_centerMinPt      = 20e3             );
    declareProperty("CentralMaxPt",       m_centerMaxPt      = -1               );
    declareProperty("CentralJvtThresh",   m_centerJvtThresh  = 0.11             );
    declareProperty("JvtMomentName",      m_jvtMomentName    = "Jvt"            );
    declareProperty("CentralDrptThresh",  m_centerDrptThresh = 0.2              );
    declareProperty("CentralMaxStochPt",  m_maxStochPt       = 35e3             );
    declareProperty("JetScaleFactor",     m_jetScaleFactor   = 0.4              );
    declareProperty("UseTightOP",         m_tightOP          = false            );//Tight or Loose
  }

  // Destructor
  ///////////////
  JetForwardPFlowJvtTool::~JetForwardPFlowJvtTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode JetForwardPFlowJvtTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    if (m_tightOP) m_fjvtThresh = 0.4;
    else m_fjvtThresh = 0.5;
    if (m_orLabel!="")  Dec_OR = std::make_unique<SG::AuxElement::Decorator<char> >(m_orLabel);
    Dec_out = std::make_unique<SG::AuxElement::Decorator<char> >(m_outLabel);
    Dec_outFjvt = std::make_unique<SG::AuxElement::Decorator<char> >(m_outLabelFjvt);
    Dec_outTiming = std::make_unique<SG::AuxElement::Decorator<char> >(m_outLabelTiming);

    JetFinder *jfd = new JetFinder("Jfd");
    ATH_CHECK(jfd->setProperty("JetAlgorithm","AntiKt"));
    ATH_CHECK(jfd->setProperty("JetRadius",0.4));
    //ATH_CHECK(jfd->setProperty("",""));
    //ATH_CHECK(jfd->setProperty("",""));
    ATH_CHECK(jfd->initialize());


    JetRecTool *jrc = new JetRecTool("Jrc");
    ATH_CHECK(jrc->setProperty("JetFinder",jfd));
    ATH_CHECK(jrc->initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode JetForwardPFlowJvtTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  int JetForwardPFlowJvtTool::modify(xAOD::JetContainer& jetCont) const {
    getPV();
    m_pileupMomenta.clear();
    for(const auto& jetF : jetCont) {
      (*Dec_out)(*jetF) = 1;
      (*Dec_outFjvt)(*jetF) = 1;
      (*Dec_outTiming)(*jetF) = 1;
      fjvt_dec(*jetF) = 0;
      if (!forwardJet(jetF)) continue;
      if (m_pileupMomenta.size()==0) calculateVertexMomenta(&jetCont);
      double fjvt = getFJVT(jetF)/jetF->pt();
      if (fjvt>m_fjvtThresh) (*Dec_outFjvt)(*jetF) = 0;
      if (fabs(jetF->auxdata<float>("Timing"))>m_timingCut) (*Dec_outTiming)(*jetF) = 0;
      if (fjvt>m_fjvtThresh || fabs(jetF->auxdata<float>("Timing"))>m_timingCut) (*Dec_out)(*jetF) = 0;
      fjvt_dec(*jetF) = fjvt;
    }
    return 0;
  }

  float JetForwardPFlowJvtTool::getFJVT(const xAOD::Jet *jet) const {
    TVector2 fjet(-jet->pt()*cos(jet->phi()),-jet->pt()*sin(jet->phi()));
    double fjvt = 0;
    for (size_t pui = 0; pui < m_pileupMomenta.size(); pui++) {
      if (pui==m_pvind) continue;
      double projection = m_pileupMomenta[pui]*fjet/fjet.Mod();
      if (projection>fjvt) fjvt = projection;
    }
    //fjvt += getCombinedWidth(jet);
    return fjvt;
  }

  void JetForwardPFlowJvtTool::calculateVertexMomenta(const xAOD::JetContainer *jets) const {
    m_pileupMomenta.clear();
    const xAOD::MissingETContainer* trkMet  = nullptr;
    if( evtStore()->retrieve(trkMet, "MET_Track").isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve MET_Track container");
    }
    const xAOD::VertexContainer *vxCont = 0;
    if( evtStore()->retrieve(vxCont, m_verticesName).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve primary vertex container");
    }
    for(const auto& vx : *vxCont) {
      if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
      TString vname = "PVTrack_vx";
      vname += vx->index();
      m_pileupMomenta.push_back((vx->index()==m_pvind?0:-(1./m_jetScaleFactor))*TVector2(0.5*(*trkMet)[vname.Data()]->mpx(),0.5*(*trkMet)[vname.Data()]->mpy()));
    }
    for (const auto& jet : *jets) {
      if (!centralJet(jet)) continue;
      int jetvert = getJetVertex(jet);
      if (jetvert>=0) m_pileupMomenta[jetvert] += TVector2(0.5*jet->pt()*cos(jet->phi()),0.5*jet->pt()*sin(jet->phi())); 
    }
  }

  float JetForwardPFlowJvtTool::getCombinedWidth(const xAOD::Jet *jet) const {
    float Width = 0;
    float CWidth = 0;
    float ptsum = 0;
    jet->getAttribute(xAOD::JetAttribute::Width,Width);
    xAOD::JetConstituentVector constvec = jet->getConstituents();
    for (xAOD::JetConstituentVector::iterator it = constvec.begin(); it != constvec.end(); it++) {
      const xAOD::CaloCluster *cl = static_cast<const xAOD::CaloCluster*>((*it)->rawConstituent());
      float secondR = cl->getMomentValue(xAOD::CaloCluster::MomentType::SECOND_R);
      float centermag = cl->getMomentValue(xAOD::CaloCluster::MomentType::CENTER_MAG);
      CWidth+=fabs(cl->pt()*atan(sqrt(secondR)/centermag)*cosh(cl->eta()));
      ptsum += cl->pt();
    }
    CWidth /= ptsum;
    return (CWidth + Width);
  }

  bool JetForwardPFlowJvtTool::forwardJet(const xAOD::Jet *jet) const {
    if (fabs(jet->eta())<m_etaThresh) return false;
    if (jet->pt()<m_forwardMinPt || jet->pt()>m_forwardMaxPt) return false;
    return true;
  }

  bool JetForwardPFlowJvtTool::centralJet(const xAOD::Jet *jet) const {
    if (fabs(jet->eta())>m_etaThresh) return false;
    if (jet->pt()<m_centerMinPt || (m_centerMaxPt>0 && jet->pt()>m_centerMaxPt)) return false;
    if (Dec_OR && !(*Dec_OR)(*jet)) return false;
    float jvt = 0;
    jet->getAttribute<float>(m_jvtMomentName,jvt);
    if (jvt>m_centerJvtThresh) return false;
    if (jet->pt()<m_maxStochPt && getDrpt(jet)<m_centerDrptThresh) return false;
    return true;
  }

  int JetForwardPFlowJvtTool::getJetVertex(const xAOD::Jet *jet) const {
    std::vector<float> sumpts;
    jet->getAttribute<std::vector<float> >("SumPtTrkPt500",sumpts);
    double firstVal = 0;
    int bestMatch = -1;
    for (size_t i = 0; i < sumpts.size(); i++) {
      if (sumpts[i]>firstVal) {
        bestMatch = i;
        firstVal = sumpts[i];
      }
    }
    return bestMatch;
  }

  float JetForwardPFlowJvtTool::getDrpt(const xAOD::Jet *jet) const {
    std::vector<float> sumpts;
    jet->getAttribute<std::vector<float> >("SumPtTrkPt500",sumpts);
    if (sumpts.size()<2) return 0;

    std::nth_element(sumpts.begin(),sumpts.begin()+sumpts.size()/2,sumpts.end(),std::greater<int>());
    double median = sumpts[sumpts.size()/2];
    std::nth_element(sumpts.begin(),sumpts.begin(),sumpts.end(),std::greater<int>());
    double max = sumpts[0];
    return (max-median)/jet->pt();
  }

  void JetForwardPFlowJvtTool::getPV() const {
    const xAOD::VertexContainer *vxCont = 0;
    m_pvind = 0;
    if( evtStore()->retrieve(vxCont, m_verticesName).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve primary vertex container");
    } else if(vxCont->empty()) {
      ATH_MSG_WARNING("Event has no primary vertices!");
    } else {
      ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
      for(const auto& vx : *vxCont) {
        if(vx->vertexType()==xAOD::VxType::PriVtx)
          {m_pvind = vx->index(); break;}
      }
    }
  }

  StatusCode JetForwardPFlowJvtTool::tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets) {
    for(const auto& jet : *jets) {
      bool ishs = false;
      bool ispu = true;
      for(const auto& tjet : *truthJets) {
        if (tjet->p4().DeltaR(jet->p4())<0.3 && tjet->pt()>10e3) ishs = true;
        if (tjet->p4().DeltaR(jet->p4())<0.6) ispu = false;
      }
      isHS(*jet)=ishs;
      isPU(*jet)=ispu;
    }
    return StatusCode::SUCCESS;
  }

