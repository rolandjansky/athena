///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

// JetForwardJvtTool.cxx
// Implementation file for class JetForwardJvtTool
// Author: Matt Klein<matthew.henry.klein@cern.ch>
///////////////////////////////////////////////////////////////////

// JetForwardJvtTool includes
#include "JetMomentTools/JetForwardJvtTool.h"

// Jet EDM
#include "xAODJet/JetAttributes.h"

// Shallow copy
//#include "xAODCore/ShallowCopy.h"

    static SG::AuxElement::Decorator<char>  isHS("isJvtHS");
    static SG::AuxElement::Decorator<char>  isPU("isJvtPU");

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  JetForwardJvtTool::JetForwardJvtTool(const std::string& name) :
    AsgTool(name),
    m_fjvtThresh(15e3)
  {
    declareProperty("OverlapDec",          m_orLabel            = ""                );
    declareProperty("OutputDec",           m_outLabel           = "passFJVT"        );
    declareProperty("EtaThresh",        m_etaThresh          = 2.5              );
    declareProperty("ForwardMinPt",        m_forwardMinPt          = 20e3              );
    declareProperty("ForwardMaxPt",        m_forwardMaxPt          = 50e3              );
    declareProperty("CentralMinPt",        m_centerMinPt          = 20e3              );
    declareProperty("CentralMaxPt",        m_centerMaxPt          = -1              );
    declareProperty("CentralJvtThresh",        m_centerJvtThresh          = 0.14              );
    declareProperty("JvtMomentName",   m_jvtMomentName   = "Jvt"               );
    declareProperty("CentralDrptThresh",       m_centerDrptThresh          = 0.2              );
    declareProperty("CentralMaxStochPt",          m_maxStochPt         = 35e3              );
    declareProperty("JetScaleFactor",          m_jetScaleFactor         = 0.4              );
    //declareProperty("FJVTThreshold",          m_fjvtThresh         = 15e3              );//15GeV->92%,11GeV->85%
    declareProperty("UseTightOP",          m_tightOP         = false              );//Tight or Loose

    declareProperty("VertexContainer", m_vertexContainer_key="PrimaryVertices");
    declareProperty("Met_Track", m_trkMET_key="Met_Track");

  }

  // Destructor
  ///////////////
  JetForwardJvtTool::~JetForwardJvtTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode JetForwardJvtTool::initialize()
  {
    ATH_MSG_DEBUG("initializing version with data handles");
    ATH_MSG_INFO ("Initializing " << name() << "...");
    if (m_tightOP) m_fjvtThresh = 0.4;
    else m_fjvtThresh = 0.5;
    if (m_orLabel!="")  m_Dec_OR = new SG::AuxElement::Decorator<char>(m_orLabel);
    m_Dec_out = new SG::AuxElement::Decorator<char>(m_outLabel);

    ATH_CHECK(m_vertexContainer_key.initialize());
    ATH_CHECK(m_trkMET_key.initialize());

    return StatusCode::SUCCESS;
  }

  StatusCode JetForwardJvtTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  int JetForwardJvtTool::modify(xAOD::JetContainer& jetCont) const {
    getPV();
    m_pileupMomenta.clear();
    for(const auto& jetF : jetCont) {
      (*m_Dec_out)(*jetF) = 1;
      if (!forwardJet(jetF)) continue;
      if (m_pileupMomenta.size()==0) calculateVertexMomenta(&jetCont);
      double fjvt = getFJVT(jetF)/jetF->pt();
      if (fjvt>m_fjvtThresh) (*m_Dec_out)(*jetF) = 0;
    }
    return 0;
  }

  float JetForwardJvtTool::getFJVT(const xAOD::Jet *jet) const {
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

  void JetForwardJvtTool::calculateVertexMomenta(const xAOD::JetContainer *jets) const {
    m_pileupMomenta.clear();

    auto trkMETContainer = SG::makeHandle (m_trkMET_key);
    if (!trkMETContainer.isValid()){
      ATH_MSG_WARNING("Invalid  xAOD::MissingETContainer datahandle");
      return;
    }
    auto trkMet = trkMETContainer.cptr();

    auto vertexContainer = SG::makeHandle (m_vertexContainer_key);
    if (!vertexContainer.isValid()){
      ATH_MSG_WARNING("Invalid  xAOD::VertexContainer datahandle");
      return;
    }
    auto vxCont = vertexContainer.cptr();

    for(const auto& vx : *vxCont) {
      if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
      TString vname = "PVTrack_vx";
      vname += vx->index();
      m_pileupMomenta.push_back(\
                                (vx->index()==m_pvind ? \
                                 0:\
                                 -(1./m_jetScaleFactor))*TVector2(0.5*(*trkMet)[vname.Data()]->mpx(),0.5*(*trkMet)[vname.Data()]->mpy()));
    }

    for (const auto& jet : *jets) {
      if (!centralJet(jet)) continue;
      int jetvert = getJetVertex(jet);
      if (jetvert>=0) m_pileupMomenta[jetvert] += TVector2(0.5*jet->pt()*cos(jet->phi()),0.5*jet->pt()*sin(jet->phi())); 
    }
  }

  float JetForwardJvtTool::getCombinedWidth(const xAOD::Jet *jet) const {
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

  bool JetForwardJvtTool::forwardJet(const xAOD::Jet *jet) const {
    if (fabs(jet->eta())<m_etaThresh) return false;
    if (jet->pt()<m_forwardMinPt || jet->pt()>m_forwardMaxPt) return false;
    return true;
  }

  bool JetForwardJvtTool::centralJet(const xAOD::Jet *jet) const {
    if (fabs(jet->eta())>m_etaThresh) return false;
    if (jet->pt()<m_centerMinPt || (m_centerMaxPt>0 && jet->pt()>m_centerMaxPt)) return false;
    if (m_Dec_OR && !(*m_Dec_OR)(*jet)) return false;
    float jvt = 0;
    jet->getAttribute<float>(m_jvtMomentName,jvt);
    if (jvt>m_centerJvtThresh) return false;
    if (jet->pt()<m_maxStochPt && getDrpt(jet)<m_centerDrptThresh) return false;
    return true;
  }

  int JetForwardJvtTool::getJetVertex(const xAOD::Jet *jet) const {
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

  float JetForwardJvtTool::getDrpt(const xAOD::Jet *jet) const {
    std::vector<float> sumpts;
    jet->getAttribute<std::vector<float> >("SumPtTrkPt500",sumpts);
    if (sumpts.size()<2) return 0;

    std::nth_element(sumpts.begin(),sumpts.begin()+sumpts.size()/2,sumpts.end(),std::greater<int>());
    double median = sumpts[sumpts.size()/2];
    std::nth_element(sumpts.begin(),sumpts.begin(),sumpts.end(),std::greater<int>());
    double max = sumpts[0];
    return (max-median)/jet->pt();
  }

  void JetForwardJvtTool::getPV() const {

    auto vertexContainer = SG::makeHandle (m_vertexContainer_key);
    if (!vertexContainer.isValid()){
      ATH_MSG_WARNING("Invalid  xAOD::VertexContainer datahandle");
      return;
    }
    auto vxCont = vertexContainer.cptr();

    m_pvind = 0;
    if(vxCont->empty()) {
      ATH_MSG_WARNING("Event has no primary vertices!");
    } else {
      ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
      for(const auto& vx : *vxCont) {
        if(vx->vertexType()==xAOD::VxType::PriVtx)
          {m_pvind = vx->index(); break;}
      }
    }
  }

  StatusCode JetForwardJvtTool::tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets) {
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

