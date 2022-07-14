///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// JetBalancePFlowJvtTool.cxx
// Implementation file for class JetBalancePFlowJvtTool
// Author: Eimear Conroy <eimear.isobel.conroy@cern.ch>
// Inherits from JetForwardPFlowJvtTool - Author: Anastasia Kotsokechagia <anastasia.kotsokechagia@cern.ch>
///////////////////////////////////////////////////////////////////

// JetBalancePFlowJvtTool includes
#include "JetMomentTools/JetBalancePFlowJvtTool.h"

// Jet EDM
#include "xAODJet/JetAttributes.h"

// FastJet
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include <fastjet/AreaDefinition.hh>

// Jet
#include "JetRec/JetFromPseudojet.h"

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  JetBalancePFlowJvtTool::JetBalancePFlowJvtTool(const std::string& name) : 
  JetForwardPFlowJvtTool(name){
    m_bjvtThresh = 0;
  }

 // Destructor
  ///////////////
  JetBalancePFlowJvtTool::~JetBalancePFlowJvtTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode JetBalancePFlowJvtTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    //WPs have not been calculated yet. Using holdover fJVT values here for now
    if (m_tightOP) m_fjvtThresh = 0.53; //Closer to 1 ==> more likely to be PU
    else m_fjvtThresh = 0.72;

    ATH_CHECK( m_tvaKey.initialize() );

    if(m_jetContainerName.empty()){
      ATH_MSG_ERROR("JetBalancePFlowJvtTool needs to have its input jet container configured!");
      return StatusCode::FAILURE;
    }

    if(!m_FEKey.empty()){
      if(!m_orFEKey.key().empty()){
        m_orFEKey = m_jetContainerName + "." + m_orFEKey.key();
      }
    }
    else{
      ATH_MSG_ERROR("Flow Element container is empty");
    }

    ATH_CHECK(m_FEKey.initialize( !m_FEKey.empty() ));
    ATH_CHECK(m_orFEKey.initialize( !m_FEKey.empty() && !m_orFEKey.key().empty() ));

    m_bjvtKey = m_jetContainerName + "." + m_bjvtKey.key();
    m_bjvtRawKey = m_jetContainerName + "." + m_bjvtRawKey.key();
    m_isHSKey = m_jetContainerName + "." + m_isHSKey.key();
    m_isQCDPUKey = m_jetContainerName + "." + m_isQCDPUKey.key();
    m_isStochPUKey = m_jetContainerName + "." + m_isStochPUKey.key();
    m_jvtKey = m_jetContainerName + "." + m_jvtKey.key();

    ATH_CHECK(m_bjvtKey.initialize());
    ATH_CHECK(m_bjvtRawKey.initialize());
    ATH_CHECK(m_isHSKey.initialize());
    ATH_CHECK(m_isQCDPUKey.initialize());
    ATH_CHECK(m_isStochPUKey.initialize());
    ATH_CHECK(m_jvtKey.initialize());

    ATH_CHECK(m_vxContKey.initialize());

    //These keys do nothing in this tool, but need to be initialised because of inheritance from JetForwardPFlowJvtTool
    //This is probably a very hacky way of getting around the issue
    m_orKey = m_jetContainerName + "." + m_orKey.key();;
    m_PFOKey = m_jetContainerName + "." + m_PFOKey.key();
    m_fjvtKey = m_jetContainerName + "." + m_fjvtKey.key();
    m_fjvtRawKey = m_jetContainerName + "." + m_fjvtRawKey.key();
    m_isPUKey = m_jetContainerName + "." + m_isPUKey.key();
    
    ATH_CHECK(m_orKey.initialize());
    ATH_CHECK(m_PFOKey.initialize());
    ATH_CHECK(m_fjvtKey.initialize());
    ATH_CHECK(m_fjvtRawKey.initialize());
    ATH_CHECK(m_isPUKey.initialize());
    
    return StatusCode::SUCCESS;
  }

  StatusCode JetBalancePFlowJvtTool::decorate(const xAOD::JetContainer& jetCont) const {
    std::vector<TVector2> pileupMomenta;
    pileupMomenta=calculateVertexMomenta(&jetCont,m_pvind, m_vertices);
    SG::WriteDecorHandle<xAOD::JetContainer, char> bjvtHandle(m_bjvtKey);
    SG::WriteDecorHandle<xAOD::JetContainer, float> bjvtRawHandle(m_bjvtRawKey);
    if(pileupMomenta.size()==0) {
      ATH_MSG_DEBUG( "pileupMomenta is empty, this can happen for events with no PU vertices."
                     <<" bJVT won't be computed for this event and will be set to 0 instead." );
      for(const xAOD::Jet* jetC : jetCont) {
        bjvtHandle(*jetC) = 1;
        bjvtRawHandle(*jetC) = 0;
      }
      return StatusCode::SUCCESS;
    }

    for(const xAOD::Jet* jetC : jetCont) {
      bjvtHandle(*jetC) = 1;
      bjvtRawHandle(*jetC) = 0;

      if (isCentralJet(jetC)){
        double bjvt = getFJVT(jetC,pileupMomenta); //Same projection function as fJVT
        if (bjvt>m_bjvtThresh) bjvtHandle(*jetC) = 0;
        bjvtRawHandle(*jetC) = bjvt;
      }
    }
    return StatusCode::SUCCESS;
  }

  //Redefined here: Using isQCDPUJet()
  std::vector<TVector2> JetBalancePFlowJvtTool::calculateVertexMomenta(const xAOD::JetContainer *pjets,
                                                                       int pvind, int vertices) const {
    std::vector<TVector2> pileupMomenta;
    // -- Retrieve PV index if not provided by user
    const std::size_t pv_index = (pvind==-1) ? getPV() : std::size_t(pvind);

    SG::ReadHandle<xAOD::VertexContainer> vxContHandle(m_vxContKey);

    for(const xAOD::Vertex* vx: *vxContHandle) {
      if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
      if(vx->index()==(size_t)pv_index) continue;

      TString jname = m_jetsName.value();
      jname += vx->index();

      pflow::puJets vertjets = buildPFlowPUjets(*vx);
      if( !vertjets.jetCont || !vertjets.jetAuxCont ){
        ATH_MSG_WARNING(" Some issue appeared while building the pflow pileup jets for vertex "
                        << vx->index() << " (vxType = " << vx->vertexType()<<" )!" );
        return pileupMomenta;
      }
      
      TVector2 vertex_met;
      for( const xAOD::Jet *jet : *(vertjets.jetCont) ) {

        // Remove jets which are close to hs
        if (!m_includePV && hasCloseByHSjet(jet,pjets)) continue;

        // Calculate vertex missing momentum
        if (isQCDPUJet(jet) && getRpt(jet)> m_rptCut)
        { 
          vertex_met += TVector2(jet->pt()*cos(jet->phi()),jet->pt()*sin(jet->phi()) ) ;
        } 
        else{
          vertex_met += TVector2(jet->jetP4(m_jetchargedp4).Pt()*cos(jet->jetP4(m_jetchargedp4).Phi()),
                                 jet->jetP4(m_jetchargedp4).Pt()*sin(jet->jetP4(m_jetchargedp4).Phi()) );
        }
      }

      pileupMomenta.push_back(vertex_met);
      if(vertices!=-1 && int(vx->index())==vertices) break;
    }
    return pileupMomenta;
  }

  bool JetBalancePFlowJvtTool::isQCDPUJet(const xAOD::Jet *jet) const {
    if (std::abs(jet->eta())>m_etaThresh) return false; //Must be central
    if (jet->pt()<m_QCDPUMinPt || (m_QCDPUMaxPt>0 && jet->pt()>m_QCDPUMaxPt) ) return false;
    //Other selections? EMPTopo studies included cuts on JVT and JVF
    return true;
  }

  //Redefined here: Separately label QCD and stochastic PU
  StatusCode JetBalancePFlowJvtTool::tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets,
                                              const xAOD::JetContainer *itpujets,const xAOD::JetContainer *ootpujets) {
    //truthJets container taken to be truth HS - @to-Do double check this
    //In-time and out-of-time PU truth jets given in AODs as separate containers
    SG::WriteDecorHandle<xAOD::JetContainer, bool> isHSHandle(m_isHSKey);
    SG::WriteDecorHandle<xAOD::JetContainer, bool> isQCDPUHandle(m_isQCDPUKey);
    SG::WriteDecorHandle<xAOD::JetContainer, bool> isStochPUHandle(m_isStochPUKey);
    
    for(const xAOD::Jet *jet : *jets) {
      bool ishs = false;
      bool ispu = true;

      bool isqcdpu = false;
      bool isstochpu = true; //Default value to make logic work - changed below
      
      for(const xAOD::Jet *tjet : *truthJets) {
        if (tjet->p4().DeltaR(jet->p4())<0.3 && tjet->pt()>10e3) ishs = true;
        if (tjet->p4().DeltaR(jet->p4())<0.6 && tjet->pt()>4e3)  ispu = false;
      }
      
      if (ispu == true){
        //Need to check both containers
        for(const xAOD::Jet *itpujet : *itpujets){
          if (itpujet->p4().DeltaR(jet->p4())<0.3 && itpujet->pt()>10e3) isqcdpu = true;
          if (itpujet->p4().DeltaR(jet->p4())<0.6 && itpujet->pt()>10e3) isstochpu = false;
        }
        for(const xAOD::Jet *ootpujet : *ootpujets){
          if (ootpujet->p4().DeltaR(jet->p4())<0.3 && ootpujet->pt()>10e3) isqcdpu = true;
          if (ootpujet->p4().DeltaR(jet->p4())<0.6 && ootpujet->pt()>10e3) isstochpu = false;
        }
      }

      else {
        isstochpu = false; //Can't be labelled stochastic PU if jet is not determined to be PU
      }

      isHSHandle(*jet)=ishs;
      isQCDPUHandle(*jet)=isqcdpu;
      isStochPUHandle(*jet)=isstochpu;
    }
  
  return StatusCode::SUCCESS;
  }