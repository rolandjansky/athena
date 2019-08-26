///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetForwardPFlowJvtTool.cxx
// Implementation file for class JetForwardPFlowJvtTool
// Author: Anastasia Kotsokechagia <anastasia.kotsokechagia@cern.ch>
///////////////////////////////////////////////////////////////////

// JetForwardPFlowJvtTool includes
#include "JetMomentTools/JetForwardPFlowJvtTool.h"

// Jet EDM
#include "xAODJet/JetAttributes.h"

// FastJet
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include <fastjet/AreaDefinition.hh>

// Jet
#include "xAODJet/JetAuxContainer.h"
#include "JetRec/JetFromPseudojet.h"

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
    declareProperty("OverlapDec",         m_orLabel          = ""                        );
    declareProperty("JetContainerName",   m_jetsName         = "AntiKt4PFlowJets"        );
    declareProperty("OutputDecFjvt",      m_outLabelFjvt     = "passOnlyFJVT"            );
    declareProperty("VertexContainer",    m_verticesName     = "PrimaryVertices"         );
    declareProperty("JetChargedp4",       m_jetchargedp4     = "JetChargedScaleMomentum" );
    declareProperty("EtaThresh",          m_etaThresh        = 2.5                       );
    declareProperty("ForwardMinPt",       m_forwardMinPt     = 20e3                      );
    declareProperty("ForwardMaxPt",       m_forwardMaxPt     = 60e3                      );
    declareProperty("CentralMinPt",       m_centerMinPt      = 20e3                      );
    declareProperty("CentralMaxPt",       m_centerMaxPt      = -1                        );
    declareProperty("CentralJvtThresh",   m_centerJvtThresh  = 0.2                       );
    declareProperty("UseTightOP",         m_tightOP          = false                     );//Tight or Loose
    declareProperty("PVIndexHS",          m_pvind            = -1                        );
    declareProperty("RptThresValue",      m_rptCut           = 0.1                       );
    declareProperty("JVTThresValue",      m_jvtCut           = 0.2                       );
    declareProperty("DzThresValue",       m_dzCut            = 2.                        );
    declareProperty("JetsFromVertices",   m_vertices         = 10                        );
    declareProperty("JetBuildingMaxRap",  m_maxRap           = 2.5                       );
    declareProperty("NeutralMaxRap",      m_neutMaxRap       = 2.5                       );
    declareProperty("PFOToolName",        m_pfoToolName      = "PFOTool"                 );
    declareProperty("PFOWeightToolName",  m_wpfoToolName     = "WPFOTool"                );
    declareProperty("JetCalibToolName",   m_pfoJESName       = "pfoJES"                  );
    declareProperty("JetCalibCollection", m_jetAlgo          = "AntiKt4EMPFlow"          );
    declareProperty("JetCalibConfigFile", m_caliconfig       = "JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config");
    declareProperty("JetCalibSequence",   m_calibSeq         = "JetArea_Residual_EtaJES" );
    declareProperty("JetCalibArea",       m_calibArea        = "00-04-82"                );
    declareProperty("JetCalibIsData",     m_isdata           = false                     );
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
    Dec_outFjvt = std::make_unique<SG::AuxElement::Decorator<char> >(m_outLabelFjvt);

    m_pfotool = new CP::RetrievePFOTool(m_pfoToolName);
    m_pfotool->initialize();
    m_wpfotool = new CP::WeightPFOTool(m_wpfoToolName);
    m_wpfotool->initialize();

    m_pfoJES = new JetCalibrationTool(m_pfoJESName);
    m_pfoJES->setProperty("JetCollection",m_jetAlgo);
    m_pfoJES->setProperty("ConfigFile",m_caliconfig);
    m_pfoJES->setProperty("CalibSequence",m_calibSeq);
    m_pfoJES->setProperty("CalibArea",m_calibArea);
    m_pfoJES->setProperty("IsData",m_isdata);
    m_pfoJES->initializeTool(m_pfoJESName);
    
    return StatusCode::SUCCESS;
  }

  StatusCode JetForwardPFlowJvtTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  int JetForwardPFlowJvtTool::modify(xAOD::JetContainer& jetCont) const {

    m_pileupMomenta.clear();
    for(const auto& jetF : jetCont) {
      (*Dec_outFjvt)(*jetF) = 1;
      fjvt_dec(*jetF) = 0;
      if (!forwardJet(jetF)) continue;
      if (m_pileupMomenta.size()==0) calculateVertexMomenta(&jetCont,m_pvind, m_vertices);
      double fjvt = getFJVT(jetF)/jetF->pt();
      if (fjvt>m_fjvtThresh) (*Dec_outFjvt)(*jetF) = 0;
      fjvt_dec(*jetF) = fjvt;
    }
    return 0;
  }

  float JetForwardPFlowJvtTool::getFJVT(const xAOD::Jet *jet) const {
    TVector2 fjet(-jet->pt()*cos(jet->phi()),-jet->pt()*sin(jet->phi()));
    double fjvt = 0;
    for (size_t pui = 0; pui < m_pileupMomenta.size(); pui++) {
      //if (pui==pvind) continue;
      double projection = m_pileupMomenta[pui]*fjet/fjet.Mod();
      if (projection>fjvt) fjvt = projection;
    }
    //fjvt += getCombinedWidth(jet);
    return fjvt;
  }

  void JetForwardPFlowJvtTool::calculateVertexMomenta(const xAOD::JetContainer *pjets, int m_pvind, int m_vertices) const {
    m_pileupMomenta.clear();

    // -- Retrieve PV index if not provided by user
    int pvind = (m_pvind==-1) ? getPV() : m_pvind;

    const xAOD::VertexContainer *vxCont = 0;
    if( evtStore()->retrieve(vxCont, m_verticesName).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve primary vertex container \"" << m_verticesName << "\"");
    }
    const xAOD::PFOContainer *pfos = m_pfotool->retrievePFO(CP::EM,CP::all);
    if(pfos == NULL ){
      ATH_MSG_ERROR("PFO container is empty!");    
    } else {
      ATH_MSG_DEBUG("Successfully retrieved PFO objects");
    }

    for(const xAOD::Vertex* vx: *vxCont) {
      if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
      if(vx->index()==pvind) continue;
      // Build and retrieve PU jets
      buildPFlowPUjets(*vx,*pfos);
      TString jname = m_jetsName;
      jname += vx->index();
      const xAOD::JetContainer* vertex_jets  = nullptr;
      evtStore()->retrieve(vertex_jets,jname.Data());

      TVector2 vertex_met;
      for (const auto& jet : *vertex_jets) {

        // Remove jets which are close to hs
        if (hasCloseByHSjet(jet,pjets)) continue;

        // Calculate vertex missing momentum
        if (centralJet(jet) && getRpt(jet)> m_rptCut)
        {
          vertex_met += TVector2(jet->pt()*cos(jet->phi()),jet->pt()*sin(jet->phi()) ) ;
        } else{
          vertex_met += TVector2(jet->jetP4(m_jetchargedp4).Pt()*cos(jet->jetP4(m_jetchargedp4).Phi()), jet->jetP4(m_jetchargedp4).Pt()*sin(jet->jetP4(m_jetchargedp4).Phi()) );
        }
      }
      m_pileupMomenta.push_back(-1*vertex_met);
      if(m_vertices!=-1 && vx->index()==m_vertices) break;
    }

  }

  bool JetForwardPFlowJvtTool::hasCloseByHSjet(const xAOD::Jet *jet, const xAOD::JetContainer *pjets ) const {
    for (const auto& pjet : *pjets) {
     if (pjet->p4().DeltaR(jet->p4())<0.3 && pjet->auxdata<float>("Jvt")>m_jvtCut && centralJet(pjet) ) return true;
    }
    return false;
  }

  void JetForwardPFlowJvtTool::buildPFlowPUjets(const xAOD::Vertex &vx, const xAOD::PFOContainer &pfos) const {

    std::vector<fastjet::PseudoJet> input_pfo;
    std::set<int> charged_pfo;
    for(const xAOD::PFO* pfo : pfos){ 
      if (pfo->charge()!=0) { 
        if (vx.index()==0 && fabs((vx.z()-pfo->track(0)->z0())*sin(pfo->track(0)->theta()))>m_dzCut) continue;
        if (vx.index()!=0 && &vx!=pfo->track(0)->vertex()) continue;
        float pweight = 0;
        m_wpfotool->fillWeight(*pfo,pweight);
        input_pfo.push_back(pfoToPseudoJet(pfo, CP::charged, &vx) );
        charged_pfo.insert(pfo->index());
      } 
      else if (fabs(pfo->eta())<m_neutMaxRap && pfo->charge()==0 && pfo->eEM()>0)
      { 
        input_pfo.push_back(pfoToPseudoJet(pfo, CP::neutral, &vx) );
      }
    }
    xAOD::JetContainer* vertjets = new xAOD::JetContainer();
    xAOD::JetAuxContainer* vertjetsAux = new xAOD::JetAuxContainer();
    vertjets->setStore(vertjetsAux);
    TString newname = m_jetsName;
    newname += vx.index();
    evtStore()->record(vertjets,newname.Data());
    evtStore()->record(vertjetsAux,(newname+"Aux.").Data());

    fastjet::JetDefinition jet_def(fastjet::antikt_algorithm,0.4);
    fastjet::AreaDefinition area_def(fastjet::active_area_explicit_ghosts,fastjet::GhostedAreaSpec(fastjet::SelectorAbsRapMax(m_maxRap)));
    fastjet::ClusterSequenceArea clust_pfo(input_pfo,jet_def,area_def);
    std::vector<fastjet::PseudoJet> inclusive_jets = sorted_by_pt(clust_pfo.inclusive_jets(0));

    for (size_t i = 0; i < inclusive_jets.size(); i++) {
      xAOD::Jet* jet = new xAOD::Jet();
      xAOD::JetFourMom_t tempjetp4(inclusive_jets[i].pt(),inclusive_jets[i].rap(),inclusive_jets[i].phi(),0);
      xAOD::JetFourMom_t newArea(inclusive_jets[i].area_4vector().perp(),inclusive_jets[i].area_4vector().rap(),inclusive_jets[i].area_4vector().phi(),0);
      vertjets->push_back(jet);
      jet->setJetP4(tempjetp4);
      jet->setJetP4("JetConstitScaleMomentum",tempjetp4);
      jet->setJetP4("JetPileupScaleMomentum",tempjetp4);
      jet->setAttribute("ActiveArea4vec",newArea);
      jet->setAttribute("DetectorEta",jet->eta());
      std::vector<fastjet::PseudoJet> constituents = inclusive_jets[i].constituents();
      float chargedpart = 0;
      for (size_t j = 0; j < constituents.size(); j++) {
        if (charged_pfo.count(constituents[j].user_index())>=1) {
          chargedpart += constituents[j].perp(); 
        }
      }
      xAOD::JetFourMom_t chargejetp4(1.*chargedpart,inclusive_jets[i].rap(),inclusive_jets[i].phi(),0);
      jet->setJetP4(m_jetchargedp4,chargejetp4);
    }   
    m_pfoJES->modify(*vertjets);
  }

  fastjet::PseudoJet JetForwardPFlowJvtTool::pfoToPseudoJet(const xAOD::PFO* pfo, const CP::PFO_JetMETConfig_charge& theCharge, const xAOD::Vertex *vx) const {
    if ( pfo == 0 ) {
      ATH_MSG_WARNING("Have NULL pointer to PFO");
    }

    TLorentzVector pfo_p4;
    if (CP::charged == theCharge){
      // Create a PSeudojet with the momentum of the selected IParticle
      pfo_p4= pfo->p4();
    } else if (CP::neutral == theCharge){ 
      pfo_p4= pfo->GetVertexCorrectedEMFourVec(*vx);
    }

    fastjet::PseudoJet psj(pfo_p4);

    // user index is used to identify the xAOD object used for the PSeudoJet
    psj.set_user_index(pfo->index());

    return psj;
  }

  bool JetForwardPFlowJvtTool::forwardJet(const xAOD::Jet *jet) const {
    if (fabs(jet->eta())<m_etaThresh) return false;
    if (jet->pt()<m_forwardMinPt || jet->pt()>m_forwardMaxPt) return false;
    return true;
  }

  bool JetForwardPFlowJvtTool::centralJet(const xAOD::Jet *jet) const {
    if (fabs(jet->eta())>m_etaThresh) return false;
    if (jet->pt()<m_centerMinPt || (m_centerMaxPt>0 && jet->pt()>m_centerMaxPt)) return false;
    return true;
  }

  double JetForwardPFlowJvtTool::getRpt(const xAOD::Jet *jet) const {
    double Rpt;
    Rpt= jet->jetP4(m_jetchargedp4).Pt()/ jet->pt();
    return Rpt;
  }

  int JetForwardPFlowJvtTool::getPV() const{

    const xAOD::VertexContainer *vxCont = 0;
    if( evtStore()->retrieve(vxCont, m_verticesName).isFailure() ) {
      ATH_MSG_ERROR("Unable to retrieve primary vertex container");
      return StatusCode::FAILURE;
    } else if(vxCont->empty()) {
      ATH_MSG_INFO("Event has no primary vertices!");
    } else {
      ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
      for(const xAOD::Vertex *vx : *vxCont) {
        if(vx->vertexType()==xAOD::VxType::PriVtx) return vx->index();
      }
    }
    ATH_MSG_ERROR("Couldn't identify the hard-scatter primary vertex (no vertex with \"vx->vertexType()==xAOD::VxType::PriVtx\" in the container)!");
    return StatusCode::FAILURE;
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

