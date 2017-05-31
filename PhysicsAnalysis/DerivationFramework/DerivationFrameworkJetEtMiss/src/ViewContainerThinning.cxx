///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ViewContainerThinning.cxx 
// Implementation file for class ViewContainerThinning
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// DerivationFrameworkJetEtMiss includes
#include "ViewContainerThinning.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"



namespace DerivationFramework {

  // Constructors
  ////////////////
  ViewContainerThinning::ViewContainerThinning( const std::string& type, 
                                                const std::string& name, 
                                                const IInterface* parent ) : 
    ::AthAlgTool  ( type, name, parent   )
    , m_thinningSvc("ThinningSvc",name)
  {
    declareInterface< IThinningTool >(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty( "SourceContainer", m_sourceContName );
    declareProperty( "ViewContainer", m_viewContName );
    declareProperty( "ParticleType", m_particleType );
    declareProperty( "ApplyAnd", m_and = false );

  }

  // Destructor
  ///////////////
  ViewContainerThinning::~ViewContainerThinning()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode ViewContainerThinning::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode ViewContainerThinning::finalize()
  {

    return StatusCode::SUCCESS;
  }



  StatusCode ViewContainerThinning::doThinning() const {

    switch ( (xAOD::Type::ObjectType) m_particleType) {
    case xAOD::Type::TrackParticle : return doThinningT<xAOD::TrackParticleContainer>() ;
    case xAOD::Type::CaloCluster : return doThinningT<xAOD::CaloClusterContainer>() ;
    case xAOD::Type::TruthParticle : return doThinningT<xAOD::TruthParticleContainer>() ;
    default:{
      ATH_MSG_ERROR(" Unkown particle type for containers "<< m_sourceContName << " and "<< m_viewContName <<"... type was"<< m_particleType);
      return StatusCode::SUCCESS;
    }

    }
    return StatusCode::SUCCESS;

  }

}
