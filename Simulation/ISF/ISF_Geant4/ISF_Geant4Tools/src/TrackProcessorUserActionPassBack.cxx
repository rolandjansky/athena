/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackProcessorUserActionPassBack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TrackProcessorUserActionPassBack.h"

// includes from this package
#include "ISFG4Helpers.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ITruthBinding.h"
#include "ISF_Event/EntryLayer.h"

#include "ISF_HepMC_Event/HepMC_TruthBinding.h"

#include "ISF_Interfaces/IParticleBroker.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/VTrackInformation.h"

#include "SimHelpers/StepHelper.h"
#include "StoreGate/StoreGateSvc.h"

// Geant4 includes
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"
//#include "G4VPhysicalVolume.hh"

#include <iostream>

iGeant4::TrackProcessorUserActionPassBack::TrackProcessorUserActionPassBack(const std::string& type,
                                                            const std::string& name,
                                                            const IInterface* parent)
  : TrackProcessorUserActionBase(type,name,parent),
    m_particleBroker("",name),
    m_particleBrokerQuick(0),
    m_geoIDSvc("",name),
    m_geoIDSvcQuick(0),
    m_passBackEkinThreshold(0.05), //TODO: should we add units here (MeV)?
    m_killBoundaryParticlesBelowThreshold(false)
{

  ATH_MSG_DEBUG("create TrackProcessorUserActionPassBack name: "<<name);

  declareProperty("ParticleBroker", m_particleBroker, "ISF Particle Broker Svc");
  declareProperty("GeoIDSvc"      , m_geoIDSvc      , "ISF GeoID Svc"          );

  declareProperty("PassBackEkinThreshold",
                  m_passBackEkinThreshold=0.05, //TODO: should we add units here (MeV)?
                  "Ekin cut-off for particles returned to ISF");
  declareProperty("KillBoundaryParticlesBelowThreshold",
                  m_killBoundaryParticlesBelowThreshold=false,
                  "Kill particles at boundary which are below Ekin cut-off rather than continue their simulation in G4");
}

