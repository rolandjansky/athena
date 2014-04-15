/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeSimSelector.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "ISF_SimulationSelectors/ConeSimSelector.h"
// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/HepMCHelper.h"
#include "ISF_HepMC_Event/HepMC_TruthBinding.h"

// Trk
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkGeometry/TrackingGeometry.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"

/** Constructor **/
ISF::ConeSimSelector::ConeSimSelector(const std::string& t, const std::string& n, const IInterface* p) : 
  ISimulationSelector(t,n,p),
  ConeParticleCuts(),
  m_absPDGVector(),
  m_checkRelatives(false),
  m_relativesVec(),
  m_relatives(),
  m_relationProp(),
  m_relation(HepMC::parents),
  m_trackingGeometry(0),
  m_trackingGeometrySvc("TrackingGeometrySvc/AtlasTrackingGeometrySvc",n),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_extrapolateToCalo(false),
  m_caloEntrance(0),
  m_pdgToParticleHypothesis(new Trk::PdgToParticleHypothesis())
{
    declareInterface<ISF::ISimulationSelector>(this);
    
    // cone options
    declareProperty( "ConeCreatorPDGs",
                     m_absPDGVector,
                     "Register cones around particles with this PDG code" );
    declareProperty( "ConeCreatorMinPt",
                     m_cut_minPt,
                     "Register cones around particles with this minimum pT" );
    declareProperty( "ConeSize",
                     m_cut_coneSize2,
                     "Size of cone around initial particles." );
    declareProperty( "CheckParticlePosition",
                     m_cut_checkPosition,
                     "Choose if particle position (or momentum direction) has to be within cone." );


    declareProperty( "CheckConeCreatorAncestors",
                     m_checkRelatives,
                     "on/off switch to check ancestor particles for cone creation (eg. cone only around e from Z decay)" );
    declareProperty( "ConeCreatorAncestor",
                     m_relativesVec,
                     "Only ISFParticles which have specified 'Relation' to the given PDGCode are selected." );
    // as defined in the HepMC manual HepMC::GenVertex::particle iterator
    declareProperty( "ConeCreatorAncestorRelation",
                     m_relationProp,
                     "Relation to the given PDG codes: 0=parents, 1=family, 2=ancestors, 3=relatives." );

    declareProperty( "TrackingGeometrySvc", 
		     m_trackingGeometrySvc,
		     "TrackingGeometrySvc used for track extrapolation" );
    declareProperty( "Extrapolator", 
		     m_extrapolator,
		     "Extrapolator used for track extrapolation" );   
    declareProperty( "ExtrapolateToCaloEntry",
		     m_extrapolateToCalo,
		     "Extrapolate particles to calorimeter entry layer and build cone there" );
}


/** Destructor **/
ISF::ConeSimSelector::~ConeSimSelector()
{
  delete m_pdgToParticleHypothesis;
}


// Athena algtool's Hooks
StatusCode  ISF::ConeSimSelector::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    // store the square of the cone size (faster comparison)
    m_cut_coneSize2 *= m_cut_coneSize2;
    // convert std::vector to std::set for faster searching
    m_cut_absPdgs.insert( m_absPDGVector.begin(), m_absPDGVector.end() );

    // check relatives for cone creating particle, in case vector is filled
    if ( m_checkRelatives) {
      // convert std::vector to std::set for faster searching
      m_relatives.insert( m_relativesVec.begin(), m_relativesVec.end() );
      // convert the 'Relation' Python property into a HepMC data type
      m_relation = HepMCHelper::convertIteratorRange( m_relationProp);
    }
    
    StatusCode sc = StatusCode::SUCCESS;

    if ( m_extrapolateToCalo ){
      sc = m_extrapolator.retrieve();
      if (sc.isFailure()){
	ATH_MSG_FATAL( "Could not get " << m_extrapolator );
	return StatusCode::FAILURE;
      }
    }
    
    ATH_MSG_VERBOSE( "Initialize successful" );
    return sc;
}


StatusCode  ISF::ConeSimSelector::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");

    ATH_MSG_VERBOSE("Finalize successful");
    return StatusCode::SUCCESS;
}



void ISF::ConeSimSelector::beginEvent()
{
    ATH_MSG_VERBOSE("beginEvent being called. Resetting the cone container.");

    ConeSimSelector::resetCones();
}


void ISF::ConeSimSelector::endEvent()
{
    ATH_MSG_VERBOSE("endEvent being called.");
}


