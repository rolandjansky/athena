/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "ParticleCaloExtensionTool.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"

#include "xAODTruth/TruthVertex.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/Electron.h"
namespace Trk
{
ParticleCaloExtensionTool::ParticleCaloExtensionTool(const std::string& t, const std::string& n, const IInterface*  p )
  : AthAlgTool(t,n,p),
  m_detID(nullptr),
  m_particleType(muon){
    declareInterface<IParticleCaloExtensionTool>(this);
  }

ParticleCaloExtensionTool::~ParticleCaloExtensionTool() {}

StatusCode ParticleCaloExtensionTool::initialize() {
  /* Retrieve track extrapolator from ToolService */
  ATH_CHECK( m_extrapolator.retrieve() );
  /* Retrieve the Atlas detector ID */
  ATH_CHECK(detStore()->retrieve(m_detID, "AtlasID" ));
  /* convert string to proper particle type enum*/  
  if( m_particleTypeName == "nonInteracting" ) {m_particleType = nonInteracting;}
  else if( m_particleTypeName == "muon" )      {m_particleType = muon;}
  else if( m_particleTypeName == "pion" )      {m_particleType = pion;}
  else if( m_particleTypeName == "electron" )  {m_particleType = electron;}
  else {ATH_MSG_WARNING("Unsupported particle type, using muon " << m_particleTypeName );}
  ATH_MSG_INFO(" Using particle type " << m_particleTypeName << " enum value " << m_particleType );
  return StatusCode::SUCCESS;
}

StatusCode ParticleCaloExtensionTool::finalize() {
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::CaloExtension> 
ParticleCaloExtensionTool::caloExtension(const xAOD::IParticle& particle) const {

  ATH_MSG_DEBUG(" caloExtension: index " << particle.index());
  // work out the type of particle and get the extension

  if(particle.type()==xAOD::Type::TrackParticle){
    const xAOD::TrackParticle* trackParticle = static_cast< const xAOD::TrackParticle*>(&particle); 
    return caloExtension(*trackParticle);
  }
  else if (particle.type()==xAOD::Type::TruthParticle){
    const xAOD::TruthParticle* truthParticle = static_cast< const xAOD::TruthParticle*>(&particle);
    return caloExtension(*truthParticle);
  }
  else if(particle.type()==xAOD::Type::Electron){
    const xAOD::Electron* el = static_cast< const xAOD::Electron*>(&particle);
    if( el->trackParticle() ) {
      return caloExtension(*(el->trackParticle()));
    } 
  }
  else if(particle.type()==xAOD::Type::Muon){
    const xAOD::Muon* muon = static_cast< const xAOD::Muon*>(&particle);
    if( muon->primaryTrackParticle() ) {
      return caloExtension(*(muon->primaryTrackParticle()));
    } 
  }
  else if(particle.type()==xAOD::Type::NeutralParticle){
    const xAOD::NeutralParticle* neutralParticle = static_cast< const xAOD::NeutralParticle*>(&particle);
    return caloExtension(*neutralParticle);
  } 
  ATH_MSG_WARNING("Unsupported IParticle type");
  return nullptr;
}

const Trk::CaloExtension* ParticleCaloExtensionTool::caloExtension( const xAOD::IParticle& particle, 
                                                                    IParticleCaloExtensionTool::Cache& cache ) const{
  /*if not there , default ctor for unique_ptr (nullptr)*/
  std::unique_ptr<Trk::CaloExtension>& extension= cache[particle.index()];
  if (extension.get()==nullptr){   
    ATH_MSG_DEBUG(" Adding  caloExtension to cache for index: " << particle.index());
    extension=caloExtension(particle);
  }
  return extension.get();
}

const Trk::CaloExtension* ParticleCaloExtensionTool::caloExtension( const xAOD::IParticle& particle, 
                                                                    const CaloExtensionCollection& cache ) const{
  size_t index=particle.index();
  if(index < cache.size()){ 
    return cache[index];
  } 
  ATH_MSG_WARNING("cache size smaller than particle index"); 
  return nullptr;
}

StatusCode ParticleCaloExtensionTool::caloExtensionCollection( const xAOD::IParticleContainer& particles, 
                                                               const std::vector<bool>& mask,
                                                               CaloExtensionCollection& caloextensions) const{
  const size_t numparticles=particles.size();   
  if(mask.size()!=numparticles){
    ATH_MSG_ERROR("mask does not have the same size as in input collection");
    return StatusCode::FAILURE;
  }

  /* Either create a proper CaloExtension or otherwise a dummy one
   * i.e one with no intersections
   */
  for (size_t i=0 ; i<numparticles; ++i){
    if (mask[i]==true){
      std::unique_ptr<Trk::CaloExtension> extension=caloExtension(*(particles[i]));
      if(extension!=nullptr){
        caloextensions.push_back(std::move(extension));
      }
      else {
        std::vector<const CurvilinearParameters*> dummyPar{};
        std::unique_ptr<Trk::CaloExtension> dummyExt=std::make_unique<Trk::CaloExtension>(nullptr,nullptr,std::move(dummyPar));
        caloextensions.push_back(std::move (dummyExt));
      }
    }
    else{
      std::vector<const CurvilinearParameters*> dummyPar{};
      std::unique_ptr<CaloExtension> dummyExt=std::make_unique<Trk::CaloExtension>(nullptr,nullptr,std::move(dummyPar));
      caloextensions.push_back(std::move (dummyExt));
    }
  }
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::CaloExtension> ParticleCaloExtensionTool::caloExtension( const xAOD::TruthParticle& particle ) const {

  ParticleHypothesis particleType = muon;  
  if( abs(particle.pdgId()) == 11 )      {particleType = muon;} 
  else if( abs(particle.pdgId()) == 13 ) {particleType = muon;}
  // get start parameters
  const xAOD::TruthVertex* pvtx = particle.prodVtx();
  if ( pvtx == 0 ) {
    return nullptr;
  }
  double charge = particle.charge();
  Amg::Vector3D  pos( pvtx->x() , pvtx->y() , pvtx->z() );
  Amg::Vector3D  mom( particle.px() , particle.py() , particle.pz() );
  // Aproximate neutral particles as charged with infinite momentum
  if( particle.isNeutral() ){
    charge = 1.;
    mom.normalize();
    mom *= 1e10;
  }
  Trk::CurvilinearParameters startPars(pos,mom,charge);
  // get extension
  return caloExtension( startPars, alongMomentum, particleType );
}

std::unique_ptr<Trk::CaloExtension> ParticleCaloExtensionTool::caloExtension( const xAOD::NeutralParticle& particle ) const {

  // create start parameters
  const Trk::NeutralPerigee& perigee = particle.perigeeParameters();
  double charge = 1.;
  Amg::Vector3D  pos( perigee.position() );
  Amg::Vector3D  mom( perigee.momentum() );
  // Aproximate neutral particles as charged with infinite momentum
  mom.normalize();
  mom *= 1e10;
  Trk::CurvilinearParameters startPars(pos,mom,charge);
  // get extension
  return caloExtension( startPars, alongMomentum, muon );
}

std::unique_ptr<Trk::CaloExtension> ParticleCaloExtensionTool::caloExtension( const xAOD::TrackParticle& particle ) const {


  /* 
   * The electrons are done separately here.
   *
   * Here we have also the Gaussian Sum Filter having out back
   * as a main case to consider
   *
   * In principle we will extrapolate either from the perigee or 
   * from the last measurement of the trackParticle
   * The extrapolation will be done as a muon since this
   * is the closest to non-interacting in the calorimeter
   * while still providing intersections.
   */

  if(m_particleType == electron || 
     particle.particleHypothesis() ==  xAOD::electron ){  
    ATH_MSG_DEBUG("Fitting using electron hypothesis");
    ParticleHypothesis particleTypeForElectron = muon;//nonInteracting;
    if(!m_startFromPerigee){
      unsigned int index(0);
      if(particle.indexOfParameterAtPosition(index, xAOD::LastMeasurement)){    
        return caloExtension(particle.curvilinearParameters(index),alongMomentum,particleTypeForElectron);
      } else {
        ATH_MSG_INFO("No last measurement:"
                     <<" Perhaps you try to run the extrapolator after slimming."
                     <<" Falling back to perigee");
      }
    }
    return caloExtension(particle.perigeeParameters(),alongMomentum,particleTypeForElectron);
  }

  /* 
   * This is the case for muons of pions
   * Extra logic for the muon spectrometer entry and exit
   */
  ParticleHypothesis particleType = m_particleType;


  if(m_startFromPerigee || !particle.track()){
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


std::unique_ptr<Trk::CaloExtension> ParticleCaloExtensionTool::caloExtension( const TrackParameters& startPars, 
                                                                              PropDirection propDir, 
                                                                              ParticleHypothesis particleType ) const {

  ATH_MSG_DEBUG("looking up calo states: r " << startPars.position().perp() << " z " << startPars.position().z()
                << " momentum " << startPars.momentum().mag() );

  // pointers to hold results and go
  std::vector<const TrackStateOnSurface*>* material = nullptr;
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
  if( !caloParameters ) {
    return nullptr;
  }
  TrackParametersIdHelper  parsIdHelper;

  // create final object
  const TrackParameters* caloEntry = 0;
  const TrackParameters* muonEntry = 0;
  std::vector<const CurvilinearParameters*> caloLayers;
  caloLayers.reserve(caloParameters->size()-1);
  ATH_MSG_DEBUG( " Found calo parameters: " << caloParameters->size() );

  for( const auto& p : *caloParameters ){
    if( !p.first ) {
      continue;
    }

    ATH_MSG_DEBUG( " param " << p.first << " id " << p.second  
                   << " pos: r " << p.first->position().perp() << " z " << p.first->position().z() 
                   << " pt " << p.first->momentum().perp() << " cov " << p.first->covariance() );

    // assign parameters
    if( p.second == 1 && propDir == Trk::alongMomentum)         {caloEntry = p.first;}
    else if( p.second == 3 && propDir == Trk::oppositeMomentum) {caloEntry = p.first;}
    else if( p.second == 3 && propDir == Trk::alongMomentum)    {muonEntry = p.first;}
    else if( p.second == 4 && propDir == Trk::oppositeMomentum) {muonEntry = p.first;}
    else{
      bool isEntry = p.second > 0 ? true : false;
      TrackParametersIdentifier id = parsIdHelper.encode( AtlasDetDescr::fFirstAtlasCaloTechnology, 
                                                          static_cast<CaloSampling::CaloSample>( abs(p.second)%1000 ),
                                                          isEntry );

      /*
       * We construct curvilinear parameters which we push
       * back to the caloLayers
       */
      if( p.first->type() != Trk::Curvilinear ){
        const CurvilinearParameters* cpars = new CurvilinearParameters(p.first->position(),
                                                                       p.first->momentum(),p.first->charge(),nullptr,id); 
        caloLayers.push_back( cpars );
        delete p.first;
      }else{ 
        AmgSymMatrix(5)* covariance(nullptr);
        if(p.first->covariance()){
          covariance=new AmgSymMatrix(5)(*(p.first->covariance()));
        }
        /*Note that the curvilinear parameters (cpars) now own the covariance
         * it will be deleted by the ParameterT dtor*/
        const CurvilinearParameters* cpars = new CurvilinearParameters(p.first->position(),p.first->momentum(),
                                                                       p.first->charge(),covariance,id);
        caloLayers.push_back( cpars );
        delete p.first;
      }
    }      
  }

  if(!muonEntry && propDir == Trk::oppositeMomentum 
     && fabs(startPars.position().perp()-4255.)<1.) {
    // muonEntry is right at the startPars position
    muonEntry = startPars.clone();
  } 

  if( muonEntry  &&  muonEntry->covariance() ) { 
    ATH_MSG_VERBOSE (" p at MuonEntry " << muonEntry->momentum().mag() 
                     << " cov 00 " << (*(muonEntry->covariance()))(0,0) 
                     << " cov 11 " << (*(muonEntry->covariance()))(1,1));
  }

  delete caloParameters;

  return std::make_unique<Trk::CaloExtension>(caloEntry,muonEntry,std::move(caloLayers));
}

} // end of namespace Trk


