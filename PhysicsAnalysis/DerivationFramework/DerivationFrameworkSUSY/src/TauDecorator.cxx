/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkSUSY/TauDecorator.h"
#include "xAODTau/TauJetContainer.h"

DerivationFramework::TauDecorator::TauDecorator(const std::string& t,
						const std::string& n,
						const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_tauContainerName("TauJets")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("TauContainerName", m_tauContainerName);
}

DerivationFramework::TauDecorator::~TauDecorator() {
}

StatusCode DerivationFramework::TauDecorator::initialize()
{
  if (m_tauContainerName.empty()) {
    ATH_MSG_ERROR("The TauContainerName property should be specified. Aborting...");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TauDecorator::finalize()
{
  return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::TauDecorator::addBranches() const
{
  const xAOD::TauJetContainer* taus = 0;
  if (evtStore()->retrieve(taus,m_tauContainerName).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve " << m_tauContainerName << " collection. Aborting...");
    return StatusCode::FAILURE;
  }
  
  static const SG::AuxElement::Decorator< float > dec( "seedJetWidth" );
  
  for( const xAOD::TauJet* tau : *taus ) {
    const float width = tau->jetLink().isValid() ? tau->jet()->auxdataConst<float>( "Width" ) : -1;
    dec( *tau ) = width;
  }
  
  return StatusCode::SUCCESS;
}
