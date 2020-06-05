/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloExtensionTool.h"
// forward declares
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#define private public
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#define public private 
#include "xAODTracking/TrackingPrimitives.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODMuon/Muon.h"

namespace Trk
{
  ParticleCaloExtensionTool::ParticleCaloExtensionTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
      m_detID(nullptr),
      m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
      m_particleType(muon)
  {

    declareInterface<IParticleCaloExtensionTool>(this);
    declareProperty("Extrapolator",   m_extrapolator );
    declareProperty("ParticleType",   m_particleTypeName = "muon" );
    declareProperty("OutputContainerName", m_containerName = n );
    declareProperty("StartFromPerigee", m_startFromPerigee = false);
  }
  
  ParticleCaloExtensionTool::~ParticleCaloExtensionTool() {}
  
  StatusCode ParticleCaloExtensionTool::initialize() {
    /* Retrieve track extrapolator from ToolService */
    ATH_CHECK(detStore()->retrieve(m_detID, "AtlasID" ));
    ATH_CHECK( m_extrapolator.retrieve() );

    if( m_particleTypeName == "nonInteracting" ) m_particleType = nonInteracting;
    else if( m_particleTypeName == "muon" )      m_particleType = muon;
    else if( m_particleTypeName == "pion" )      m_particleType = pion;
    else ATH_MSG_WARNING("Unsupported particle type, using muon " << m_particleTypeName );
    ATH_MSG_INFO(" Using particle type " << m_particleTypeName << " enum value " << m_particleType );
    return StatusCode::SUCCESS;
  }
  
  StatusCode ParticleCaloExtensionTool::finalize() {
    return StatusCode::SUCCESS;
  }

  const xAOD::TrackParticle* ParticleCaloExtensionTool::getTrackParticle(const xAOD::IParticle& particle ) const {

    const xAOD::TrackParticle* trackParticle = dynamic_cast< const xAOD::TrackParticle*>(&particle);
    if( trackParticle ) return trackParticle;
 
    const xAOD::Muon* muon = dynamic_cast< const xAOD::Muon*>(&particle);
    if( muon && muon->primaryTrackParticle() ) return muon->primaryTrackParticle();
    return nullptr;
  }

  bool ParticleCaloExtensionTool::caloExtension( const xAOD::IParticle& particle, const Trk::CaloExtension*& extension, 
                                                 bool useCaching ) const {
    
    // reset input
    extension = 0;

    ATH_MSG_DEBUG(" caloExtension: ptr " << &particle << " useCaching " << useCaching);

    // check if link is already there
    if( useCaching && particle.isAvailable< CaloExtension* >("caloExtension") ){
      extension = particle.auxdata< CaloExtension* >("caloExtension");
      if( extension ){
        ATH_MSG_DEBUG("Found existing calo extension");
        return true;
      }
    }

    // work out the type of particle and get the extension
    CaloExtension* theExtension = 0;
    const xAOD::TrackParticle* trackParticle = getTrackParticle(particle);
    if( trackParticle ) theExtension = caloExtension(*trackParticle);
    else{
      const xAOD::NeutralParticle* neutralParticle = dynamic_cast< const xAOD::NeutralParticle*>(&particle);
      if( neutralParticle ) theExtension = caloExtension(*neutralParticle);
      else{
        const xAOD::TruthParticle* truthParticle = dynamic_cast< const xAOD::TruthParticle*>(&particle);
        if( truthParticle ) theExtension = caloExtension(*truthParticle);
        else{
          ATH_MSG_WARNING("Unsupported IParticle type");
          return false;
        }
      }
    }

    // create and assign extension dressing 
    if( useCaching ) particle.auxdecor< CaloExtension* >("caloExtension") = theExtension;
    extension = theExtension;

    // return false is extension failed
    return extension != nullptr;
  }

