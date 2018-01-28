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
#include "StoreGate/WriteDecorHandle.h"

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
  StatusCode sc;

  ATH_CHECK( resetStatistics() );

  ATH_CHECK( m_vertexKey.initialize() );
  ATH_CHECK( m_v0Key.initialize() );
  ATH_CHECK( m_ksKey.initialize() );
  ATH_CHECK( m_laKey.initialize() );
  ATH_CHECK( m_lbKey.initialize() );

  // Make sure decoration keys match the VertexContainer name (which can in principle be changed)
  m_decorKsMass = m_v0Key.key() + m_decorKsMass.key();
  m_decorLaMass = m_v0Key.key() + m_decorLaMass.key();
  m_decorLbMass = m_v0Key.key() + m_decorLaMass.key();

  ATH_CHECK( m_decorKsMass.initialize() );
  ATH_CHECK( m_decorLaMass.initialize() );
  ATH_CHECK( m_decorLbMass.initialize() );

  m_decorMass_ks = m_ksKey.key() + m_decorMass_ks.key();

  ATH_CHECK( m_decorMass_ks.initialize() );

  m_decorMass_la = m_laKey.key() + m_decorMass_la.key();

  ATH_CHECK( m_decorMass_la.initialize() );

  m_decorMass_lb = m_lbKey.key() + m_decorMass_lb.key();

  ATH_CHECK( m_decorMass_lb.initialize() );

// uploading the V0Finding tools
  ATH_CHECK( m_v0FinderTool.retrieve() ); 
  msg(MSG::INFO) << "Retrieved tool " << m_v0FinderTool << endmsg;

// uploading the V0 tools
  ATH_CHECK( m_V0Tools.retrieve() );
  msg(MSG::INFO) << "Retrieved tool " << m_V0Tools << endmsg;

// get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
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
  StatusCode sc;

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
  xAOD::VertexContainer*    v0Container(0);
  xAOD::VertexAuxContainer* v0AuxContainer(0);
  xAOD::VertexContainer*    ksContainer(0);
  xAOD::VertexAuxContainer* ksAuxContainer(0);
  xAOD::VertexContainer*    laContainer(0);
  xAOD::VertexAuxContainer* laAuxContainer(0);
  xAOD::VertexContainer*    lbContainer(0);
  xAOD::VertexAuxContainer* lbAuxContainer(0);

