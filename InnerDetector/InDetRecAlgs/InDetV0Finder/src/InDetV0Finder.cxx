/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetV0Finder.cxx  -  Description
                             -------------------
    begin   : 20-07-2005
    authors : Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University)
    email   : e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
    changes : December 2014
    author  : Evelina Bouhova-Thacker <e.bouhova@cern.ch> 
              Changed to use xAOD

 ***************************************************************************/

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "InDetV0Finder/InDetV0Finder.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "GaudiKernel/IPartPropSvc.h"

#include <vector>
#include <cmath>
#include <string>
#include "TVector3.h"

namespace InDet
{
  
InDetV0Finder::InDetV0Finder(const std::string &n, ISvcLocator *pSvcLoc)
  :
  AthAlgorithm(n, pSvcLoc),
  m_v0FinderTool("InDet::InDetV0FinderTool"),
  m_V0Tools("Trk::V0Tools"),
  m_particleDataTable(0),
  m_decorate(true),
  m_masses(1),
  m_masspi(139.57),
  m_massp(938.272),
  m_masse(0.510999),
  m_massK0S(497.672),
  m_massLambda(1115.68),
  m_VxPrimaryCandidateName("PrimaryVertices"),
  m_events_processed(0),
  m_V0s_stored(0),
  m_Kshort_stored(0),
  m_Lambda_stored(0),
  m_Lambdabar_stored(0),
  m_v0ContainerName("V0Candidates"),
  m_ksContainerName("KshortCandidates"),
  m_laContainerName("LambdaCandidates"),
  m_lbContainerName("LambdabarCandidates")
{
  declareProperty("InDetV0FinderToolName"   , m_v0FinderTool);
  declareProperty("V0Tools",m_V0Tools);
  declareProperty("decorateV0", m_decorate);
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

InDetV0Finder::~InDetV0Finder() {}

StatusCode InDetV0Finder::initialize()
{
  StatusCode sc;

  sc = resetStatistics();     // reset counters
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Error in resetStatistics !" << endmsg;
    return sc;
  }

// uploading the V0Finding tools
  if ( m_v0FinderTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_v0FinderTool << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_v0FinderTool << endmsg;
  }

// uploading the V0 tools
  if ( m_V0Tools.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_V0Tools << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_V0Tools << endmsg;
  }

// get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  sc = service("PartPropSvc", partPropSvc, true);
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

  msg(MSG::INFO) << "Initialization successful" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode InDetV0Finder::execute()
{
  StatusCode sc;

  m_events_processed++;

// Get primary vertex from StoreGate
  const xAOD::Vertex * primaryVertex(0);
  const xAOD::VertexContainer * importedVxContainer(0);
  sc = evtStore()->retrieve(importedVxContainer,m_VxPrimaryCandidateName);
  if (sc.isFailure() ) {
    ATH_MSG_WARNING("No VxPrimaryCandidate found in StoreGate");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Found " << m_VxPrimaryCandidateName << " in StoreGate!");
  }
  if (importedVxContainer!=0) {
    if (importedVxContainer->size()==0){
      ATH_MSG_WARNING("You have no primary vertices: " << importedVxContainer->size());
    } else {
      xAOD::VertexContainer::const_iterator pvItr = importedVxContainer->begin();
      primaryVertex = *pvItr;
    }
  }

// InDetV0 container and its auxilliary store
  xAOD::VertexContainer*    v0Container(0);
  xAOD::VertexAuxContainer* v0AuxContainer(0);
  xAOD::VertexContainer*    ksContainer(0);
  xAOD::VertexAuxContainer* ksAuxContainer(0);
  xAOD::VertexContainer*    laContainer(0);
  xAOD::VertexAuxContainer* laAuxContainer(0);
  xAOD::VertexContainer*    lbContainer(0);
  xAOD::VertexAuxContainer* lbAuxContainer(0);

// call InDetV0 finder
  if ( !m_v0FinderTool->performSearch(v0Container,v0AuxContainer,ksContainer,ksAuxContainer,laContainer,laAuxContainer,lbContainer,lbAuxContainer,primaryVertex,m_VxPrimaryCandidateName).isSuccess() )
  //if ( !m_v0FinderTool->performSearch(v0Container,v0AuxContainer,ksContainer,ksAuxContainer,laContainer,laAuxContainer,lbContainer,lbAuxContainer,NULL,m_VxPrimaryCandidateName).isSuccess() )
  {
    ATH_MSG_FATAL("InDetV0 finder (" << m_v0FinderTool << ") failed.");
    return StatusCode::FAILURE;
  }

  if (m_decorate) {
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
    }
  }

  m_V0s_stored += v0Container->size();
  m_Kshort_stored += ksContainer->size();
  m_Lambda_stored += laContainer->size();
  m_Lambdabar_stored += lbContainer->size();

  //---- Recording section: write the results to StoreGate ---//
  if (!evtStore()->contains<xAOD::VertexContainer>(m_v0ContainerName))
    CHECK(evtStore()->record(v0Container, m_v0ContainerName));

  if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_v0ContainerName+"Aux."))
    CHECK(evtStore()->record(v0AuxContainer, m_v0ContainerName+"Aux."));

  if (!evtStore()->contains<xAOD::VertexContainer>(m_ksContainerName))
    CHECK(evtStore()->record(ksContainer, m_ksContainerName));

  if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_ksContainerName+"Aux."))
    CHECK(evtStore()->record(ksAuxContainer, m_ksContainerName+"Aux."));

  if (!evtStore()->contains<xAOD::VertexContainer>(m_laContainerName))
    CHECK(evtStore()->record(laContainer, m_laContainerName));

  if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_laContainerName+"Aux."))
    CHECK(evtStore()->record(laAuxContainer, m_laContainerName+"Aux."));

  if (!evtStore()->contains<xAOD::VertexContainer>(m_lbContainerName))
    CHECK(evtStore()->record(lbContainer, m_lbContainerName));

  if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_lbContainerName+"Aux."))
    CHECK(evtStore()->record(lbAuxContainer, m_lbContainerName+"Aux."));


  return StatusCode::SUCCESS;
}// end execute block

StatusCode InDetV0Finder::finalize()
{
  msg(MSG::INFO)
    << "----------------------------------------------------------------------------------------------------------------------------------------------" << endmsg
    << "\tSummary" << endmsg 
    << "\tProcessed              : " << m_events_processed            << " events" << endmsg
    << "\tStored                 : " << m_V0s_stored                  << " V0s" << endmsg
    << "\tof which               : " << m_Kshort_stored               << " Kshorts" << endmsg
    << "\t                       : " << m_Lambda_stored               << " Lambdas" << endmsg
    << "\t                       : " << m_Lambdabar_stored            << " Lambdabars" << endmsg;
  msg(MSG::INFO) << "----------------------------------------------------------------------------------------------------------------------------------------------" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode InDetV0Finder::resetStatistics() {
    m_events_processed           = 0;
    m_V0s_stored                 = 0;
    m_Kshort_stored              = 0;
    m_Lambdabar_stored           = 0;
    m_Lambda_stored              = 0;
  
    return StatusCode :: SUCCESS;
}


}//end of namespace InDet