StatusCode iGeant4::TrackProcessorUserActionPassBack::initialize()
{
  ATH_MSG_DEBUG("Initializing TrackProcessorUserActionPassBack");

  if ( !m_particleBroker.empty() ) {
    if (m_particleBroker.retrieve().isFailure()) {
      ATH_MSG_FATAL("Could not retrieve ISF Particle Broker: " << m_particleBroker);
      return StatusCode::FAILURE;
    }
    m_particleBrokerQuick = &(*m_particleBroker);
  }

  if ( !m_geoIDSvc.empty() ) {
    if ( m_geoIDSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL("Could not retrieve ISF GeoID Svc: " << m_geoIDSvc);
      return StatusCode::FAILURE;
    }
    m_geoIDSvcQuick = &(*m_geoIDSvc);
  }

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::TrackProcessorUserActionPassBack::finalize()
{
  return StatusCode::SUCCESS;
}


void iGeant4::TrackProcessorUserActionPassBack::ISFSteppingAction(const G4Step* aStep,
                                                                  ISF::ISFParticle *curISP)
{
  G4Track*           aTrack   = aStep->GetTrack();
  G4TrackStatus aTrackStatus  = aTrack->GetTrackStatus();

  const G4StepPoint *preStep  = aStep->GetPreStepPoint();
  const G4StepPoint *postStep = aStep->GetPostStepPoint();

  // get geoID from parent
  AtlasDetDescr::AtlasRegion curGeoID = curISP->nextGeoID();

  //std::cout<<"retrieved isp "<<curISP<<" for trackID "<<curISP<<std::endl;

  // check geoID of postStep
  const G4ThreeVector     &postPos  = postStep->GetPosition();
  //const G4ThreeVector     &postMom  = postStep->GetMomentum();
  //AtlasDetDescr::AtlasRegion  nextGeoID = m_geoIDSvcQuick->identifyNextGeoID( postPos.x(),
  //                                                                            postPos.y(),
  //                                                                            postPos.z(),
  //                                                                            postMom.x(),
  //                                                                            postMom.y(),
  //                                                                            postMom.z() );
  AtlasDetDescr::AtlasRegion  nextGeoID = m_geoIDSvcQuick->identifyGeoID( postPos.x(),
                                                                          postPos.y(),
                                                                          postPos.z() );

  ATH_MSG_DEBUG("PostStep point resolved to geoID = "<<nextGeoID);

  // return if particle did NOT cross boundary
  if ( nextGeoID==curGeoID ) {
    ATH_MSG_DEBUG(" -> G4Track stays inside geoID = "<<curGeoID);

    //
    // for debugging:
    //
    if ( msgLvl(MSG::DEBUG) ) {
      const G4ThreeVector &prePos = preStep->GetPosition();
      AtlasDetDescr::AtlasRegion  preStepGeoID  = m_geoIDSvcQuick->identifyGeoID( prePos.x(),
                                                                                  prePos.y(),
                                                                                  prePos.z() );
      AtlasDetDescr::AtlasRegion  postStepGeoID = m_geoIDSvcQuick->identifyGeoID( postPos.x(),
                                                                                  postPos.y(),
                                                                                  postPos.z() );
    
      if( preStepGeoID!=postStepGeoID ) {
        const G4VPhysicalVolume     *preVol  = preStep->GetPhysicalVolume();
        const G4VPhysicalVolume     *postVol = postStep->GetPhysicalVolume();
        const G4ThreeVector         &preMom  = preStep->GetMomentum();
        const G4ThreeVector        &postMom  = postStep->GetMomentum();
        const G4TrackVector *secondaryVector = aStep->GetSecondary();
        const G4ThreeVector&      aTrack_pos = aTrack->GetPosition();
        const G4ThreeVector&      aTrack_mom = aTrack->GetMomentum();
        int pdgID=aTrack->GetDefinition()->GetPDGEncoding();
        int bc=0;
        int                         aTrackID = aTrack->GetTrackID();
        TrackInformation* trackInfo=dynamic_cast<TrackInformation*>(aTrack->GetUserInformation());
        if (trackInfo) {
          bc=trackInfo->GetParticleBarcode();
        }

        ATH_MSG_WARNING("pre "<<preVol->GetName()<<" x="<<prePos.x()<<" y="<<prePos.y()<<" z="<<prePos.z()<<" p="<<preMom.mag()<<" geoID="<<preStepGeoID<<"; post "<<postVol->GetName()<<" x="<<postPos.x()<<" y="<<postPos.y()<<" z="<<postPos.z()<<" p="<<postMom.mag()<<" geoID="<<nextGeoID<<"; length="<<aStep->GetStepLength()<<"; n2nd="<<secondaryVector->size()<<" track  x="<<aTrack_pos.x()<<" y="<<aTrack_pos.y()<<" z="<<aTrack_pos.z()<<" p="<<aTrack_mom.mag()<<" curgeoID="<<curGeoID<<" pdgid="<<pdgID<<" bc="<<bc<<" trackID="<<aTrackID<<" ISF="<<curISP<<"; ploss="<<(postMom-preMom).mag());
      }
    }

    return;
  }
  
  
  //
  // this point is only reached if particle has crossed
  // a sub-det boundary in the non-Geant4-only mode
  //
  
  if ( aTrack->GetKineticEnergy() < m_passBackEkinThreshold ) {
    // kinetic energy of primary particle below threshold
    ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
                  " but is below Ekin threshold. Not returned to ISF.");
    if ( m_killBoundaryParticlesBelowThreshold ) {
        aTrack->SetTrackStatus( fStopAndKill );
    } else {
        // TODO: link G4Track to ISF particle with the new GeoID
    }
  } else if ( aTrackStatus!=fAlive ) {
    // particle is killed by G4 in this step
    // TODO: do we need to handle this case specifically?
    ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
                  " but is destroyed in this step. Not returned to ISF.");

  } else {
    // particle is above kinetic energy threshold and alive after this step
    // -> push new ISFParticle back to ISF particle broker
    ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
                  " and is returned to ISF.");

    ISF::ISFParticle *parent = curISP;
    returnParticleToISF(aTrack, parent, nextGeoID);
  }

  //
  // handle secondaries that were created in this G4Step
  //
  const std::vector<const G4Track*> *secondaryVector = aStep->GetSecondaryInCurrentStep();
  // loop over new secondaries
  for ( const G4Track* aConstTrack_2nd : *secondaryVector ) {
    // get a non-const G4Track for current secondary (nasty!)
    G4Track *aTrack_2nd = const_cast<G4Track*>( aConstTrack_2nd );

    // check if new secondary position is behind boundary
    const G4ThreeVector&             pos_2nd = aTrack_2nd->GetPosition();
    AtlasDetDescr::AtlasRegion nextGeoID_2nd = m_geoIDSvcQuick->identifyGeoID( pos_2nd.x(),
                                                                               pos_2nd.y(),
                                                                               pos_2nd.z() );
    if( nextGeoID_2nd!=curGeoID ) {
      // secondary was generated in this step and has
      // a different geoID than the currently tracked one

      if ( aTrack_2nd->GetKineticEnergy() < m_passBackEkinThreshold ) {
        // kinetic energy of secondary particle below threshold
        ATH_MSG_DEBUG(" -> Secondary particle generated in this G4Step does not pass Ekin cut." <<
                      " Not returned to ISF.");
        if ( m_killBoundaryParticlesBelowThreshold ) {
            // TODO: should we use fKillTrackAndSecondaries instead?
            aTrack_2nd->SetTrackStatus( fStopAndKill );
        } else {
            // TODO: link G4Track to ISF particle with the new GeoID
        }
      } else {
        // secondary particle is above kinetic energy threshold
        // -> return it to ISF
        ATH_MSG_DEBUG(" -> Secondary particle generated in this G4Step is returned to ISF.");
        ISF::ISFParticle *parent = curISP;
        returnParticleToISF(aTrack_2nd, parent, nextGeoID_2nd);
      }
    }

  } // <-- end loop over new secondaries

  return;
}