void ISF::ConeSimSelector::update(const ISFParticle& particle) {

  // this method will determine whether or not to create a cone around the given particle

  ATH_MSG_VERBOSE("Checking whether cone will be created around particle: " << particle);

  // check relatives if required
  if ( m_checkRelatives ) {
    // get the truth binding (as HepMC)
    const HepMC_TruthBinding *truth = dynamic_cast<HepMC_TruthBinding*>( particle.truthBinding() );

    if (truth) {
      // get GenParticle from truth binding
      const HepMC::GenParticle &genParticle = truth->truthParticle();

      // test whether any of the pdg codes is found in the genParticle history
      const HepMC::GenParticle *relative = HepMCHelper::findRealtiveWithPDG( genParticle, m_relation, m_relatives);

      if (relative) {
        ATH_MSG_VERBOSE("Current particle has valid relative particle:"
                        << " (pdg=" << relative->pdg_id() << ","
                        << " barcode=" << relative->barcode() << ")."
                        << " Will now check whether cone cuts apply" );
      } 
      else {
        // cone conditions not fulfilled
        return;
      }
    } // has truth association
  } // check relatives

  // do a quick check to see whether passes minimum cuts before extrapolating
  bool passes=checkParticle(particle);
  double eta=particle.momentum().eta();
  double phi=particle.momentum().phi();

  if ( passes ) {
    ATH_MSG_VERBOSE("Passes minimum cuts");
    
    if ( m_extrapolateToCalo ) {
      
      if( m_extrapolator.empty() ) {
	ATH_MSG_ERROR( "Problem with extrapolator!" );
	return;
      }
      
      if ( !m_caloEntrance ) {
	if ( !m_trackingGeometry ) { 
	  if ( !retrieveTrackingGeometry() ) {
	    ATH_MSG_ERROR( "Problem with TrackingGeometrySvc!" );
	    return;
	  }
	}
	m_caloEntrance = m_trackingGeometry->trackingVolume("InDet::Containers::EntryVolume");
	if (!m_caloEntrance) ATH_MSG_FATAL("Failed to retrieve calo entrance!");
      }
      
      // create objects from ISFParticle needed for extrapolation
      Trk::CurvilinearParameters par(particle.position(),particle.momentum(),particle.charge());
      
      int  absPdg   = abs(particle.pdgCode());
      //bool photon   = (absPdg == 22);
      //bool geantino = (absPdg == 999);
      //bool charged  = photon || geantino ? false : (particle.charge()*particle.charge() > 0) ;
      
      Trk::ParticleHypothesis particleHypo = 
	m_pdgToParticleHypothesis->convert(particle.pdgCode(),particle.charge());
      if ( absPdg == 999 ) particleHypo = Trk::geantino;
      
      // extrapolate to calorimeter entry
      const Trk::TrackParameters* extrapolatedPars = 
	m_extrapolator->extrapolateToVolume(par,*m_caloEntrance,Trk::alongMomentum,particleHypo);
      
      // get momentum from extrapolatedPars
      const Amg::Vector3D& extrapolatedMomentum = extrapolatedPars->momentum();
      
      // check if passes cuts
      passes = ConeParticleCuts::checkAndRegisterCone(particle,extrapolatedMomentum);
      eta=extrapolatedMomentum.eta();
      phi=extrapolatedMomentum.phi();
      
      ATH_MSG_DEBUG("Initial position: ("
		    <<particle.position().x()<<","
		    <<particle.position().y()<<","
		    <<particle.position().z()<<"), final position: ("
		    <<extrapolatedPars->position().x()<<","
		    <<extrapolatedPars->position().y()<<","
		    <<extrapolatedPars->position().z()<<")");
      
      ATH_MSG_DEBUG("Initial eta/phi: ("
		    <<particle.momentum().eta()<<"/"
		    <<particle.momentum().phi()<<"), final eta/phi: ("
		    <<extrapolatedPars->momentum().eta()<<"/"
		    <<extrapolatedPars->momentum().phi()<<")");
      
      // cleanup
      delete extrapolatedPars;
    }
    else {
      passes = ConeParticleCuts::checkAndRegisterCone(particle);
      eta=particle.momentum().eta();
      phi=particle.momentum().phi();
    }
    
    if (passes) {
      ATH_MSG_DEBUG("Particle (eta=" << eta << ", " 
		    << " phi=" << phi << ","
		    << " pdg=" << particle.pdgCode() << ","
		    << " barcode=" << particle.barcode() << ")"
		    << " has passed all cuts. A new simulation cone around it has been created");
    }
  }


  if (!passes) {
    ATH_MSG_VERBOSE("Particle (eta=" << eta << ", " 
                    << " phi=" << phi << ","
                    << " pdg=" << particle.pdgCode() << ","
                    << " barcode=" << particle.barcode() << ")"
                    << " did not pass all cuts. NO new simulation cone has been created");
  }
}


/** check whether given particle passes all cuts -> will be used for routing decision*/
bool  ISF::ConeSimSelector::passSelectorCuts(const ISFParticle& isfpart) const
{
  bool passes = ISF::ConeParticleCuts::insideCone( isfpart );
  return passes;
}

bool ISF::ConeSimSelector::retrieveTrackingGeometry() const 
{
  if ( m_trackingGeometry ) return true;
  if ( m_trackingGeometrySvc.empty() ) return false;
  MsgStream log(msgSvc(),name());
  StatusCode sc = m_trackingGeometrySvc.retrieve();
  if ( sc.isFailure() ){
    log << MSG::WARNING << " failed to retrieve geometry Svc " << m_trackingGeometrySvc << endreq;
    return false;
  } 
  log << MSG::INFO << "  geometry Svc " << m_trackingGeometrySvc << " retrieved " << endreq;
  
  m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();
  if ( !m_trackingGeometry ) return false;
  return true;
}
