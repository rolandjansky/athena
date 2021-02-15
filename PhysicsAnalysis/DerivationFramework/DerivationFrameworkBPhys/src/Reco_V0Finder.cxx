/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/////////////////////////////////////////////////////////////////
// Reco_V0Finder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Adam Barton
#include "DerivationFrameworkBPhys/Reco_V0Finder.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "EventKernel/PdtPdg.h"

namespace DerivationFramework {

  Reco_V0Finder::Reco_V0Finder(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_v0FinderTool("InDet::V0FinderTool"),
    m_V0Tools("Trk::V0Tools"),
    m_particleDataTable(nullptr),
    m_masses(1),
    m_masspi(139.57),
    m_massp(938.272),
    m_masse(0.510999),
    m_massK0S(497.672),
    m_massLambda(1115.68),
    m_VxPrimaryCandidateName("PrimaryVertices"),
    m_v0ContainerName("RecoV0Candidates"),
    m_ksContainerName("RecoKshortCandidates"),
    m_laContainerName("RecoLambdaCandidates"),
    m_lbContainerName("RecoLambdabarCandidates")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare user-defined properties
    declareProperty("CheckVertexContainers", m_CollectionsToCheck);
    declareProperty("V0FinderTool", m_v0FinderTool);
    declareProperty("V0Tools", m_V0Tools);
    declareProperty("masses", m_masses);
    declareProperty("masspi", m_masspi);
    declareProperty("massp", m_massp);
    declareProperty("masse", m_masse);
    declareProperty("massK0S", m_massK0S);
    declareProperty("massLambda", m_massLambda);
    declareProperty("VxPrimaryCandidateName", m_VxPrimaryCandidateName);
    declareProperty("V0ContainerName", m_v0ContainerName);
    declareProperty("KshortContainerName", m_ksContainerName);
    declareProperty("LambdaContainerName", m_laContainerName);
    declareProperty("LambdabarContainerName", m_lbContainerName);
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Reco_V0Finder::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
 
    // get the V0Finder tool
    ATH_CHECK( m_v0FinderTool.retrieve());

    // uploading the V0 tools
    ATH_CHECK( m_V0Tools.retrieve());

    // get the Particle Properties Service
    IPartPropSvc* partPropSvc = nullptr;
    StatusCode sc = service("PartPropSvc", partPropSvc, true);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not initialize Particle Properties Service" << endmsg;
      return StatusCode::FAILURE;
    }
    m_particleDataTable = partPropSvc->PDT();

    const HepPDT::ParticleData* pd_pi = m_particleDataTable->particle(PDG::pi_plus);
    const HepPDT::ParticleData* pd_p  = m_particleDataTable->particle(PDG::p_plus);
    const HepPDT::ParticleData* pd_e  = m_particleDataTable->particle(PDG::e_minus);
    const HepPDT::ParticleData* pd_K  = m_particleDataTable->particle(PDG::K_S0);
    const HepPDT::ParticleData* pd_L  = m_particleDataTable->particle(PDG::Lambda0);
    if (m_masses == 1) {
     m_masspi     = pd_pi->mass();
     m_massp      = pd_p->mass();
     m_masse      = pd_e->mass();
     m_massK0S    = pd_K->mass();
     m_massLambda = pd_L->mass();
    }