//_______________________________________________________________________
HepMC::GenParticle* iGeant4::TrackProcessorUserActionPassBack::findMatchingDaughter(HepMC::GenParticle* parent, bool verbose=false) const {
  // Add all necessary daughter particles
  if(NULL==parent->end_vertex()) {
    if(verbose) ATH_MSG_INFO ( "Number of daughters of "<<parent->barcode()<<": 0" );
    return parent;
  }
  const int pdgID(parent->pdg_id());
  for (HepMC::GenVertex::particles_out_const_iterator iter=parent->end_vertex()->particles_out_const_begin();
       iter!=parent->end_vertex()->particles_out_const_end(); ++iter){
    if (verbose) ATH_MSG_INFO ( "Daughter Particle of "<<parent->barcode()<<": " << **iter );
    if(NULL==(*iter)->end_vertex()) {
      if(verbose) ATH_MSG_INFO ( "Number of daughters of "<<(*iter)->barcode()<<": 0 (NULL)." );
    }
    else {
      if(verbose) ATH_MSG_INFO ("Number of daughters of "<<(*iter)->barcode()<<": " << (*iter)->end_vertex()->particles_out_size() );
    }
    if (pdgID == (*iter)->pdg_id()) {
      if (verbose) ATH_MSG_INFO ( "Look for daughters of Particle: " << (*iter)->barcode() );
      return this->findMatchingDaughter(*iter, verbose);
    }
  }
  if(!verbose) (void) this->findMatchingDaughter(parent, true);
  else {  ATH_MSG_ERROR ( "No matching Daughter Particles." ); }
  return parent;
}

//________________________________________________________________________
ISF::ITruthBinding*
iGeant4::TrackProcessorUserActionPassBack::newTruthBinding(const G4Track* aTrack) const
{
  ISF::ITruthBinding* tBinding = 0;

  VTrackInformation* trackInfo = ISFG4Helpers::getISFTrackInfo(*aTrack);

  if (trackInfo) {
    HepMC::GenParticle* genpart = const_cast<HepMC::GenParticle*>(trackInfo->GetHepMCParticle());
    if (genpart) {
      //find the last particle of this type in the decay chain - this is the one that we should pass back to ISF
      genpart = this->findMatchingDaughter(genpart);
      tBinding = new ISF::HepMC_TruthBinding(*genpart);
      // particle should be already known to McTruth Tree
      tBinding->setPersistency( true );
    }
  }

  return tBinding;
}

