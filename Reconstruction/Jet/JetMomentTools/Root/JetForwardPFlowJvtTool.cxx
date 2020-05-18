///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


    static SG::AuxElement::Decorator<char>  isHS("isJvtHS");
    static SG::AuxElement::Decorator<char>  isPU("isJvtPU");

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  JetForwardPFlowJvtTool::JetForwardPFlowJvtTool(const std::string& name) :
    AsgTool(name),
    m_fjvtThresh(15e3){
    declareProperty("orLabel",     m_orLabel          = ""                        );
    declareProperty("jetsName",    m_jetsName         = "AntiKt4PUPFlowJets"      ,   "Container name for the output reconstructed PU jets "                                                        );
    declareProperty("tightOP",     m_tightOP          = false                     ,   "If true a tight fjvt threshold value is applied"                                                             );
    declareProperty("outLabelFjvt",m_outLabelFjvt     = "passOnlyFJVT"            ,   "Decorator for passing fJVT threshold (tight or loose)"                                                       );
    declareProperty("outLabel"    ,m_outLabel         = "fJvt"                    ,   "Decorator for raw fJVT variable"                                                                             );
    declareProperty("verticesName",m_verticesName     = "PrimaryVertices"         ,   "Container name of vertices to be retrieved"                                                                  );
    declareProperty("includePV"   ,m_includePV        = false                     ,   "Flag to include jets and tracks associated to PV in the calculation"                                         );
    declareProperty("jetchargedp4",m_jetchargedp4     = "JetChargedScaleMomentum" ,   "Name of the jet charged momentum 4-vector"                                                                   );
    declareProperty("etaThresh",   m_etaThresh        = 2.5                       ,   "Maximum eta value for considering a jet as central"                                                          );
    declareProperty("forwardMinPt",m_forwardMinPt     = 20e3                      ,   "Minimum forward jet pt"                                                                                      );
    declareProperty("forwardMaxPt",m_forwardMaxPt     = -1                        ,   "Maximum forward jet pt. If -1 no threshold is applied"                                                       );
    declareProperty("centralMinPt",m_centerMinPt      = 20e3                      ,   "Minimum central jet pt"                                                                                      );
    declareProperty("centralMaxPt",m_centerMaxPt      = -1                        ,   "Maximum central jet pt. If -1 no threshold is applied"                                                       );
    declareProperty("pvind",       m_pvind            = -1                        ,   "Hard-Scatter primary vertex index of the event. If -1 it's automatically retrieved from the event"           );
    declareProperty("rptCut",      m_rptCut           = 0.1                       ,   "Rpt cut value for central PU jets contributing in the missing momentum calculation"                          );
    declareProperty("jvtCut",      m_jvtCut           = 0.2                       ,   "JVT threshold value for considering a central PU jet as HS"                                                  );
    declareProperty("dzCut",       m_dzCut            = 2.                        ,   "Dz=z-z0 cut value for pfo objects participating in the HS vertex jet reco"                                   );
    declareProperty("vertices",    m_vertices         = 10                        ,   "Number of vertices for which the missing momentum is calculated"                                             );
    declareProperty("maxRap",      m_maxRap           = 2.5                       ,   "Maximum rapidity value in fastjet::AreaDefinition"                                                           );
    declareProperty("neutMaxRap",  m_neutMaxRap       = 2.5                       ,   "Maximum rapidity value for neutral pfos participating in jet reco"                                           );
    declareProperty("weight",      m_weight           = 0                         ,   "PFO weight value"                                                                                            );
    declareProperty("pfoToolName", m_pfoToolName      = "PFOTool"                 ,   "Name of PFO retriever tool"                                                                                  );
    declareProperty("wpfoToolName",m_wpfoToolName     = "WPFOTool"                ,   "Name of PFO weighting tool"                                                                                  );
    declareProperty("pfoJESName",  m_pfoJESName       = "pfoJES"                  ,   "Name of jet calibration tool"                                                                                );
    declareProperty("jetAlgo",     m_jetAlgo          = "AntiKt4EMPFlow"          ,   "Jet calibration collection name"                                                                             );
    declareProperty("caliconfig",  m_caliconfig       = "JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config",   "Calibration config for PFlow jets, need to be updated with latest one" );
    declareProperty("calibSeq",    m_calibSeq         = "JetArea_Residual_EtaJES" ,   "Calibration sequence to be applied"                                                                          );
    declareProperty("calibArea",   m_calibArea        = "00-04-82"                ,   "Calibration area"                                                                                            );
    declareProperty("isdata",      m_isdata           = false                     ,   "True if data"                                                                                                );
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
    if (m_orLabel!="")  Dec_OR = std::make_unique<SG::AuxElement::Decorator<char> >(m_orLabel);
    Dec_outFjvt = std::make_unique<SG::AuxElement::Decorator<char> >(m_outLabelFjvt);
    Dec_outFjvtRaw = std::make_unique<SG::AuxElement::Decorator<float> >(m_outLabel);

    m_pfotool.setTypeAndName("CP::RetrievePFOTool/"+ m_pfoToolName );
    ATH_CHECK( m_pfotool.retrieve() );

    m_wpfotool.setTypeAndName("CP::WeightPFOTool/"+ m_wpfoToolName );
    ATH_CHECK( m_wpfotool.retrieve() );

    m_pfoJES.setTypeAndName("JetCalibrationTool/"+ m_pfoJESName   );
    ATH_CHECK( m_pfoJES.setProperty("JetCollection",m_jetAlgo                ) );
    ATH_CHECK( m_pfoJES.setProperty("ConfigFile"   ,m_caliconfig             ) );
    ATH_CHECK( m_pfoJES.setProperty("CalibSequence",m_calibSeq               ) );
    ATH_CHECK( m_pfoJES.setProperty("CalibArea"    ,m_calibArea              ) );
    ATH_CHECK( m_pfoJES.setProperty("IsData"       ,m_isdata                 ) );
    ATH_CHECK( m_pfoJES.retrieve() ) ;

    return StatusCode::SUCCESS;
  }

  int JetForwardPFlowJvtTool::modify(xAOD::JetContainer& jetCont) const {
    std::vector<TVector2> pileupMomenta;
    pileupMomenta=calculateVertexMomenta(&jetCont,m_pvind, m_vertices);

    if(pileupMomenta.size()==0) {
      ATH_MSG_DEBUG( "pileupMomenta is empty, this can happen for events with no PU vertices. fJVT won't be computed for this event and will be set to 0 instead." );
      for(const xAOD::Jet* jetF : jetCont) {
	(*Dec_outFjvt)(*jetF) = 1;
	(*Dec_outFjvtRaw)(*jetF) = 0;
      }
      return 0;
    }

    for(const xAOD::Jet* jetF : jetCont) {
      (*Dec_outFjvt)(*jetF) = 1;
      (*Dec_outFjvtRaw)(*jetF) = 0;

      if (isForwardJet(jetF)){
       double fjvt = getFJVT(jetF,pileupMomenta);
       if (fjvt>m_fjvtThresh) (*Dec_outFjvt)(*jetF) = 0;
       (*Dec_outFjvtRaw)(*jetF) = fjvt;
      }
    }
    return 0;
  }

  float JetForwardPFlowJvtTool::getFJVT(const xAOD::Jet *jet, std::vector<TVector2> pileupMomenta) const {
    TVector2 fjet(jet->pt()*cos(jet->phi()),jet->pt()*sin(jet->phi()));
    double fjvt = 0;
    for (size_t pui = 0; pui < pileupMomenta.size(); pui++) {
      double projection = pileupMomenta[pui]*fjet/fjet.Mod();
      if (projection<fjvt) fjvt = projection;
    }
    return -1*fjvt/fjet.Mod();
  }

  std::vector<TVector2> JetForwardPFlowJvtTool::calculateVertexMomenta(const xAOD::JetContainer *pjets, int pvind, int vertices) const {
    std::vector<TVector2> pileupMomenta;
    // -- Retrieve PV index if not provided by user
    const std::size_t pv_index = (pvind==-1) ? getPV() : std::size_t(pvind);

    const xAOD::VertexContainer *vxCont = 0;
    if( evtStore()->retrieve(vxCont, m_verticesName).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve primary vertex container \"" << m_verticesName << "\"");
      return pileupMomenta;
    }
    const xAOD::PFOContainer *pfos = m_pfotool->retrievePFO(CP::EM,CP::all);
    if(pfos == NULL ){
      ATH_MSG_WARNING("PFO container is empty!");
      return pileupMomenta;
    } else {
      ATH_MSG_DEBUG("Successfully retrieved PFO objects");
    }

    for(const xAOD::Vertex* vx: *vxCont) {
      if(vx->vertexType()!=xAOD::VxType::PriVtx && vx->vertexType()!=xAOD::VxType::PileUp) continue;
      if(vx->index()==(size_t)pv_index) continue;

      TString jname = m_jetsName;
      jname += vx->index();
      const xAOD::JetContainer* vertex_jets  = nullptr;

      // Check if pflow pileup jet container exists
      if( !evtStore()->contains<xAOD::JetContainer>(jname.Data()) ){
	// if not, build it
	if( buildPFlowPUjets(*vx,*pfos).isFailure() ){
	  ATH_MSG_WARNING(" Some issue appeared while building the pflow pileup jets for vertex "<< vx->index() << " (vxType = " << vx->vertexType()<<" )!" );
	  return pileupMomenta;
	}
      } else {
	ATH_MSG_WARNING( jname.Data() << " already exists. Existing container will be used.");
      }
      
      if(evtStore()->retrieve(vertex_jets,jname.Data()).isFailure()){
        ATH_MSG_WARNING("Unable to retrieve built PU jets with name \"" << m_jetsName << "\"");
        return pileupMomenta;
      }

      TVector2 vertex_met;
      for (const xAOD::Jet *jet : *vertex_jets) {

        // Remove jets which are close to hs
        if (!m_includePV && hasCloseByHSjet(jet,pjets)) continue;

        // Calculate vertex missing momentum
        if (isCentralJet(jet) && getRpt(jet)> m_rptCut)
        { 
          vertex_met += TVector2(jet->pt()*cos(jet->phi()),jet->pt()*sin(jet->phi()) ) ;
        } 
        else{
          vertex_met += TVector2(jet->jetP4(m_jetchargedp4).Pt()*cos(jet->jetP4(m_jetchargedp4).Phi()), jet->jetP4(m_jetchargedp4).Pt()*sin(jet->jetP4(m_jetchargedp4).Phi()) );
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
      pjet->getAttribute<float>("Jvt",jet_jvt);
     if (pjet->p4().DeltaR(jet->p4())<0.3 && jet_jvt>m_jvtCut && isCentralJet(pjet) ) return true;
    }
    return false;
  }

  StatusCode JetForwardPFlowJvtTool::buildPFlowPUjets(const xAOD::Vertex &vx, const xAOD::PFOContainer &pfos) const {
    
    const std::size_t pv_index = (m_pvind==-1) ? getPV() : std::size_t (m_pvind);

    std::vector<fastjet::PseudoJet> input_pfo;
    std::set<int> charged_pfo;
    for(const xAOD::PFO* pfo : pfos){ 
      if (Dec_OR && !(*Dec_OR)(*pfo)) continue;
      if (pfo->isCharged()) { 
        if (vx.index()==pv_index && fabs((vx.z()-pfo->track(0)->z0())*sin(pfo->track(0)->theta()))>m_dzCut) continue;
        if (vx.index()!=pv_index && (!pfo->track(0)->vertex() || vx.index()!=pfo->track(0)->vertex()->index())) continue;
        input_pfo.push_back(pfoToPseudoJet(pfo, CP::charged, &vx) );
        charged_pfo.insert(pfo->index());
      } 
      else if (fabs(pfo->eta())<m_neutMaxRap && !pfo->isCharged() && pfo->eEM()>0)
      { 
        input_pfo.push_back(pfoToPseudoJet(pfo, CP::neutral, &vx) );
      }
    }
    std::unique_ptr<xAOD::JetContainer> vertjets = std::make_unique<xAOD::JetContainer>();
    std::unique_ptr<xAOD::JetAuxContainer> vertjetsAux = std::make_unique<xAOD::JetAuxContainer>();
    vertjets->setStore(vertjetsAux.get());
    TString newname = m_jetsName;
    newname += vx.index();

    fastjet::JetDefinition jet_def(fastjet::antikt_algorithm,0.4);
    fastjet::AreaDefinition area_def(fastjet::active_area_explicit_ghosts,fastjet::GhostedAreaSpec(fastjet::SelectorAbsRapMax(m_maxRap)));
    fastjet::ClusterSequenceArea clust_pfo(input_pfo,jet_def,area_def);
    std::vector<fastjet::PseudoJet> inclusive_jets = sorted_by_pt(clust_pfo.inclusive_jets(5000.));

    for (size_t i = 0; i < inclusive_jets.size(); i++) {
      xAOD::Jet* jet=  new xAOD::Jet();
      xAOD::JetFourMom_t tempjetp4(inclusive_jets[i].pt(),inclusive_jets[i].eta(),inclusive_jets[i].phi(),inclusive_jets[i].m());
      xAOD::JetFourMom_t newArea(inclusive_jets[i].area_4vector().perp(),inclusive_jets[i].area_4vector().eta(),inclusive_jets[i].area_4vector().phi(),inclusive_jets[i].area_4vector().m());
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

    if( m_pfoJES->modify(*vertjets) ){
      ATH_MSG_ERROR(" Failed to calibrate PU jet container ");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( evtStore()->record(vertjets.release(),newname.Data())    );
    ATH_CHECK( evtStore()->record(vertjetsAux.release(),newname.Data()) );
    return StatusCode::SUCCESS;
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

  bool JetForwardPFlowJvtTool::isForwardJet(const xAOD::Jet *jet) const {
    if (fabs(jet->eta())<m_etaThresh) return false;
    if (jet->pt()<m_forwardMinPt || (m_forwardMaxPt>0 && jet->pt()>m_forwardMaxPt) ) return false;
    return true;
  }

  bool JetForwardPFlowJvtTool::isCentralJet(const xAOD::Jet *jet) const {
    if (fabs(jet->eta())>m_etaThresh) return false;
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

    const xAOD::VertexContainer *vxCont = 0;
    if( evtStore()->retrieve(vxCont, m_verticesName).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve primary vertex container");
      // this almost certainly isn't what we should do here, the
      // caller doesn't check this for errors
      return 0;
    } else {
      ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
      for(const xAOD::Vertex *vx : *vxCont) {
        if(vx->vertexType()==xAOD::VxType::PriVtx) return vx->index();
      }
    }
    ATH_MSG_WARNING("Couldn't identify the hard-scatter primary vertex (no vertex with \"vx->vertexType()==xAOD::VxType::PriVtx\" in the container)!");
    // this almost certainly isn't what we should do here, the
    // caller doesn't check this for errors
    return 0;
  }

  StatusCode JetForwardPFlowJvtTool::tagTruth(const xAOD::JetContainer *jets,const xAOD::JetContainer *truthJets) {
    for(const xAOD::Jet *jet : *jets) {
      bool ishs = false;
      bool ispu = true;
      for(const xAOD::Jet *tjet : *truthJets) {
        if (tjet->p4().DeltaR(jet->p4())<0.3 && tjet->pt()>10e3) ishs = true;
        if (tjet->p4().DeltaR(jet->p4())<0.6) ispu = false;
      }
      isHS(*jet)=ishs;
      isPU(*jet)=ispu;
    }
    return StatusCode::SUCCESS;
  }

