/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloExtensionTool.h"
// forward declares
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrack/Track.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"

#include "xAODTracking/TrackingPrimitives.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkEventPrimitives/PropDirection.h" 
#include "TrkEventPrimitives/CurvilinearUVT.h"
#include "TrkEventPrimitives/JacobianLocalToCurvilinear.h"

namespace Trk
{
  ParticleCaloExtensionTool::ParticleCaloExtensionTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
      m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
      m_magFieldSvc("AtlasFieldSvc", n),
      m_particleType(Trk::muon)
  {

    declareInterface<IParticleCaloExtensionTool>(this);
    declareProperty("Extrapolator",   m_extrapolator );
    declareProperty("MagFieldSvc",    m_magFieldSvc);
    declareProperty("ParticleType",   m_particleTypeName = "muon" );
    declareProperty("OutputContainerName", m_containerName = n );
  }
  
  ParticleCaloExtensionTool::~ParticleCaloExtensionTool() {}
  
  StatusCode ParticleCaloExtensionTool::initialize() {
    /* Retrieve track extrapolator from ToolService */
    ATH_CHECK(detStore()->retrieve(m_detID, "AtlasID" ));
    ATH_CHECK( m_extrapolator.retrieve() );
    ATH_CHECK( m_magFieldSvc.retrieve() );

    if( m_particleTypeName == "nonInteracting" ) m_particleType = Trk::nonInteracting;
    else if( m_particleTypeName == "muon" )      m_particleType = Trk::muon;
    else if( m_particleTypeName == "pion" )      m_particleType = Trk::pion;
    else ATH_MSG_WARNING("Unsupported particle type, using muon " << m_particleTypeName );
    ATH_MSG_INFO(" Using particle type " << m_particleTypeName << " enum value " << m_particleType );
    return StatusCode::SUCCESS;
  }
  