// call InDetV0 finder
  ATH_CHECK( m_v0FinderTool->performSearch(v0Container, v0AuxContainer,
					   ksContainer, ksAuxContainer,
					   laContainer, laAuxContainer,
                                           lbContainer, lbAuxContainer,
					   primaryVertex, importedVxContainer) );

  //---- Recording section: write the results to StoreGate ---//

  SG::WriteHandle<xAOD::VertexContainer> h_V0( m_v0Key );
  ATH_CHECK( h_V0.record(std::unique_ptr<xAOD::VertexContainer>(v0Container), 
			 std::unique_ptr<xAOD::VertexAuxContainer>(v0AuxContainer)) );

  SG::WriteHandle<xAOD::VertexContainer> h_Ks( m_ksKey );
  ATH_CHECK( h_Ks.record(std::unique_ptr<xAOD::VertexContainer>(ksContainer), 
			 std::unique_ptr<xAOD::VertexAuxContainer>(ksAuxContainer)) );

  SG::WriteHandle<xAOD::VertexContainer> h_La( m_laKey );
  ATH_CHECK( h_La.record(std::unique_ptr<xAOD::VertexContainer>(laContainer), 
			 std::unique_ptr<xAOD::VertexAuxContainer>(laAuxContainer)) );

  SG::WriteHandle<xAOD::VertexContainer> h_Lb( m_lbKey );
  ATH_CHECK(h_Lb.record(std::unique_ptr<xAOD::VertexContainer>(lbContainer), 
			std::unique_ptr<xAOD::VertexAuxContainer>(lbAuxContainer)) );

  if (m_decorate) {
    /*
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
    */
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorKsMass(m_decorKsMass);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorLaMass(m_decorLaMass);
    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorLbMass(m_decorLbMass);

    for ( auto unconstrV0 : *h_V0 )
    {
      double mass_ks = m_V0Tools->invariantMass(unconstrV0,m_masspi,m_masspi);
      //double mass_error_ks = m_V0Tools->invariantMassError(unconstrV0,m_masspi,m_masspi);
      double mass_la = m_V0Tools->invariantMass(unconstrV0,m_massp,m_masspi);
      //double mass_error_la = m_V0Tools->invariantMassError(unconstrV0,m_massp,m_masspi);
      double mass_lb = m_V0Tools->invariantMass(unconstrV0,m_masspi,m_massp);
      /*
      double mass_error_lb = m_V0Tools->invariantMassError(unconstrV0,m_masspi,m_massp);
      double pt = m_V0Tools->pT(unconstrV0);
      double ptError = m_V0Tools->pTError(unconstrV0);
      double rxy = m_V0Tools->rxy(unconstrV0);
      double rxyError = m_V0Tools->rxyError(unconstrV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(unconstrV0);
      */
      decorKsMass( *unconstrV0 ) = mass_ks;
      decorLaMass( *unconstrV0 ) = mass_la;
      decorLbMass( *unconstrV0 ) = mass_lb;
      /*
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
      */
    }

    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMass_ks(m_decorMass_ks);

    for ( auto ksV0 : *h_Ks )
    {
      double mass_ks = m_V0Tools->invariantMass(ksV0,m_masspi,m_masspi);
      /*
      double mass_error_ks = m_V0Tools->invariantMassError(ksV0,m_masspi,m_masspi);
      double pt = m_V0Tools->pT(ksV0);
      double ptError = m_V0Tools->pTError(ksV0);
      double rxy = m_V0Tools->rxy(ksV0);
      double rxyError = m_V0Tools->rxyError(ksV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(ksV0);
      */
      decorMass_ks( *ksV0 ) = mass_ks;
      /*
      mDecor_mass( *ksV0 ) = mass_ks;
      mDecor_massError( *ksV0 ) = mass_error_ks;
      mDecor_pt( *ksV0 ) = pt;
      mDecor_ptError( *ksV0 ) = ptError;
      mDecor_rxy( *ksV0 ) = rxy;
      mDecor_rxyError( *ksV0 ) = rxyError;
      mDecor_px( *ksV0 ) = momentum.x();
      mDecor_py( *ksV0 ) = momentum.y();
      mDecor_pz( *ksV0 ) = momentum.z();
      */
    }

    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMass_la(m_decorMass_la);

    for ( auto laV0 : *h_La )
    {
      double mass_la = m_V0Tools->invariantMass(laV0,m_massp,m_masspi);
      /*
      double mass_error_la = m_V0Tools->invariantMassError(laV0,m_massp,m_masspi);
      double pt = m_V0Tools->pT(laV0);
      double ptError = m_V0Tools->pTError(laV0);
      double rxy = m_V0Tools->rxy(laV0);
      double rxyError = m_V0Tools->rxyError(laV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(laV0);
      */
      decorMass_la( *laV0 ) = mass_la;
      /*
      mDecor_mass( *laV0 ) = mass_la;
      mDecor_massError( *laV0 ) = mass_error_la;
      mDecor_pt( *laV0 ) = pt;
      mDecor_ptError( *laV0 ) = ptError;
      mDecor_rxy( *laV0 ) = rxy;
      mDecor_rxyError( *laV0 ) = rxyError;
      mDecor_px( *laV0 ) = momentum.x();
      mDecor_py( *laV0 ) = momentum.y();
      mDecor_pz( *laV0 ) = momentum.z();
      */
    }

    SG::WriteDecorHandle<xAOD::VertexContainer, float> decorMass_lb(m_decorMass_lb);

    for ( auto lbV0 : *h_Lb )
    {
      double mass_lb = m_V0Tools->invariantMass(lbV0,m_masspi,m_massp);
      /*
      double mass_error_lb = m_V0Tools->invariantMassError(lbV0,m_masspi,m_massp);
      double pt = m_V0Tools->pT(lbV0);
      double ptError = m_V0Tools->pTError(lbV0);
      double rxy = m_V0Tools->rxy(lbV0);
      double rxyError = m_V0Tools->rxyError(lbV0);
      Amg::Vector3D momentum = m_V0Tools->V0Momentum(lbV0);
      */
      decorMass_lb( *lbV0 ) = mass_lb;

      /*
      mDecor_mass( *lbV0 ) = mass_lb;
      mDecor_massError( *lbV0 ) = mass_error_lb;
      mDecor_pt( *lbV0 ) = pt;
      mDecor_ptError( *lbV0 ) = ptError;
      mDecor_rxy( *lbV0 ) = rxy;
      mDecor_rxyError( *lbV0 ) = rxyError;
      mDecor_px( *lbV0 ) = momentum.x();
      mDecor_py( *lbV0 ) = momentum.y();
      mDecor_pz( *lbV0 ) = momentum.z();
      */
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

