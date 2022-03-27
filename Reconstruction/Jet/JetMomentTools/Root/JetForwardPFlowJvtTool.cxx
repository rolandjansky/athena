///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "JetRec/JetFromPseudojet.h"

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  JetForwardPFlowJvtTool::JetForwardPFlowJvtTool(const std::string& name) :
    AsgTool(name) {
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
    if (m_tightOP) m_fjvtThresh = 0.53;
    else m_fjvtThresh = 0.72;

    ATH_CHECK( m_tvaKey.initialize() );

    if(m_jetContainerName.empty()){
      ATH_MSG_ERROR("JetForwardPFlowJvtTool needs to have its input jet container configured!");
      return StatusCode::FAILURE;
    }

    //FlowElement jets instead of PFO jets
    if(!m_FEKey.empty()){
      if(!m_orFEKey.key().empty()){
        m_orFEKey = m_jetContainerName + "." + m_orFEKey.key();
      }
    }
    else{ //PFO reconstruction
      if(!m_orKey.key().empty()){
        m_orKey = m_jetContainerName + "." + m_orKey.key();
      }
    }
    ATH_CHECK(m_PFOKey.initialize( m_FEKey.empty() ));
    ATH_CHECK(m_orKey.initialize( m_FEKey.empty() && !m_orKey.key().empty() ));
    ATH_CHECK(m_FEKey.initialize( !m_FEKey.empty() ));
    ATH_CHECK(m_orFEKey.initialize( !m_FEKey.empty() && !m_orFEKey.key().empty() ));

    m_fjvtKey = m_jetContainerName + "." + m_fjvtKey.key();
    m_fjvtRawKey = m_jetContainerName + "." + m_fjvtRawKey.key();
    m_isHSKey = m_jetContainerName + "." + m_isHSKey.key();
    m_isPUKey = m_jetContainerName + "." + m_isPUKey.key();
    m_jvtKey = m_jetContainerName + "." + m_jvtKey.key();

    ATH_CHECK(m_fjvtKey.initialize());
    ATH_CHECK(m_fjvtRawKey.initialize());
    ATH_CHECK(m_isHSKey.initialize());
    ATH_CHECK(m_isPUKey.initialize());
    ATH_CHECK(m_jvtKey.initialize());

    ATH_CHECK(m_vxContKey.initialize());
    
    return StatusCode::SUCCESS;
  }

  StatusCode JetForwardPFlowJvtTool::decorate(const xAOD::JetContainer& jetCont) const {
    std::vector<TVector2> pileupMomenta;

    pileupMomenta=calculateVertexMomenta(&jetCont,m_pvind, m_vertices);

    SG::WriteDecorHandle<xAOD::JetContainer, char> fjvtHandle(m_fjvtKey);
    SG::WriteDecorHandle<xAOD::JetContainer, float> fjvtRawHandle(m_fjvtRawKey);
    if(pileupMomenta.size()==0) {
      ATH_MSG_DEBUG( "pileupMomenta is empty, this can happen for events with no PU vertices."
                     <<" fJVT won't be computed for this event and will be set to 0 instead." );
      for(const xAOD::Jet* jetF : jetCont) {
	fjvtHandle(*jetF) = 1;
	fjvtRawHandle(*jetF) = 0;
      }
      return StatusCode::SUCCESS;
    }

    for(const xAOD::Jet* jetF : jetCont) {
      fjvtHandle(*jetF) = 1;
      fjvtRawHandle(*jetF) = 0;

      if (isForwardJet(jetF)){
       double fjvt = getFJVT(jetF,pileupMomenta);
       if (fjvt>m_fjvtThresh) fjvtHandle(*jetF) = 0;
       fjvtRawHandle(*jetF) = fjvt;
      }
    }
    return StatusCode::SUCCESS;
  }

  float JetForwardPFlowJvtTool::getFJVT(const xAOD::Jet *jet, std::vector<TVector2> pileupMomenta) const {
    TVector2 fjet(jet->pt()*cos(jet->phi()),jet->pt()*sin(jet->phi()));
    double fjvt = 0;
    for (const TVector2& pu : pileupMomenta) {
      double projection = pu*fjet/fjet.Mod();
      if (projection<fjvt) fjvt = projection;
    }
    return -1*fjvt/fjet.Mod();
  }

  std::vector<TVector2> JetForwardPFlowJvtTool::calculateVertexMomenta(const xAOD::JetContainer *pjets,
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
        if (isCentralJet(jet) && getRpt(jet)> m_rptCut)
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

  bool JetForwardPFlowJvtTool::hasCloseByHSjet(const xAOD::Jet *jet, const xAOD::JetContainer *pjets ) const {
    for (const xAOD::Jet* pjet : *pjets) {
      float jet_jvt=0;
      SG::ReadDecorHandle<xAOD::JetContainer, float> jvtHandle(m_jvtKey);
      jet_jvt = jvtHandle(*pjet);
     if (pjet->p4().DeltaR(jet->p4())<0.3 && jet_jvt>m_jvtCut && isCentralJet(pjet) ) return true;
    }
    return false;
  }

  pflow::puJets JetForwardPFlowJvtTool::buildPFlowPUjets(const xAOD::Vertex &vx) const {
    pflow::puJets pu_jets;
    const std::size_t pv_index = (m_pvind==-1) ? getPV() : std::size_t (m_pvind);

    std::vector<fastjet::PseudoJet> input_pfo;
    std::set<int> charged_pfo;

    SG::ReadHandle<jet::TrackVertexAssociation> tvaHandle(m_tvaKey);
        
    if (!tvaHandle.isValid()){
      ATH_MSG_ERROR("Could not retrieve the TrackVertexAssociation: "
                    << m_tvaKey.key());
      return pu_jets;
    }

    if(!m_FEKey.empty()){
      SG::ReadHandle<xAOD::FlowElementContainer> FlowElementHandle(m_FEKey);

      for(const xAOD::FlowElement* fe : *FlowElementHandle){
        if (!m_orFEKey.key().empty()){
          SG::ReadDecorHandle<xAOD::FlowElement, char> orHandle(m_orFEKey);
          if (!orHandle(*fe)) continue;
        }
        if (fe->isCharged()) {
          const xAOD::TrackParticle* track = dynamic_cast<const xAOD::TrackParticle*>(fe->chargedObject(0));

          if (vx.index()==pv_index && std::abs((vx.z()-track->z0())*sin(track->theta()))>m_dzCut)
            continue;
          if (vx.index()!=pv_index
              && (!tvaHandle->associatedVertex(track)
                  || vx.index()!=tvaHandle->associatedVertex(track)->index())
              ) continue;
          input_pfo.push_back(feToPseudoJet(fe, CP::charged, &vx) );
          charged_pfo.insert(fe->index());
        }
        else if (std::abs(fe->eta())<m_neutMaxRap && !fe->isCharged() && fe->e()>0)
          {
            input_pfo.push_back(feToPseudoJet(fe, CP::neutral, &vx) );
          }
      }
    }
    else{
      SG::ReadHandle<xAOD::PFOContainer> PFOHandle(m_PFOKey);

      for(const xAOD::PFO* pfo : *PFOHandle){
        if (!m_orKey.key().empty()){
          SG::ReadDecorHandle<xAOD::PFO, char> orHandle(m_orKey);
          if (!orHandle(*pfo)) continue;
        }
        if (pfo->isCharged()) {
          if (vx.index()==pv_index && std::abs((vx.z()-pfo->track(0)->z0())*sin(pfo->track(0)->theta()))>m_dzCut)
            continue;
          if (vx.index()!=pv_index
              && (!tvaHandle->associatedVertex(pfo->track(0))
                  || vx.index()!=tvaHandle->associatedVertex(pfo->track(0))->index())
              ) continue;
          input_pfo.push_back(pfoToPseudoJet(pfo, CP::charged, &vx) );
          charged_pfo.insert(pfo->index());
        }
        else if (std::abs(pfo->eta())<m_neutMaxRap && !pfo->isCharged() && pfo->eEM()>0)
        {
          input_pfo.push_back(pfoToPseudoJet(pfo, CP::neutral, &vx) );
        }
      }
    }

    std::shared_ptr<xAOD::JetContainer> vertjets = std::make_shared<xAOD::JetContainer>();
    std::shared_ptr<xAOD::JetAuxContainer> vertjetsAux = std::make_shared<xAOD::JetAuxContainer>();

    vertjets->setStore(vertjetsAux.get());
    TString newname = m_jetsName.value();
    newname += vx.index();

    fastjet::JetDefinition jet_def(fastjet::antikt_algorithm,0.4);
    fastjet::AreaDefinition area_def(fastjet::active_area_explicit_ghosts,
                                     fastjet::GhostedAreaSpec(fastjet::SelectorAbsRapMax(m_maxRap)));
    fastjet::ClusterSequenceArea clust_pfo(input_pfo,jet_def,area_def);
    std::vector<fastjet::PseudoJet> inclusive_jets = sorted_by_pt(clust_pfo.inclusive_jets(5000.));

    for (size_t i = 0; i < inclusive_jets.size(); i++) {
      xAOD::Jet* jet=  new xAOD::Jet();
      xAOD::JetFourMom_t tempjetp4(inclusive_jets[i].pt(),
                                   inclusive_jets[i].eta(),
                                   inclusive_jets[i].phi(),
                                   inclusive_jets[i].m());
      xAOD::JetFourMom_t newArea(inclusive_jets[i].area_4vector().perp(),
                                 inclusive_jets[i].area_4vector().eta(),
                                 inclusive_jets[i].area_4vector().phi(),
                                 inclusive_jets[i].area_4vector().m());
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
      xAOD::JetFourMom_t chargejetp4(chargedpart,inclusive_jets[i].eta(),inclusive_jets[i].phi(),inclusive_jets[i].m());
      jet->setJetP4(m_jetchargedp4,chargejetp4);
    }

    if((m_pfoJES->modify(*vertjets)).isFailure()){
      ATH_MSG_ERROR(" Failed to calibrate PU jet container ");
      return pu_jets;
    }

    pu_jets.jetCont = vertjets;
    pu_jets.jetAuxCont = vertjetsAux;
    return pu_jets;
  }

  fastjet::PseudoJet JetForwardPFlowJvtTool::pfoToPseudoJet(const xAOD::PFO* pfo, const CP::PFO_JetMETConfig_charge& theCharge, const xAOD::Vertex *vx) const {
    TLorentzVector pfo_p4;
    if (CP::charged == theCharge){
      float pweight = m_weight;
      if( (m_wpfotool->fillWeight(*pfo,pweight)).isSuccess() ){
	// Create a PSeudojet with the momentum of the selected IParticle
	pfo_p4= TLorentzVector(pfo->p4().Px()*pweight,pfo->p4().Py()*pweight,pfo->p4().Pz()*pweight,pfo->e()*pweight);
      }
    } else if (CP::neutral == theCharge){ 
      pfo_p4= pfo->GetVertexCorrectedEMFourVec(*vx);
    }
    fastjet::PseudoJet psj(pfo_p4);
    // User index is used to identify the xAOD object used for the PSeudoJet
    if (CP::charged == theCharge){
      psj.set_user_index(pfo->index());
    }else{
      psj.set_user_index(-1);
    }

    return psj;
  }

  fastjet::PseudoJet JetForwardPFlowJvtTool::feToPseudoJet(const xAOD::FlowElement* fe, const CP::PFO_JetMETConfig_charge& theCharge, const xAOD::Vertex *vx) const {
    TLorentzVector fe_p4;
    if (CP::charged == theCharge){
      float pweight = m_weight;
      if( (m_wpfotool->fillWeight(*fe,pweight)).isSuccess() ){
	// Create a Peeudojet with the momentum of the selected IParticle
	fe_p4= TLorentzVector(fe->p4().Px()*pweight,fe->p4().Py()*pweight,fe->p4().Pz()*pweight,fe->e()*pweight);
      }
    } else if (CP::neutral == theCharge){
      fe_p4=FEHelpers::getVertexCorrectedFourVec(*fe, *vx);
    }
    fastjet::PseudoJet psj(fe_p4);
    // User index is used to identify the xAOD object used for the PseudoJet
    if (CP::charged == theCharge){
      psj.set_user_index(fe->index());
    }else{
      psj.set_user_index(-1);
    }

    return psj;
  }

  bool JetForwardPFlowJvtTool::isForwardJet(const xAOD::Jet *jet) const {
    if (std::abs(jet->eta())<m_etaThresh) return false;
    if (jet->pt()<m_forwardMinPt || (m_forwardMaxPt>0 && jet->pt()>m_forwardMaxPt) ) return false;
    return true;
  }

  bool JetForwardPFlowJvtTool::isCentralJet(const xAOD::Jet *jet) const {
    if (std::abs(jet->eta())>m_etaThresh) return false;
    if (jet->pt()<m_centerMinPt || (m_centerMaxPt>0 && jet->pt()>m_centerMaxPt)) return false;
    return true;
  }

  double JetForwardPFlowJvtTool::getRpt(const xAOD::Jet *jet) const {
    double Rpt;
    Rpt= jet->jetP4(m_jetchargedp4).Pt()/ jet->pt();
    return Rpt;
  }

  std::size_t JetForwardPFlowJvtTool::getPV() const{
    if (m_includePV) return -1;

    //const xAOD::VertexContainer *vxCont = 0;
    SG::ReadHandle<xAOD::VertexContainer> vxContHandle(m_vxContKey);
    ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
    for(const xAOD::Vertex *vx : *vxContHandle) {
      if(vx->vertexType()==xAOD::VxType::PriVtx) return vx->index();
    } 
    // If no verticies are found in the event the Primary Vertex container will just contain a dummy vertex and no primary vertex   
    if(vxContHandle->size() == 0 ){     
      ATH_MSG_ERROR("Primary vertex container is empty ");
    } else if(vxContHandle->size() != 1 ){     
      ATH_MSG_WARNING("Couldn't identify the hard-scatter primary vertex (no vertex with \"vx->vertexType()==xAOD::VxType::PriVtx\" in the container)! ");
    }
    // this almost certainly isn't what we should do here, the
    // caller doesn't check this for errors
    return 0;
  }

  StatusCode JetForwardPFlowJvtTool::tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets) {
    SG::WriteDecorHandle<xAOD::JetContainer, bool> isHSHandle(m_isHSKey);
    SG::WriteDecorHandle<xAOD::JetContainer, bool> isPUHandle(m_isPUKey);
    
    for(const xAOD::Jet *jet : *jets) {
      bool ishs = false;
      bool ispu = true;
      for(const xAOD::Jet *tjet : *truthJets) {
        if (tjet->p4().DeltaR(jet->p4())<0.3 && tjet->pt()>10e3) ishs = true;
        if (tjet->p4().DeltaR(jet->p4())<0.6) ispu = false;
      }
      isHSHandle(*jet)=ishs;
      isPUHandle(*jet)=ispu;
    }
    return StatusCode::SUCCESS;
  }

