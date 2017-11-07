/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackCaloClusterRecTools/ParticleToCaloExtrapolationTool.h"
// forward declares
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#define private public
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#define public private 
#include "xAODTracking/TrackingPrimitives.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODMuon/Muon.h"


ParticleToCaloExtrapolationTool::ParticleToCaloExtrapolationTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p),
    m_detID(nullptr),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    m_particleType(Trk::muon)
{

  declareInterface<IParticleExtrapolationTool>(this);
  declareProperty("Extrapolator",        m_extrapolator );
  declareProperty("ParticleType",        m_particleTypeName = "pion" );
}

ParticleToCaloExtrapolationTool::~ParticleToCaloExtrapolationTool() {}

StatusCode ParticleToCaloExtrapolationTool::initialize() {
  /* Retrieve track extrapolator from ToolService */
  ATH_CHECK(detStore()->retrieve(m_detID, "AtlasID" ));
  ATH_CHECK( m_extrapolator.retrieve() );

  if( m_particleTypeName == "nonInteracting" ) m_particleType = Trk::nonInteracting;
  else if( m_particleTypeName == "muon" )      m_particleType = Trk::muon;
  else if( m_particleTypeName == "pion" )      m_particleType = Trk::pion;
  else ATH_MSG_WARNING("Unsupported particle type, using muon " << m_particleTypeName );
  ATH_MSG_INFO(" Using particle type " << m_particleTypeName << " enum value " << m_particleType );
     
  return StatusCode::SUCCESS;
}

StatusCode ParticleToCaloExtrapolationTool::finalize() {
  return StatusCode::SUCCESS;
}

const xAOD::TrackParticle* ParticleToCaloExtrapolationTool::getTrackParticle(const xAOD::IParticle& particle ) const {

  const xAOD::TrackParticle* trackParticle = dynamic_cast< const xAOD::TrackParticle*>(&particle);
  if( trackParticle ) return trackParticle;

  const xAOD::Muon* muon = dynamic_cast< const xAOD::Muon*>(&particle);
  if( muon && muon->primaryTrackParticle() ) return muon->primaryTrackParticle();
  return nullptr;
}

bool ParticleToCaloExtrapolationTool::particleToCaloExtrapolate( const xAOD::IParticle& particle, const Trk::CaloExtension*& theExtension) const {
  
  // reset input
  theExtension = 0;

  // ATH_MSG_DEBUG(" caloExtension: ptr " << &particle );
  
  // work out the type of particle and get the extension
  const xAOD::TrackParticle* trackParticle = getTrackParticle(particle);
  if( trackParticle ) theExtension = particleToCaloExtrapolate(*trackParticle);
  else{
    const xAOD::NeutralParticle* neutralParticle = dynamic_cast< const xAOD::NeutralParticle*>(&particle);
    if( neutralParticle ) theExtension = particleToCaloExtrapolate(*neutralParticle);
    else{
      const xAOD::TruthParticle* truthParticle = dynamic_cast< const xAOD::TruthParticle*>(&particle);
      if( truthParticle ) theExtension = particleToCaloExtrapolate(*truthParticle);
      else{
        ATH_MSG_WARNING("Unsupported IParticle type");
        return false;
      }
    }
  }

  // return false is extension failed
  return theExtension != nullptr;
}

Trk::CaloExtension* ParticleToCaloExtrapolationTool::particleToCaloExtrapolate( const xAOD::TruthParticle& particle ) const {

  // get particle type
  Trk::ParticleHypothesis                particleType = Trk::muon; //nonInteracting;
  if( abs(particle.pdgId()) == 11 )      particleType = Trk::muon;   // we dont want the electron to loose energy when extrpolating in the calo
  else if( abs(particle.pdgId()) == 13 ) particleType = Trk::muon;

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
  return caloExtension( startPars, Trk::alongMomentum, particleType );
}

Trk::CaloExtension* ParticleToCaloExtrapolationTool::particleToCaloExtrapolate( const xAOD::NeutralParticle& particle ) const {

  float d0     = particle.d0();
  float z0     = particle.z0();
  float phi    = particle.phi0();
  float theta  = particle.theta();
  float oneOverP = particle.oneOverP();
  
  float vx = 0.;
  float vy = 0.;
  float vz = particle.vz();
  
  // Building the perigee
  Trk::NeutralPerigee* perigee = new Trk::NeutralPerigee(d0, z0, phi, theta, oneOverP, Trk::PerigeeSurface(Amg::Vector3D(vx, vy, vz)));
  
  double charge = 1.;
  Amg::Vector3D  pos( perigee->position() );
  Amg::Vector3D  mom( perigee->momentum() );
  delete perigee;
  // hack, extrapolate neutral particles as infinit momentum for the time being
  mom.normalize();
  mom *= 1e10;
  Trk::CurvilinearParameters startPars(pos,mom,charge);

  // get extension
  return caloExtension( startPars, Trk::alongMomentum, Trk::muon );
}