  StatusCode ParticleCaloExtensionTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const xAOD::ParticleCaloExtension* ParticleCaloExtensionTool::caloExtension( xAOD::TrackParticle& trackParticle ) const {
    
    // check if link is already there
    if( trackParticle.isAvailable< xAOD::ParticleCaloExtension* >("caloExtension") ){
      xAOD::ParticleCaloExtension* extension = trackParticle.auxdata< xAOD::ParticleCaloExtension* >("caloExtension");
      if( extension ){
        ATH_MSG_DEBUG("Found existing calo extension");
        return trackParticle.auxdata< xAOD::ParticleCaloExtension* >("caloExtension");
      }
    }
    
    // if not create it
    xAOD::ParticleCaloExtension*& caloExtension = trackParticle.auxdata< xAOD::ParticleCaloExtension* >("caloExtension");

    // check if there is a track
    if( !trackParticle.track() ){
      ATH_MSG_WARNING("No valid track, failing extension" );
      return 0;
    }
    const Trk::Track& track = *trackParticle.track();

    // look-up the parameters closest to the calorimeter in ID and muon system
    ATH_MSG_DEBUG("trying to add calo layers" );
    const TrackParameters* idExitParamers = 0;
    const TrackParameters* muonEntryParamers = 0;
    DataVector<const TrackStateOnSurface>::const_iterator itTSoS = track.trackStateOnSurfaces()->begin();
    for ( ; itTSoS != track.trackStateOnSurfaces()->end(); ++itTSoS) {
      // select state with track parameters on a measurement
      if ( !(**itTSoS).trackParameters() || !(**itTSoS).type(TrackStateOnSurface::Measurement) || (**itTSoS).type(TrackStateOnSurface::Outlier) ) continue;
      const Identifier& id = (**itTSoS).trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
      if( m_detID->is_indet(id) ) idExitParamers = (**itTSoS).trackParameters();
      if( m_detID->is_muon(id) && !muonEntryParamers ) muonEntryParamers = (**itTSoS).trackParameters();
    }
      
    // require at least one of them
    if( !idExitParamers && !muonEntryParamers) {
      ATH_MSG_WARNING("Failed to find ID and Muon parameters");
      return 0;
    }

    // pick start parameters, start in ID if possible
    const TrackParameters* startPars = idExitParamers ? idExitParamers : muonEntryParamers;
    PropDirection propDir = idExitParamers ? alongMomentum : oppositeMomentum;

    ATH_MSG_DEBUG("looking up calo states: r " << startPars->position().perp() << " z " << startPars->position().z()
                  << " momentum " << startPars->momentum().mag() );

    // pointers to hold results and go
    std::vector<const TrackStateOnSurface*>* material = 0;
    const std::vector< std::pair< const TrackParameters*, int > >* caloParameters = 
      m_extrapolator->extrapolate( *startPars, propDir, m_particleType, material, 3 ); 
    if( !caloParameters ) return 0;

    // create final object
    std::vector< std::vector<float> > parameters;
    std::vector< std::vector<float> > parametersCovariance;
    std::vector< int >       identifiers;
    parameters.reserve(caloParameters->size());
    parametersCovariance.reserve(caloParameters->size());
    ATH_MSG_DEBUG( " Found calo parameters: " << caloParameters->size() );
    for( const auto& p : *caloParameters ){

      const Trk::TrackParameters* param = p.first;
      if( !param ) continue;
      ATH_MSG_DEBUG( " id " << p.second << " pos: r " << param->position().perp() << " z " << param->position().z() 
                     << " momentum " << param->momentum().perp() << " cov " << param->covariance() );

      std::vector<float> pars(6);
      pars[0] = param->position().x();
      pars[1] = param->position().y();
      pars[2] = param->position().z();
      pars[3] = param->momentum().x();
      pars[4] = param->momentum().y();
      pars[5] = param->momentum().z();
      parameters.push_back(pars);
      identifiers.push_back(p.second);

      std::vector<float> covVec;
      if( param->covariance() ){
        //now convert from to Curvilinear -- to be double checked for correctness
        Amg::Vector3D magnFieldVect; magnFieldVect.setZero();
        m_magFieldSvc->getField( &param->position(), &magnFieldVect );

        Trk::CurvilinearUVT curvilinearUVT(param->momentum());     
        const Amg::Transform3D& localToGlobalTransform = param->associatedSurface().transform();
     
        Trk::JacobianLocalToCurvilinear jacobian(magnFieldVect,
                                                 param->parameters()[Trk::qOverP],
                                                 sin(param->parameters()[Trk::theta]),
                                                 curvilinearUVT,
                                                 localToGlobalTransform.rotation().col(0),
                                                 localToGlobalTransform.rotation().col(1));     
     
        AmgSymMatrix(5) covarianceMatrix = param->covariance()->similarity(jacobian);
        Amg::compress(covarianceMatrix,covVec);
      }
      parametersCovariance.push_back(covVec);
      delete param;
    }
    
    // clean-up memory
    delete caloParameters;
    if( material ) {
      for( auto& m : *material ) delete m;
      delete material;
    }
    caloExtension = new xAOD::ParticleCaloExtension(trackParticle.charge(),std::move(parameters),
                                                    std::move(parametersCovariance),std::move(identifiers));

    // now add the extension to the output collection so we are not causing any leaks
    ParticleCaloExtensionCollection* collection = 0;
    if( !evtStore()->contains<ParticleCaloExtensionCollection>(m_containerName) ){
      collection = new ParticleCaloExtensionCollection();
      if( evtStore()->record( collection, m_containerName).isFailure() ) {
        ATH_MSG_WARNING( "Failed to record output collection, will leak the ParticleCaloExtension");
        delete collection;
        collection = 0;
      }
    }else{
      if(evtStore()->retrieve(collection,m_containerName).isFailure()) {
        ATH_MSG_WARNING( "Unable to retrieve " << m_containerName << " will leak the ParticleCaloExtension" );
      }
    }
    if( collection ) collection->push_back(caloExtension);

    return caloExtension;
  }

  
} // end of namespace Trk
