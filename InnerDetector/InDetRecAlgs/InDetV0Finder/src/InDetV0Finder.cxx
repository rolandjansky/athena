/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "InDetV0Finder.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "InDetV0Finder/InDetV0FinderTool.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "StoreGate/WriteDecorHandle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "EventKernel/PdtPdg.h"

#include <vector>
#include <cmath>
#include <string>


namespace InDet
{
  
InDetV0Finder::InDetV0Finder(const std::string &n, ISvcLocator *pSvcLoc)
  :
  AthAlgorithm(n, pSvcLoc),
  m_v0FinderTool("InDet::InDetV0FinderTool"),
  m_V0Tools("Trk::V0Tools"),
  m_particleDataTable(nullptr),
  m_decorate(true),
  m_masses(1),
  m_masspi(139.57),
  m_massp(938.272),
  m_masse(0.510999),
  m_massK0S(497.672),
  m_massLambda(1115.68),
  m_events_processed(0),
  m_V0s_stored(0),
  m_Kshort_stored(0),
  m_Lambda_stored(0),
  m_Lambdabar_stored(0)
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
}

InDetV0Finder::~InDetV0Finder() {}

StatusCode InDetV0Finder::initialize()
{
  ATH_CHECK( resetStatistics() );

  ATH_CHECK( m_vertexKey.initialize() );
  ATH_CHECK( m_v0Key.initialize() );
  ATH_CHECK( m_ksKey.initialize() );
  ATH_CHECK( m_laKey.initialize() );
  ATH_CHECK( m_lbKey.initialize() );

  // Make sure decoration keys match the VertexContainer name (which is configured at run-time by python for production jobs)
  m_decorKsMass = m_v0Key.key() + m_decorKsMass.key();
  m_decorLaMass = m_v0Key.key() + m_decorLaMass.key();
  m_decorLbMass = m_v0Key.key() + m_decorLaMass.key();
  m_decorKsMassErr = m_v0Key.key() + m_decorKsMassErr.key();
  m_decorLaMassErr = m_v0Key.key() + m_decorLaMassErr.key();
  m_decorLbMassErr = m_v0Key.key() + m_decorLbMassErr.key();

  ATH_CHECK( m_decorKsMass.initialize() );
  ATH_CHECK( m_decorLaMass.initialize() );
  ATH_CHECK( m_decorLbMass.initialize() );
  ATH_CHECK( m_decorKsMassErr.initialize() );
  ATH_CHECK( m_decorLaMassErr.initialize() );
  ATH_CHECK( m_decorLbMassErr.initialize() );

  m_decorMass_ks = m_ksKey.key() + m_decorMass_ks.key();
  m_decorMass_la = m_laKey.key() + m_decorMass_la.key();
  m_decorMass_lb = m_lbKey.key() + m_decorMass_lb.key();

  m_decorMassErr_ks = m_ksKey.key() + m_decorMassErr_ks.key();
  m_decorMassErr_la = m_laKey.key() + m_decorMassErr_la.key();
  m_decorMassErr_lb = m_lbKey.key() + m_decorMassErr_lb.key();

  ATH_CHECK( m_decorMass_ks.initialize() );
  ATH_CHECK( m_decorMass_la.initialize() );
  ATH_CHECK( m_decorMass_lb.initialize() );
  ATH_CHECK( m_decorMassErr_ks.initialize() );
  ATH_CHECK( m_decorMassErr_la.initialize() );
  ATH_CHECK( m_decorMassErr_lb.initialize() );

  m_decorPt_v0 = m_v0Key.key() + m_decorPt_v0.key();
  m_decorPt_ks = m_ksKey.key() + m_decorPt_ks.key();
  m_decorPt_la = m_laKey.key() + m_decorPt_la.key();
  m_decorPt_lb = m_lbKey.key() + m_decorPt_lb.key();
  m_decorPtErr_v0 = m_v0Key.key() + m_decorPtErr_v0.key();
  m_decorPtErr_ks = m_ksKey.key() + m_decorPtErr_ks.key();
  m_decorPtErr_la = m_laKey.key() + m_decorPtErr_la.key();
  m_decorPtErr_lb = m_lbKey.key() + m_decorPtErr_lb.key();
  m_decorRxy_v0 = m_v0Key.key() + m_decorRxy_v0.key();
  m_decorRxy_ks = m_ksKey.key() + m_decorRxy_ks.key();
  m_decorRxy_la = m_laKey.key() + m_decorRxy_la.key();
  m_decorRxy_lb = m_lbKey.key() + m_decorRxy_lb.key();
  m_decorRxyErr_v0 = m_v0Key.key() + m_decorRxyErr_v0.key();
  m_decorRxyErr_ks = m_ksKey.key() + m_decorRxyErr_ks.key();
  m_decorRxyErr_la = m_laKey.key() + m_decorRxyErr_la.key();
  m_decorRxyErr_lb = m_lbKey.key() + m_decorRxyErr_lb.key();
  m_decorPx_v0 = m_v0Key.key() + m_decorPx_v0.key();
  m_decorPx_ks = m_ksKey.key() + m_decorPx_ks.key();
  m_decorPx_la = m_laKey.key() + m_decorPx_la.key();
  m_decorPx_lb = m_lbKey.key() + m_decorPx_lb.key();
  m_decorPy_v0 = m_v0Key.key() + m_decorPy_v0.key();
  m_decorPy_ks = m_ksKey.key() + m_decorPy_ks.key();
  m_decorPy_la = m_laKey.key() + m_decorPy_la.key();
  m_decorPy_lb = m_lbKey.key() + m_decorPy_lb.key();
  m_decorPz_v0 = m_v0Key.key() + m_decorPz_v0.key();
  m_decorPz_ks = m_ksKey.key() + m_decorPz_ks.key();
  m_decorPz_la = m_laKey.key() + m_decorPz_la.key();
  m_decorPz_lb = m_lbKey.key() + m_decorPz_lb.key();

  ATH_CHECK( m_decorPt_v0.initialize() );
  ATH_CHECK( m_decorPt_ks.initialize() );
  ATH_CHECK( m_decorPt_la.initialize() );
  ATH_CHECK( m_decorPt_lb.initialize() );
  ATH_CHECK( m_decorPtErr_v0.initialize() );
  ATH_CHECK( m_decorPtErr_ks.initialize() );
  ATH_CHECK( m_decorPtErr_la.initialize() );
  ATH_CHECK( m_decorPtErr_lb.initialize() );
  ATH_CHECK( m_decorRxy_v0.initialize() );
  ATH_CHECK( m_decorRxy_ks.initialize() );
  ATH_CHECK( m_decorRxy_la.initialize() );
  ATH_CHECK( m_decorRxy_lb.initialize() );
  ATH_CHECK( m_decorRxyErr_v0.initialize() );
  ATH_CHECK( m_decorRxyErr_ks.initialize() );
  ATH_CHECK( m_decorRxyErr_la.initialize() );
  ATH_CHECK( m_decorRxyErr_lb.initialize() );
  ATH_CHECK( m_decorPx_v0.initialize() );
  ATH_CHECK( m_decorPx_ks.initialize() );
  ATH_CHECK( m_decorPx_la.initialize() );
  ATH_CHECK( m_decorPx_lb.initialize() );
  ATH_CHECK( m_decorPy_v0.initialize() );
  ATH_CHECK( m_decorPy_ks.initialize() );
  ATH_CHECK( m_decorPy_la.initialize() );
  ATH_CHECK( m_decorPy_lb.initialize() );
  ATH_CHECK( m_decorPz_v0.initialize() );
  ATH_CHECK( m_decorPz_ks.initialize() );
  ATH_CHECK( m_decorPz_la.initialize() );
  ATH_CHECK( m_decorPz_lb.initialize() );

// uploading the V0Finding tools
  ATH_CHECK( m_v0FinderTool.retrieve() ); 
  msg(MSG::INFO) << "Retrieved tool " << m_v0FinderTool << endmsg;

// uploading the V0 tools
  ATH_CHECK( m_V0Tools.retrieve() );
  msg(MSG::INFO) << "Retrieved tool " << m_V0Tools << endmsg;

// get the Particle Properties Service
  IPartPropSvc* partPropSvc = nullptr;
  ATH_CHECK( service("PartPropSvc", partPropSvc, true) );
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

  m_events_processed++;

// Get primary vertex from StoreGate
  const xAOD::Vertex* primaryVertex = nullptr;
  SG::ReadHandle<xAOD::VertexContainer> importedVxContainer( m_vertexKey );
  if ( !importedVxContainer.isValid() )
  {
    ATH_MSG_WARNING("No xAOD::VertexContainer named " << m_vertexKey.key() << " found in StoreGate");
    return StatusCode::RECOVERABLE;    
  } else {
    ATH_MSG_DEBUG("Found xAOD::VertexContainer named " << m_vertexKey.key() );
  }
  if ( importedVxContainer->empty() ){
    ATH_MSG_WARNING("Primary vertex container is empty.");
  } else {
    primaryVertex = importedVxContainer->front();
  }

// InDetV0 container and its auxilliary store
  xAOD::VertexContainer*    v0Container{};
  xAOD::VertexAuxContainer* v0AuxContainer{};
  xAOD::VertexContainer*    ksContainer{};
  xAOD::VertexAuxContainer* ksAuxContainer{};
  xAOD::VertexContainer*    laContainer{};
  xAOD::VertexAuxContainer* laAuxContainer{};
  xAOD::VertexContainer*    lbContainer{};
  xAOD::VertexAuxContainer* lbAuxContainer{};
					   
	
	//
	const auto statusOfSearch = m_v0FinderTool->performSearch(v0Container, v0AuxContainer,
					   ksContainer, ksAuxContainer,
					   laContainer, laAuxContainer,
             lbContainer, lbAuxContainer,
					   primaryVertex, importedVxContainer.cptr());
	//
	using Container_p = std::unique_ptr<xAOD::VertexContainer>;
	using Aux_p = std::unique_ptr<xAOD::VertexAuxContainer>;
	//create unique pointers for all containers and aux containers
	Container_p pV = Container_p(v0Container);
	Aux_p pVaux = Aux_p(v0AuxContainer);
	//
	Container_p pK = Container_p(ksContainer);
	Aux_p pKaux = Aux_p(ksAuxContainer);
	//
	Container_p pLa = Container_p(laContainer);
	Aux_p pLaaux = Aux_p(laAuxContainer);
	//
	Container_p pLb = Container_p(lbContainer);
	Aux_p pLbaux = Aux_p(lbAuxContainer);
	//
	if (statusOfSearch != StatusCode::SUCCESS){
	  ATH_MSG_ERROR("Vertex search of v0Container failed.");
		return StatusCode::FAILURE;
	}
  //
  //---- Recording section: write the results to StoreGate ---//
  SG::WriteHandle<xAOD::VertexContainer> h_V0( m_v0Key );
  if ( h_V0.record(std::move(pV),std::move(pVaux)) !=StatusCode::SUCCESS){
		ATH_MSG_ERROR("Storegate record of v0Container failed.");
		return StatusCode::FAILURE;
	}

  SG::WriteHandle<xAOD::VertexContainer> h_Ks( m_ksKey );
  if ( h_Ks.record(std::move(pK), std::move(pKaux)) != StatusCode::SUCCESS){
	  ATH_MSG_ERROR("Storegate record of ksContainer failed.");
	  return StatusCode::FAILURE;
	}

  SG::WriteHandle<xAOD::VertexContainer> h_La( m_laKey );
  if( h_La.record(std::move(pLa),std::move(pLaaux)) !=  StatusCode::SUCCESS){
    ATH_MSG_ERROR("Storegate record of laContainer failed.");
	  return StatusCode::FAILURE;

  }
  SG::WriteHandle<xAOD::VertexContainer> h_Lb( m_lbKey );
  if(h_Lb.record(std::move(pLb), std::move(pLbaux)) != StatusCode::SUCCESS){
	  ATH_MSG_ERROR("Storegate record of lbContainer failed.");
	  return StatusCode::FAILURE;
	}

  if (m_decorate) {
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorKsMass(m_decorKsMass);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorLaMass(m_decorLaMass);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorLbMass(m_decorLbMass);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorKsMassErr(m_decorKsMassErr);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorLaMassErr(m_decorLaMassErr);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorLbMassErr(m_decorLbMassErr);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPt_v0(m_decorPt_v0);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPtErr_v0(m_decorPtErr_v0);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorRxy_v0(m_decorRxy_v0);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorRxyErr_v0(m_decorRxyErr_v0);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPx_v0(m_decorPx_v0);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPy_v0(m_decorPy_v0);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPz_v0(m_decorPz_v0);

    for ( auto unconstrV0 : *h_V0 )
    {
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

      decorKsMass( *unconstrV0 ) = mass_ks;
      decorLaMass( *unconstrV0 ) = mass_la;
      decorLbMass( *unconstrV0 ) = mass_lb;
      decorKsMassErr( *unconstrV0 ) = mass_error_ks;
      decorLaMassErr( *unconstrV0 ) = mass_error_la;
      decorLbMassErr( *unconstrV0 ) = mass_error_lb;
      decorPt_v0( *unconstrV0 ) = pt;
      decorPtErr_v0( *unconstrV0 ) =ptError;
      decorRxy_v0( *unconstrV0 ) = rxy;
      decorRxyErr_v0( *unconstrV0 ) =rxyError;
      decorPx_v0( *unconstrV0 ) = momentum.x();
      decorPy_v0( *unconstrV0 ) = momentum.y();
      decorPz_v0( *unconstrV0 ) = momentum.z();
    }

    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMass_ks(m_decorMass_ks);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMassErr_ks(m_decorMassErr_ks);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPt_ks(m_decorPt_ks);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPtErr_ks(m_decorPtErr_ks);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorRxy_ks(m_decorRxy_ks);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorRxyErr_ks(m_decorRxyErr_ks);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPx_ks(m_decorPx_ks);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPy_ks(m_decorPy_ks);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPz_ks(m_decorPz_ks);

    for ( auto ksV0 : *h_Ks )
    {
      double mass_ks = m_V0Tools->invariantMass(ksV0,m_masspi,m_masspi);
      double mass_error_ks = m_V0Tools->invariantMassError(ksV0,m_masspi,m_masspi);
      double pt = m_V0Tools->pT(ksV0);
      double ptError = m_V0Tools->pTError(ksV0);
      double rxy = m_V0Tools->rxy(ksV0);
      double rxyError = m_V0Tools->rxyError(ksV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(ksV0);

      decorMass_ks( *ksV0 ) = mass_ks;
      decorMassErr_ks( *ksV0 ) = mass_error_ks;
      decorPt_ks( *ksV0 ) = pt;
      decorPtErr_ks( *ksV0 ) = ptError;
      decorRxy_ks( *ksV0 ) = rxy;
      decorRxyErr_ks( *ksV0 ) = rxyError;
      decorPx_ks( *ksV0 ) = momentum.x();
      decorPy_ks( *ksV0 ) = momentum.y();
      decorPz_ks( *ksV0 ) = momentum.z();
    }

    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMass_la(m_decorMass_la);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMassErr_la(m_decorMassErr_la);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPt_la(m_decorPt_la);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPtErr_la(m_decorPtErr_la);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorRxy_la(m_decorRxy_la);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorRxyErr_la(m_decorRxyErr_la);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPx_la(m_decorPx_la);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPy_la(m_decorPy_la);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPz_la(m_decorPz_la);

    for ( auto laV0 : *h_La )
    {
      double mass_la = m_V0Tools->invariantMass(laV0,m_massp,m_masspi);
      double mass_error_la = m_V0Tools->invariantMassError(laV0,m_massp,m_masspi);
      double pt = m_V0Tools->pT(laV0);
      double ptError = m_V0Tools->pTError(laV0);
      double rxy = m_V0Tools->rxy(laV0);
      double rxyError = m_V0Tools->rxyError(laV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(laV0);

      decorMass_la( *laV0 ) = mass_la;
      decorMassErr_la( *laV0 ) = mass_error_la;
      decorPt_la( *laV0 ) = pt;
      decorPtErr_la( *laV0 ) = ptError;
      decorRxy_la( *laV0 ) = rxy;
      decorRxyErr_la( *laV0 ) = rxyError;
      decorPx_la( *laV0 ) = momentum.x();
      decorPy_la( *laV0 ) = momentum.y();
      decorPz_la( *laV0 ) = momentum.z();
    }

    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMass_lb(m_decorMass_lb);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMassErr_lb(m_decorMassErr_lb);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPt_lb(m_decorPt_lb);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPtErr_lb(m_decorPtErr_lb);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorRxy_lb(m_decorRxy_lb);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorRxyErr_lb(m_decorRxyErr_lb);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPx_lb(m_decorPx_lb);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPy_lb(m_decorPy_lb);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorPz_lb(m_decorPz_lb);

    for ( auto lbV0 : *h_Lb )
    {
      double mass_lb = m_V0Tools->invariantMass(lbV0,m_masspi,m_massp);
      double mass_error_lb = m_V0Tools->invariantMassError(lbV0,m_masspi,m_massp);
      double pt = m_V0Tools->pT(lbV0);
      double ptError = m_V0Tools->pTError(lbV0);
      double rxy = m_V0Tools->rxy(lbV0);
      double rxyError = m_V0Tools->rxyError(lbV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(lbV0);

      decorMass_lb( *lbV0 ) = mass_lb;
      decorMassErr_lb( *lbV0 ) = mass_error_lb;
      decorPt_lb( *lbV0 ) = pt;
      decorPtErr_lb( *lbV0 ) = ptError;
      decorRxy_lb( *lbV0 ) = rxy;
      decorRxyErr_lb( *lbV0 ) = rxyError;
      decorPx_lb( *lbV0 ) = momentum.x();
      decorPy_lb( *lbV0 ) = momentum.y();
      decorPz_lb( *lbV0 ) = momentum.z();
    }
  }

  m_V0s_stored += h_V0->size();
  m_Kshort_stored += h_Ks->size();
  m_Lambda_stored += h_La->size();
  m_Lambdabar_stored += h_Lb->size();

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