  Trk::CaloExtension* ParticleCaloExtensionTool::caloExtension( const xAOD::TruthParticle& particle ) const {

    // get particle type
    ParticleHypothesis                     particleType = muon; //nonInteracting;
    if( abs(particle.pdgId()) == 11 )      particleType = muon; // we dont want the electron to loose energy when extrpolating in the calo
    else if( abs(particle.pdgId()) == 13 ) particleType = muon;

    // get start parameters
    const xAOD::TruthVertex* pvtx = particle.prodVtx();
    if ( pvtx == 0 ) return nullptr;

    double charge = particle.charge();
    Amg::Vector3D  pos( pvtx->x() , pvtx->y() , pvtx->z() );
    Amg::Vector3D  mom( particle.px() , particle.py() , particle.pz() );

    // hack, extrapolate neutral particles as infinit momentum for the time being
    if( particle.isNeutral() ){
      charge = 1.;
      mom.normalize();
      mom *= 1e10;
    }
    Trk::CurvilinearParameters startPars(pos,mom,charge);

    // get extension
    return caloExtension( startPars, alongMomentum, particleType );
  }

  Trk::CaloExtension* ParticleCaloExtensionTool::caloExtension( const xAOD::NeutralParticle& particle ) const {

    // create start parameters
    const Trk::NeutralPerigee& perigee = particle.perigeeParameters();
    
    double charge = 1.;
    Amg::Vector3D  pos( perigee.position() );
    Amg::Vector3D  mom( perigee.momentum() );
    // hack, extrapolate neutral particles as infinit momentum for the time being
    mom.normalize();
    mom *= 1e10;
    Trk::CurvilinearParameters startPars(pos,mom,charge);

    // get extension
    return caloExtension( startPars, alongMomentum, muon );
  }

  Trk::CaloExtension* ParticleCaloExtensionTool::caloExtension( const xAOD::TrackParticle& particle ) const {

    //Determine if the track was fit electron hypothesis -- so extrapolate as if the particles is non interacting
    ParticleHypothesis particleType = m_particleType;
    if( particle.particleHypothesis() ==  xAOD::electron )
    { 
      ATH_MSG_DEBUG("Fitting using electron hypothesis");
      particleType = muon;//nonInteracting;
      
      if(!m_startFromPerigee){
        unsigned int index(0);
        if (!particle.indexOfParameterAtPosition(index, xAOD::LastMeasurement))        {    
          ATH_MSG_WARNING("No TrackParticle or no have first measurement");
        } else {
          return caloExtension(particle.curvilinearParameters(index),alongMomentum,particleType);
        }
      }
    }

    if(m_startFromPerigee || !particle.track())
    {
      bool idExit = true;
// Muon Entry is around z 6783 and r  4255 
      if(fabs(particle.perigeeParameters().position().z())>6700.) idExit = false; 
      if(particle.perigeeParameters().position().perp()>4200.) idExit = false; 
      PropDirection propDir = idExit ? alongMomentum : oppositeMomentum;
      return caloExtension(particle.perigeeParameters(),propDir,particleType);
    }
  
    const Track& track = *particle.track();

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
      idExitParamers = track.perigeeParameters();
    }

    // pick start parameters, start in ID if possible
    const TrackParameters* startPars = idExitParamers ? idExitParamers : muonEntryParamers;
    if( !startPars ){
      ATH_MSG_WARNING("Failed to find  start parameters");
      return nullptr;
    }
    PropDirection propDir = idExitParamers ? alongMomentum : oppositeMomentum;
    
