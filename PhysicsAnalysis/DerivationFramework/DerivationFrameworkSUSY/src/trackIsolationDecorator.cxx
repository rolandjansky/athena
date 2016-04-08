/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// trackIsolationDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Christopher Young (christopher.young@cern.ch)
#include "DerivationFrameworkSUSY/trackIsolationDecorator.h"
#include "AthenaKernel/errorcheck.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"

// Constructor
DerivationFramework::trackIsolationDecorator::trackIsolationDecorator(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  AthAlgTool(t, n, p),
  m_trackIsolationTool(),
  m_decorators(xAOD::Iso::numIsolationTypes, 0)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("TrackIsolationTool", m_trackIsolationTool);
  declareProperty("TargetContainer",    m_containerName = "InDetTrackParticles");
  declareProperty("ptcones",            m_ptcones);
  declareProperty("Prefix",             m_prefix="");
}
  
// Destructor
DerivationFramework::trackIsolationDecorator::~trackIsolationDecorator() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::trackIsolationDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  // load the matching tool
  if( ! m_trackIsolationTool.empty() ) {
     CHECK( m_trackIsolationTool.retrieve() );
     ATH_MSG_INFO( "Successfully retrived the TrackIsolationTool!" );
  }

  m_trkCorrList.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));

  /// create decorator list
  m_ptconeTypes.clear();
  for(auto c: m_ptcones){
    xAOD::Iso::IsolationType t = static_cast<xAOD::Iso::IsolationType>(c);
    m_decorators[c] = new SG::AuxElement::Decorator< float >(m_prefix+xAOD::Iso::toString(t));
    m_ptconeTypes.push_back(t);
  }

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::trackIsolationDecorator::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");

  /// delete the decorators
  for(auto d: m_decorators) {if(d) delete d;}

  /// proxyLoaders and SGxAODProxyLoader are not deleted.

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::trackIsolationDecorator::addBranches() const
{
  // retrieve container
   const xAOD::IParticleContainer* toDecorate = 0;
   if(evtStore()->retrieve(toDecorate, m_containerName).isFailure()) {
     ATH_MSG_FATAL( "Unable to retrieve " << m_containerName );
     return StatusCode::FAILURE;
   }

  /// Loop over tracks
  for(auto particle : *toDecorate) {

    /// track isolation
    xAOD::TrackIsolation resultTrack;
    if (m_trackIsolationTool->trackIsolation(resultTrack, *particle, m_ptconeTypes, m_trkCorrList)){
        for(unsigned int i=0; i<m_ptcones.size(); i++){
//           (*(m_decorators[static_cast<int>(m_ptcones[i])]))(*particle) = resultTrack.ptcones.at(i);
          (*(m_decorators[m_ptcones[i]]))(*particle) = resultTrack.ptcones.at(i);
        }
    }else{
        ATH_MSG_WARNING("Failed to apply the track isolation for a particle");
    }

  } 

  return StatusCode::SUCCESS;
}
