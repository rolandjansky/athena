/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):


#include "xAODEgamma/ElectronContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "ZMassConstraint/ConstraintFit.h"

// local include
#include "TestAthenaConstraintFit.h"

TestAthenaConstraintFit::TestAthenaConstraintFit( const std::string& name, ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_constraintFit("ZMassConstraint::ConstraintFit/ConstraintFit", this)
{
  declareProperty( "ElectronContName",      m_electronContName = "ElectronCollection" );
  declareProperty( "PrimaryVertexContName", m_vertexContName = "PrimaryVertices" );
  declareProperty( "ConstraintFit",         m_constraintFit);
}


StatusCode TestAthenaConstraintFit::initialize()
{
  // Greet the user:
  ATH_MSG_INFO( "Initialising..." );

  // Retrieve the tools:
  ATH_CHECK(m_constraintFit.retrieve());

  return StatusCode::SUCCESS;
}


StatusCode TestAthenaConstraintFit::execute()
{
  const xAOD::EventInfo* ei = 0;
  ATH_CHECK( evtStore()->retrieve(ei) );

  const xAOD::ElectronContainer* electrons;
  ATH_CHECK( evtStore()->retrieve(electrons, m_electronContName) );


  // get primary vertex
  const xAOD::VertexContainer* vtxs = 0;
  ATH_CHECK( evtStore()->retrieve( vtxs, m_vertexContName ) );

  const xAOD::Vertex*  primaryVtx = 0;
  for ( auto vtx : *vtxs) if (vtx->vertexType() == xAOD::VxType::PriVtx) primaryVtx = vtx;

  
  std::vector<const xAOD::Electron*> els;
  els.reserve(electrons->size());

  ATH_MSG_INFO("run,evt,nels  " << ei->runNumber() << "," << ei->eventNumber() << "," << electrons->size());

  for ( auto el : *electrons ) {

      ATH_MSG_INFO("elpt/eta/phi  " << el->pt() << "," << el->eta() << "," << el->phi());

      const xAOD::TrackParticle* el_tp = el->trackParticle();

      if (fabs(el->caloCluster()->eta()) >= 2.47) continue;
      if (el->pt() <= 7000.) continue;

      float d0 = el_tp->d0();  // no smearing for run 2 yet...
      if (primaryVtx) d0 = (el_tp->z0() - primaryVtx->z() + el_tp->vz());
      
      if (fabs(d0) > 10.) continue;

      els.push_back(el);
  }

  ATH_MSG_INFO("nels passed cuts  " << els.size());
  
  
  if (els.size() > 1) {
      // fit only the first two electrons and only if they have opposite charge
      const xAOD::Electron* el0 = els[0];
      const xAOD::Electron* el1 = els[1];
      
      ZMassConstraint::ConstraintFitInput  input;
      ZMassConstraint::ConstraintFitInput  emptyInput;
      ZMassConstraint::ConstraintFitOutput result;
      if (el0->charge() != el1->charge()) {
          float res = el0->pt() * 0.1 / sqrt(el0->e());
          m_constraintFit->addParticle(*el0, res, input);
          res = el1->pt() * 0.1 / sqrt(el1->e());
          m_constraintFit->addParticle(*el1, res, input);
          if (m_constraintFit->doMassFit(input, result).isFailure()) {
              ATH_MSG_DEBUG("execute: Unable to do mass contrained fit");
            }
          ATH_MSG_INFO("el0 pt,eta,phi " << el0->pt()/1000. << "/" << el0->eta() << "/" << el0->phi()); 
          ATH_MSG_INFO("el1 pt,eta,phi " << el1->pt()/1000. << "/" << el1->eta() << "/" << el1->phi());
          TLorentzVector fit4vec;
          result.getCompositeFourVector(fit4vec);
          
          ATH_MSG_INFO("mass, error "    << fit4vec.M()/1000. << "/"
                       << m_constraintFit->getMassError(result, emptyInput)/1000.);
          
      }
      else ATH_MSG_INFO("not opposite charge  ");

  }

  return StatusCode::SUCCESS;
}

