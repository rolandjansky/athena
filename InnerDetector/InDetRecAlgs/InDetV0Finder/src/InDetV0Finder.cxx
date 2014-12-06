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

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "InDetV0Finder/InDetV0Finder.h"

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
  m_V0Tools("Trk::V0Tools"),
  m_v0FinderTool("InDet::InDetV0FinderTool"),
  m_masses(1),
  m_masspi(139.57),
  m_massp(938.272),
  m_masse(0.510999),
  m_massK0S(497.672),
  m_massLambda(1115.68),
  m_VxPrimaryCandidateName("PrimaryVertices"),
  m_v0ContainerName("V0Candidates"),
  m_ksContainerName("KshortCandidates"),
  m_laContainerName("LambdaCandidates"),
  m_lbContainerName("LambdabarCandidates")
{
  declareProperty("V0Tools",m_V0Tools);
  declareProperty("InDetV0FinderToolName"   , m_v0FinderTool);
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

// get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  sc = service("PartPropSvc", partPropSvc, true);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not initialize Particle Properties Service" << endreq;
    return StatusCode::FAILURE;
  }
  m_particleDataTable = partPropSvc->PDT();

  sc = resetStatistics();     // reset counters
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Error in resetStatistics !" << endreq;
    return sc;
  }

// uploading the V0Finding tools
  if ( m_v0FinderTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_v0FinderTool << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_v0FinderTool << endreq;
  }

// uploading the corresponding V0 tools
  if ( m_V0Tools.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_V0Tools << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_V0Tools << endreq;
  }

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

  msg(MSG::INFO) << "Initialization successful" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode InDetV0Finder::execute()
{
  StatusCode sc;

  m_events_processed++;

// Get primary vertex from StoreGate
  xAOD::Vertex * primaryVertex = NULL;
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
  xAOD::VertexContainer*    v0Container = NULL;
  xAOD::VertexAuxContainer* v0AuxContainer = NULL;
  xAOD::VertexContainer*    ksContainer = NULL;
  xAOD::VertexAuxContainer* ksAuxContainer = NULL;
  xAOD::VertexContainer*    laContainer = NULL;
  xAOD::VertexAuxContainer* laAuxContainer = NULL;
  xAOD::VertexContainer*    lbContainer = NULL;
  xAOD::VertexAuxContainer* lbAuxContainer = NULL;

// call InDetV0 finder
  if ( !m_v0FinderTool->performSearch(v0Container,v0AuxContainer,ksContainer,ksAuxContainer,laContainer,laAuxContainer,lbContainer,lbAuxContainer,primaryVertex,m_VxPrimaryCandidateName).isSuccess() )
  //if ( !m_v0FinderTool->performSearch(v0Container,v0AuxContainer,ksContainer,ksAuxContainer,laContainer,laAuxContainer,lbContainer,lbAuxContainer,NULL,m_VxPrimaryCandidateName).isSuccess() )
  {
    ATH_MSG_FATAL("InDetV0 finder (" << m_v0FinderTool << ") failed.");
    return StatusCode::FAILURE;
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
    << "----------------------------------------------------------------------------------------------------------------------------------------------" << endreq
    << "\tSummary" << endreq 
    << "\tProcessed              : " << m_events_processed            << " events" << endreq
    << "\tStored                 : " << m_V0s_stored                  << " V0s" << endreq
    << "\tof which               : " << m_Kshort_stored               << " Kshorts" << endreq
    << "\t                       : " << m_Lambda_stored               << " Lambdas" << endreq
    << "\t                       : " << m_Lambdabar_stored            << " Lambdabars" << endreq;
  msg(MSG::INFO) << "----------------------------------------------------------------------------------------------------------------------------------------------" << endreq;

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