Trk::CaloExtension* ParticleToCaloExtrapolationTool::particleToCaloExtrapolate( const xAOD::TrackParticle& particle ) const {

  Trk::ParticleHypothesis particleType = m_particleType;
  bool idExit = true;

  const Trk::TrackParameters* startPar = nullptr;

  bool createdPerigee(false);
  if (particle.isAvailable<std::vector<float> >( "definingParametersCovMatrix" ) and 
      particle.isAvailable<float>( "vx" ) and particle.isAvailable<float>( "vy" ) ) {
    startPar = &particle.perigeeParameters();
  } else {
    float d0     = particle.d0();
    float z0     = particle.z0();
    float phi    = particle.phi0();
    float theta  = particle.theta();
    float qOverP = particle.qOverP();
    
    float vx = 0.;
    float vy = 0.;
    float vz = particle.vz();
    
    // Building the perigee
    startPar = new Trk::Perigee(d0, z0, phi, theta, qOverP, Trk::PerigeeSurface(Amg::Vector3D(vx, vy, vz)));
    createdPerigee = true;
  }

  if(fabs(startPar->position().z())>6700.) idExit = false; 
  if(startPar->position().perp()>4200.) idExit = false; 
  Trk::PropDirection propDir = idExit ? Trk::alongMomentum : Trk::oppositeMomentum;
  Trk::CaloExtension* extension = caloExtension(*startPar,propDir,particleType);
  if(createdPerigee) delete startPar;
 
  return extension;
}


Trk::CaloExtension* ParticleToCaloExtrapolationTool::caloExtension( const Trk::TrackParameters& startPars, Trk::PropDirection propDir,
						      Trk::ParticleHypothesis particleType ) const {

  // ATH_MSG_DEBUG("looking up calo states: r " << startPars.position().perp() << " z " << startPars.position().z()
                // << " momentum " << startPars.momentum().mag() );

  // pointers to hold results and go
  std::vector<const Trk::TrackStateOnSurface*>* material = 0;//new std::vector<const Trk::TrackStateOnSurface*>();
  const auto* caloParameters = m_extrapolator->extrapolate( startPars, propDir, particleType, material, 3 ); 
  if( material ) {
    ATH_MSG_DEBUG("Got material " << material->size() );
    for( auto& m : *material ) {
      if( msgLvl(MSG::DEBUG) ){
        msg(MSG::DEBUG) << " layer ";
        const Trk::TrackParameters* param = m->trackParameters();
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
  Trk::TrackParametersIdHelper  parsIdHelper;

  // create final object
  const Trk::TrackParameters* caloEntry = 0;
  const Trk::TrackParameters* muonEntry = 0;
  std::vector<const Trk::CurvilinearParameters*> caloLayers;
  caloLayers.reserve(caloParameters->size()-1);
  // ATH_MSG_DEBUG( " Found calo parameters: " << caloParameters->size() );
  for( const auto& p : *caloParameters ){

    const Trk::TrackParameters* param = p.first;
    if( !param ) continue;
    // ATH_MSG_DEBUG( " param " << param << " id " << p.second /*<< " type " << p.first->type()*/ << " pos: r " << param->position().perp() << " z " << param->position().z() 
                   // << " pt " << param->momentum().perp() << " cov " << param->covariance() );
    
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
      const Trk::CurvilinearParameters* cpars = dynamic_cast<const Trk::CurvilinearParameters*>(p.first);
      if( !cpars ){ /*p.first->type() == Curvilinear*/
        cpars = p.first->covariance() ? new Trk::CurvilinearParameters(p.first->position(),p.first->momentum(),p.first->charge(),new AmgSymMatrix(5)(*p.first->covariance()),id) : new Trk::CurvilinearParameters(p.first->position(),p.first->momentum(),p.first->charge(),nullptr,id);
        delete p.first;
      }else{
        const_cast<Trk::CurvilinearParameters*>(cpars)->m_cIdentifier = id;
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
  // clean-up memory
  delete caloParameters;
  return new Trk::CaloExtension(caloEntry,muonEntry,std::move(caloLayers));
    
}