//________________________________________________________________________
ISF::ISFParticle*
iGeant4::TrackProcessorUserActionPassBack::attachNewISFParticle(G4Track* aTrack,
                                                                const ISF::ISFParticle* parent,
                                                                AtlasDetDescr::AtlasRegion  nextGeoID)
{
  ISF::ITruthBinding* tBinding = newTruthBinding(aTrack);
    
  ISF::ISFParticle* isp = ISFG4Helpers::convertG4TrackToISFParticle( *aTrack, *parent, tBinding );

  if (nextGeoID!=AtlasDetDescr::fUndefinedAtlasRegion) {
    isp->setNextGeoID( AtlasDetDescr::AtlasRegion(nextGeoID) );
    isp->setNextSimID( parent->nextSimID() );
  }

  // store new ISF particle in m_parentISPmap
  if (aTrack->GetTrackStatus()==fAlive) {
    int trackID  = aTrack->GetTrackID();
    ATH_MSG_VERBOSE("Setting ISFParticle to "<<isp<<" for trackID "<<trackID<<" (from new ISFParticle)");
    ISFG4Helpers::linkG4TrackToISFParticle( *aTrack, isp );
  }  

  return isp;
}


//________________________________________________________________________
void
iGeant4::TrackProcessorUserActionPassBack::returnParticleToISF( G4Track *aTrack,
                                                        ISF::ISFParticle *parentISP,
                                                        AtlasDetDescr::AtlasRegion nextGeoID )
{
  // kill track inside G4
  aTrack->SetTrackStatus( fStopAndKill );

  // create new ISFParticle and attach it to current G4Track
  ISF::ISFParticle *newISP = attachNewISFParticle(aTrack, parentISP, nextGeoID);

  // flag the track to let code downstream know that this track was returned to ISF
  VTrackInformation* trackInfo = dynamic_cast<VTrackInformation*>(aTrack->GetUserInformation());
  if (!trackInfo) {
    trackInfo = new TrackBarcodeInfo(Barcode::fUndefinedBarcode, newISP);
    aTrack->SetUserInformation(trackInfo);
  }
  trackInfo->SetReturnedToISF(true);


  // push the particle back to ISF
  m_particleBroker->push(newISP, parentISP);

  return;
}

bool iGeant4::TrackProcessorUserActionPassBack::checkVolumeDepth( G4LogicalVolume * lv , int volLevel , int d ) {
  if (lv==0) return false;
  bool Cavern = false;

  // Check the volumes rather explicitly
  if ( lv->GetName() == "BeamPipe::BeamPipe" ||
       lv->GetName() == "IDET::IDET" ||
       lv->GetName() == "CALO::CALO" ||
       lv->GetName() == "MUONQ02::MUONQ02" ||
       lv->GetName() == "TTR_BARREL::TTR_BARREL" ){
    if (d==volLevel){
      ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
    } else {
      ATH_MSG_ERROR("Volume " << lv->GetName() << " at depth " << d << " instead of depth " << volLevel);
      throw "WrongDepth";
    } // Check of volume level
  } else if ( lv->GetName()=="BeamPipe::BeamPipeCentral" ){ // Things that are supposed to be one deeper
    if (d==volLevel+1){
      ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
    } else {
      ATH_MSG_ERROR("Volume " << lv->GetName() << " at depth " << d << " instead of depth " << volLevel+1);
      throw "WrongDepth";
    } // Check of volume level
  } else if ( lv->GetName().find("CavernInfra")!=std::string::npos ){ // Things that are supposed to be one shallower
    if (d==volLevel-1){
      Cavern=true;
      ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
      // Note: a number of volumes exist with "CavernInfra" in the name at the wrong depth, so we just need to
      //   check that there's at least one at the right depth
    } // Check of volume level
  } // Check of volume name

  // Going through the volume depth
  for (int i=0; i<lv->GetNoDaughters(); ++i){
    Cavern = Cavern || checkVolumeDepth( lv->GetDaughter(i)->GetLogicalVolume() , volLevel , d+1 );
  }
  if (d==0 && !Cavern && volLevel>1){
    ATH_MSG_ERROR("No CavernInfra volume registered at depth " << volLevel-1);
    throw "WrongDepth";
  }
  return Cavern;
}