    return caloExtension(*startPars,propDir,particleType);
  }


  Trk::CaloExtension* ParticleCaloExtensionTool::caloExtension( const TrackParameters& startPars, PropDirection propDir, 
                                                                ParticleHypothesis particleType ) const {

    ATH_MSG_DEBUG("looking up calo states: r " << startPars.position().perp() << " z " << startPars.position().z()
                  << " momentum " << startPars.momentum().mag() );

    // pointers to hold results and go
    std::vector<const TrackStateOnSurface*>* material = 0;//new std::vector<const TrackStateOnSurface*>();
    const auto* caloParameters = m_extrapolator->extrapolate( startPars, propDir, particleType, material, 3 ); 
    if( material ) {
      ATH_MSG_DEBUG("Got material " << material->size() );
      for( auto& m : *material ) {
        if( msgLvl(MSG::DEBUG) ){
          msg(MSG::DEBUG) << " layer ";
          const TrackParameters* param = m->trackParameters();
          if( param ) msg(MSG::DEBUG) << " param " << param << " pos: r " << param->position().perp() << " z " << param->position().z() 
                                      << " pt " << param->momentum().perp();
          const Trk::MaterialEffectsBase* mat = m->materialEffectsOnTrack();
          if( mat ) msg(MSG::DEBUG) << " mat: " <<  mat->thicknessInX0();
          msg(MSG::DEBUG) << endmsg;
        }
        delete m;
      }
      delete material;
    }
    if( !caloParameters ) return nullptr;
    TrackParametersIdHelper  parsIdHelper;

    // create final object
    const TrackParameters* caloEntry = 0;
    const TrackParameters* muonEntry = 0;
    std::vector<const CurvilinearParameters*> caloLayers;
    caloLayers.reserve(caloParameters->size()-1);
    ATH_MSG_DEBUG( " Found calo parameters: " << caloParameters->size() );
    for( const auto& p : *caloParameters ){

      const TrackParameters* param = p.first;
      if( !param ) continue;
      ATH_MSG_DEBUG( " param " << param << " id " << p.second /*<< " type " << p.first->type()*/ << " pos: r " << param->position().perp() << " z " << param->position().z() 
                     << " pt " << param->momentum().perp() << " cov " << param->covariance() );
      
      // assign parameters
      if( p.second == 1 && propDir == Trk::alongMomentum)         caloEntry = p.first;
      else if( p.second == 3 && propDir == Trk::oppositeMomentum) caloEntry = p.first;
      else if( p.second == 3 && propDir == Trk::alongMomentum)    muonEntry = p.first;
      else if( p.second == 4 && propDir == Trk::oppositeMomentum) muonEntry = p.first;
      else{
        bool isEntry = p.second > 0 ? true : false;
        TrackParametersIdentifier id = parsIdHelper.encode( AtlasDetDescr::fFirstAtlasCaloTechnology, 
                                                            static_cast<CaloSampling::CaloSample>( abs(p.second)%1000 ),
                                                            isEntry );
        const CurvilinearParameters* cpars = dynamic_cast<const CurvilinearParameters*>(p.first);
        if( !cpars ){ /*p.first->type() == Curvilinear*/
          cpars = new CurvilinearParameters(p.first->position(),p.first->momentum(),p.first->charge(),nullptr,id);
          delete p.first;
        }else{
          const_cast<CurvilinearParameters*>(cpars)->m_cIdentifier = id;
        }
        caloLayers.push_back( cpars );
      }      
    }

    if(!muonEntry && propDir == Trk::oppositeMomentum && fabs(startPars.position().perp()-4255.)<1.) {
// muonEntry is right at the startPars position
      muonEntry = startPars.clone();
    } 

    if( muonEntry ) {
       if( muonEntry->covariance() ) { 
       ATH_MSG_VERBOSE (" p at MuonEntry " << muonEntry->momentum().mag() << " cov 00 " << (*(muonEntry->covariance()))(0,0) << " cov 11 " << (*(muonEntry->covariance()))(1,1));
       }
    }

    // add the following lines to avoid double deletion of the "startPars" object
    // more details in jira DATREP-183
    if (muonEntry == &startPars) {
	muonEntry = startPars.clone();
    }

    // clean-up memory
    delete caloParameters;
    CaloExtension* theExtension = new CaloExtension(caloEntry,muonEntry,std::move(caloLayers));

    // now add the extension to the output collection so we are not causing any leaks
    CaloExtensionCollection* collection = 0;
    if( !evtStore()->contains<CaloExtensionCollection>(m_containerName) ){
      collection = new CaloExtensionCollection();
      if( evtStore()->record( collection, m_containerName).isFailure() ) {
        ATH_MSG_WARNING( "Failed to record output collection, will leak the ParticleCaloExtension");
        delete collection;
        collection = 0;
      }
    }else{
      if(evtStore()->retrieve(collection,m_containerName).isFailure()) {
        ATH_MSG_WARNING( "Unable to retrieve " << m_containerName << " will leak the ParticleCaloExtension" );
        collection = 0;
      }
    }
    if( collection ) collection->push_back(theExtension);
    else{
      ATH_MSG_WARNING( "No CaloExtension Collection, failing extension to avoid memory leak");
      delete theExtension;
      theExtension = 0;
    }
    return theExtension;
  }
} // end of namespace Trk