    return StatusCode::SUCCESS;
    
  }
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  StatusCode Reco_V0Finder::finalize()
  {
    // everything all right
    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Reco_V0Finder::addBranches() const
  {
    bool callV0Finder = false;
    // Jpsi container and its auxilliary store
    for(const auto &str : m_CollectionsToCheck){
       const xAOD::VertexContainer*    vertContainer = nullptr;
       ATH_CHECK( evtStore()->retrieve(vertContainer, str) );
       if(vertContainer->size() == 0) {
          ATH_MSG_DEBUG("Container VertexContainer (" << str << ") is empty");
       }else{
          callV0Finder = true;
          ATH_MSG_DEBUG("Container VertexContainer (" << str << ") has events N= " << vertContainer->size());
          break;//No point checking other containers
       }
    }

    // Call V0Finder


// InDetV0 container and its auxilliary store
    xAOD::VertexContainer*    v0Container(nullptr);
    xAOD::VertexAuxContainer* v0AuxContainer(nullptr);
    xAOD::VertexContainer*    ksContainer(nullptr);
    xAOD::VertexAuxContainer* ksAuxContainer(nullptr);
    xAOD::VertexContainer*    laContainer(nullptr);
    xAOD::VertexAuxContainer* laAuxContainer(nullptr);
    xAOD::VertexContainer*    lbContainer(nullptr);
    xAOD::VertexAuxContainer* lbAuxContainer(nullptr);

    if (callV0Finder) {

    // Get primary vertex from StoreGate
    const xAOD::Vertex * primaryVertex(0);
    const xAOD::VertexContainer * importedVxContainer(0);
    ATH_CHECK( evtStore()->retrieve(importedVxContainer, m_VxPrimaryCandidateName) );
    ATH_MSG_DEBUG("Found " << m_VxPrimaryCandidateName << " in StoreGate!");
    if (importedVxContainer->size()==0){
        ATH_MSG_WARNING("You have no primary vertices: " << importedVxContainer->size());
    } else {
        primaryVertex = (*importedVxContainer)[0];
    }

    ATH_CHECK(m_v0FinderTool->performSearch(v0Container, v0AuxContainer, ksContainer, ksAuxContainer, laContainer, laAuxContainer, lbContainer, lbAuxContainer, primaryVertex, importedVxContainer));

    ATH_MSG_DEBUG("Reco_V0Finder v0Container->size() " << v0Container->size());
    ATH_MSG_DEBUG("Reco_V0Finder ksContainer->size() " << ksContainer->size());
    ATH_MSG_DEBUG("Reco_V0Finder laContainer->size() " << laContainer->size());
    ATH_MSG_DEBUG("Reco_V0Finder lbContainer->size() " << lbContainer->size());

    SG::AuxElement::Decorator<float> mDecor_Ksmass("Kshort_mass");
    SG::AuxElement::Decorator<float> mDecor_Ksmasse("Kshort_massError");
    SG::AuxElement::Decorator<float> mDecor_Lamass("Lambda_mass");
    SG::AuxElement::Decorator<float> mDecor_Lamasse("Lambda_massError");
    SG::AuxElement::Decorator<float> mDecor_Lbmass("Lambdabar_mass");
    SG::AuxElement::Decorator<float> mDecor_Lbmasse("Lambdabar_massError");
    SG::AuxElement::Decorator<float> mDecor_mass("mass");
    SG::AuxElement::Decorator<float> mDecor_massError("massError");
    SG::AuxElement::Decorator<float> mDecor_pt("pT");
    SG::AuxElement::Decorator<float> mDecor_ptError("pTError");
    SG::AuxElement::Decorator<float> mDecor_rxy("Rxy");
    SG::AuxElement::Decorator<float> mDecor_rxyError("RxyError");
    SG::AuxElement::Decorator<float> mDecor_px("px");
    SG::AuxElement::Decorator<float> mDecor_py("py");
    SG::AuxElement::Decorator<float> mDecor_pz("pz");

    xAOD::VertexContainer::const_iterator v0Itr = v0Container->begin();
    for ( v0Itr=v0Container->begin(); v0Itr!=v0Container->end(); ++v0Itr )
    {
      const xAOD::Vertex * unconstrV0 = (*v0Itr);
      double mass_ks = m_V0Tools->invariantMass(unconstrV0,m_masspi,m_masspi);
      double mass_error_ks = m_V0Tools->invariantMassError(unconstrV0,m_masspi,m_masspi);
      double mass_la = m_V0Tools->invariantMass(unconstrV0,m_massp,m_masspi);
      double mass_error_la = m_V0Tools->invariantMassError(unconstrV0,m_massp,m_masspi);
      double mass_lb = m_V0Tools->invariantMass(unconstrV0,m_masspi,m_massp);
      double mass_error_lb = m_V0Tools->invariantMassError(unconstrV0,m_masspi,m_massp);
      double pt = m_V0Tools->pT(unconstrV0);
      double ptError = m_V0Tools->pTError(unconstrV0);
      double rxy = m_V0Tools->rxy(unconstrV0);
      double rxyError = m_V0Tools->rxyError(unconstrV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(unconstrV0);
      mDecor_Ksmass( *unconstrV0 ) = mass_ks;
      mDecor_Ksmasse( *unconstrV0 ) = mass_error_ks;
      mDecor_Lamass( *unconstrV0 ) = mass_la;
      mDecor_Lamasse( *unconstrV0 ) = mass_error_la;
      mDecor_Lbmass( *unconstrV0 ) = mass_lb;
      mDecor_Lbmasse( *unconstrV0 ) = mass_error_lb;
      mDecor_pt( *unconstrV0 ) = pt;
      mDecor_ptError( *unconstrV0 ) = ptError;
      mDecor_rxy( *unconstrV0 ) = rxy;
      mDecor_rxyError( *unconstrV0 ) = rxyError;
      mDecor_px( *unconstrV0 ) = momentum.x();
      mDecor_py( *unconstrV0 ) = momentum.y();
      mDecor_pz( *unconstrV0 ) = momentum.z();
      ATH_MSG_DEBUG("Reco_V0Finder mass_ks " << mass_ks << " mass_la " << mass_la << " mass_lb " << mass_lb);
    }
    xAOD::VertexContainer::const_iterator ksItr = ksContainer->begin();
    for ( ksItr=ksContainer->begin(); ksItr!=ksContainer->end(); ++ksItr )
    {
      const xAOD::Vertex * ksV0 = (*ksItr);
      double mass_ks = m_V0Tools->invariantMass(ksV0,m_masspi,m_masspi);
      double mass_error_ks = m_V0Tools->invariantMassError(ksV0,m_masspi,m_masspi);
      double pt = m_V0Tools->pT(ksV0);
      double ptError = m_V0Tools->pTError(ksV0);
      double rxy = m_V0Tools->rxy(ksV0);
      double rxyError = m_V0Tools->rxyError(ksV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(ksV0);
      mDecor_mass( *ksV0 ) = mass_ks;
      mDecor_massError( *ksV0 ) = mass_error_ks;
      mDecor_pt( *ksV0 ) = pt;
      mDecor_ptError( *ksV0 ) = ptError;
      mDecor_rxy( *ksV0 ) = rxy;
      mDecor_rxyError( *ksV0 ) = rxyError;
      mDecor_px( *ksV0 ) = momentum.x();
      mDecor_py( *ksV0 ) = momentum.y();
      mDecor_pz( *ksV0 ) = momentum.z();
      ATH_MSG_DEBUG("Reco_V0Finder mass_ks " << mass_ks << " mass_error_ks " << mass_error_ks << " pt " << pt << " rxy " << rxy);
    }
    xAOD::VertexContainer::const_iterator laItr = laContainer->begin();
    for ( laItr=laContainer->begin(); laItr!=laContainer->end(); ++laItr )
    {
      const xAOD::Vertex * laV0 = (*laItr);
      double mass_la = m_V0Tools->invariantMass(laV0,m_massp,m_masspi);
      double mass_error_la = m_V0Tools->invariantMassError(laV0,m_massp,m_masspi);
      double pt = m_V0Tools->pT(laV0);
      double ptError = m_V0Tools->pTError(laV0);
      double rxy = m_V0Tools->rxy(laV0);
      double rxyError = m_V0Tools->rxyError(laV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(laV0);
      mDecor_mass( *laV0 ) = mass_la;
      mDecor_massError( *laV0 ) = mass_error_la;
      mDecor_pt( *laV0 ) = pt;
      mDecor_ptError( *laV0 ) = ptError;
      mDecor_rxy( *laV0 ) = rxy;
      mDecor_rxyError( *laV0 ) = rxyError;
      mDecor_px( *laV0 ) = momentum.x();
      mDecor_py( *laV0 ) = momentum.y();
      mDecor_pz( *laV0 ) = momentum.z();
      ATH_MSG_DEBUG("Reco_V0Finder mass_la " << mass_la << " mass_error_la " << mass_error_la << " pt " << pt << " rxy " << rxy);
    }
    xAOD::VertexContainer::const_iterator lbItr = lbContainer->begin();
    for ( lbItr=lbContainer->begin(); lbItr!=lbContainer->end(); ++lbItr )
    {
      const xAOD::Vertex * lbV0 = (*lbItr);
      double mass_lb = m_V0Tools->invariantMass(lbV0,m_masspi,m_massp);
      double mass_error_lb = m_V0Tools->invariantMassError(lbV0,m_masspi,m_massp);
      double pt = m_V0Tools->pT(lbV0);
      double ptError = m_V0Tools->pTError(lbV0);
      double rxy = m_V0Tools->rxy(lbV0);
      double rxyError = m_V0Tools->rxyError(lbV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(lbV0);
      mDecor_mass( *lbV0 ) = mass_lb;
      mDecor_massError( *lbV0 ) = mass_error_lb;
      mDecor_pt( *lbV0 ) = pt;
      mDecor_ptError( *lbV0 ) = ptError;
      mDecor_rxy( *lbV0 ) = rxy;
      mDecor_rxyError( *lbV0 ) = rxyError;
      mDecor_px( *lbV0 ) = momentum.x();
      mDecor_py( *lbV0 ) = momentum.y();
      mDecor_pz( *lbV0 ) = momentum.z();
      ATH_MSG_DEBUG("Reco_V0Finder mass_lb " << mass_lb << " mass_error_lb " << mass_error_lb << " pt " << pt << " rxy " << rxy);
    }
    }


    if(!callV0Finder){ //Fill with empty containers
      v0Container = new xAOD::VertexContainer;
      v0AuxContainer = new xAOD::VertexAuxContainer;
      v0Container->setStore(v0AuxContainer);
      ksContainer = new xAOD::VertexContainer;
      ksAuxContainer = new xAOD::VertexAuxContainer;
      ksContainer->setStore(ksAuxContainer);
      laContainer = new xAOD::VertexContainer;
      laAuxContainer = new xAOD::VertexAuxContainer;
      laContainer->setStore(laAuxContainer);
      lbContainer = new xAOD::VertexContainer;
      lbAuxContainer = new xAOD::VertexAuxContainer;
      lbContainer->setStore(lbAuxContainer);
    }

    //---- Recording section: write the results to StoreGate ---//
    CHECK(evtStore()->record(v0Container, m_v0ContainerName));
  
    CHECK(evtStore()->record(v0AuxContainer, m_v0ContainerName+"Aux."));
  
    CHECK(evtStore()->record(ksContainer, m_ksContainerName));
  
    CHECK(evtStore()->record(ksAuxContainer, m_ksContainerName+"Aux."));
  
    CHECK(evtStore()->record(laContainer, m_laContainerName));
  
    CHECK(evtStore()->record(laAuxContainer, m_laContainerName+"Aux."));
  
    CHECK(evtStore()->record(lbContainer, m_lbContainerName));
  
    CHECK(evtStore()->record(lbAuxContainer, m_lbContainerName+"Aux."));

    return StatusCode::SUCCESS;    
  }
}




