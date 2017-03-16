/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackProcessorUserActionPassBack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TrackProcessorUserActionPassBack.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/EntryLayer.h"

#include "ISF_Interfaces/IParticleBroker.h"

// ISF Geant4 includes
#include "ISF_Geant4Event/ISFG4Helper.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

// MCTruth includes
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/EventInformation.h"
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

namespace G4UA {

  namespace iGeant4{

    TrackProcessorUserActionPassBack::TrackProcessorUserActionPassBack(const Config& config):
      m_config(config),m_particleBrokerQuick(0), m_geoIDSvcQuick(0){

      if(4<m_config.verboseLevel)
        {
          G4cout << "Initializing TrackProcessorUserActionPassBack" << G4endl;
        }

      if ( !m_config.particleBroker.empty() ) {
        if (m_config.particleBroker.retrieve().isFailure()) {
          G4ExceptionDescription description;
          description << G4String("TrackProcessorUserActionPassBack: ") + "Could not retrieve ISF Particle Broker: " << m_config.particleBroker;
          G4Exception("iGeant4::TrackProcessorUserActionPassBack", "NoISFParticleBroker", FatalException, description);
          return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        }
        m_particleBrokerQuick = &(*m_config.particleBroker);
      }

      if ( !m_config.geoIDSvc.empty() ) {
        if (m_config.geoIDSvc.retrieve().isFailure()) {
          G4ExceptionDescription description;
          description << G4String("TrackProcessorUserActionPassBack: ") + "Could not retrieve ISF GeoID Svc: " << m_config.geoIDSvc;
          G4Exception("iGeant4::TrackProcessorUserActionPassBack", "NoISFGeoIDSvc", FatalException, description);
          return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        }

        m_geoIDSvcQuick = &(*m_config.geoIDSvc);
      }

      return;

    }

    void TrackProcessorUserActionPassBack::ISFSteppingAction(const G4Step* aStep,
                                                             ISF::ISFParticle *curISP)
    {
      G4Track*           aTrack   = aStep->GetTrack();
      G4TrackStatus aTrackStatus  = aTrack->GetTrackStatus();

      //const G4StepPoint *preStep  = aStep->GetPreStepPoint(); // Only used for DEBUG messages
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

      //ATH_MSG_DEBUG("PostStep point resolved to geoID = "<<nextGeoID);

      // return if particle did NOT cross boundary
      if ( nextGeoID==curGeoID ) {
        //ATH_MSG_DEBUG(" -> G4Track stays inside geoID = "<<curGeoID);

        // //
        // // for debugging:
        // //
        // if ( msgLvl(MSG::DEBUG) ) {
        //   const G4ThreeVector &prePos = preStep->GetPosition();
        //   AtlasDetDescr::AtlasRegion  preStepGeoID  = m_geoIDSvcQuick->identifyGeoID( prePos.x(),
        //                                                                               prePos.y(),
        //                                                                               prePos.z() );
        //   AtlasDetDescr::AtlasRegion  postStepGeoID = m_geoIDSvcQuick->identifyGeoID( postPos.x(),
        //                                                                               postPos.y(),
        //                                                                               postPos.z() );

        //   if( preStepGeoID!=postStepGeoID ) {
        //     const G4VPhysicalVolume     *preVol  = preStep->GetPhysicalVolume();
        //     const G4VPhysicalVolume     *postVol = postStep->GetPhysicalVolume();
        //     const G4ThreeVector         &preMom  = preStep->GetMomentum();
        //     const G4ThreeVector        &postMom  = postStep->GetMomentum();
        //     const G4TrackVector *secondaryVector = aStep->GetSecondary();
        //     const G4ThreeVector&      aTrack_pos = aTrack->GetPosition();
        //     const G4ThreeVector&      aTrack_mom = aTrack->GetMomentum();
        //     int pdgID=aTrack->GetDefinition()->GetPDGEncoding();
        //     int bc=0;
        //     int                         aTrackID = aTrack->GetTrackID();
        //     TrackInformation* trackInfo=dynamic_cast<TrackInformation*>(aTrack->GetUserInformation());
        //     if (trackInfo) {
        //       bc=trackInfo->GetParticleBarcode();
        //     }

        //     ATH_MSG_WARNING("pre "<<preVol->GetName()<<" x="<<prePos.x()<<" y="<<prePos.y()<<" z="<<prePos.z()<<" p="<<preMom.mag()<<" geoID="<<preStepGeoID<<"; post "<<postVol->GetName()<<" x="<<postPos.x()<<" y="<<postPos.y()<<" z="<<postPos.z()<<" p="<<postMom.mag()<<" geoID="<<nextGeoID<<"; length="<<aStep->GetStepLength()<<"; n2nd="<<secondaryVector->size()<<" track  x="<<aTrack_pos.x()<<" y="<<aTrack_pos.y()<<" z="<<aTrack_pos.z()<<" p="<<aTrack_mom.mag()<<" curgeoID="<<curGeoID<<" pdgid="<<pdgID<<" bc="<<bc<<" trackID="<<aTrackID<<" ISF="<<curISP<<"; ploss="<<(postMom-preMom).mag());
        //   }
        // }

        return;
      }


      //
      // this point is only reached if particle has crossed
      // a sub-det boundary in the non-Geant4-only mode
      //

      if ( aTrack->GetKineticEnergy() < m_config.passBackEkinThreshold ) {
        // kinetic energy of primary particle below threshold
        // ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
        //               " but is below Ekin threshold. Not returned to ISF.");
        if ( m_config.killBoundaryParticlesBelowThreshold ) {
          aTrack->SetTrackStatus( fStopAndKill );
        } else {
          // TODO: link G4Track to ISF particle with the new GeoID
        }
      } else if ( aTrackStatus!=fAlive ) {
        // particle is killed by G4 in this step
        // TODO: do we need to handle this case specifically?
        // ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
        //               " but is destroyed in this step. Not returned to ISF.");

      } else {
        // particle is above kinetic energy threshold and alive after this step
        // -> push new ISFParticle back to ISF particle broker
        // ATH_MSG_DEBUG(" -> G4Track enters geoID = " << nextGeoID <<
        //               " and is returned to ISF.");

        const ISF::ISFParticle*    parent = curISP;
        HepMC::GenParticle* truthParticle = m_eventInfo->GetCurrentlyTraced();
        this->returnParticleToISF(aTrack, parent, truthParticle, nextGeoID);
      }

      //
      // handle secondaries that were created in this G4Step
      //
      const std::vector<const G4Track*> *secondaryVector = aStep->GetSecondaryInCurrentStep();
      // loop over new secondaries
      for ( auto* aConstTrack_2nd : *secondaryVector ) {
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

          if ( aTrack_2nd->GetKineticEnergy() < m_config.passBackEkinThreshold ) {
            // kinetic energy of secondary particle below threshold
            // ATH_MSG_DEBUG(" -> Secondary particle generated in this G4Step does not pass Ekin cut." <<
            //               " Not returned to ISF.");
            if ( m_config.killBoundaryParticlesBelowThreshold ) {
              // TODO: should we use fKillTrackAndSecondaries instead?
              aTrack_2nd->SetTrackStatus( fStopAndKill );
            } else {
              // TODO: link G4Track to ISF particle with the new GeoID
            }
          } else {
            // secondary particle is above kinetic energy threshold
            // -> return it to ISF
            // ATH_MSG_DEBUG(" -> Secondary particle generated in this G4Step is returned to ISF.");

            // attach TrackInformation instance to the new secondary G4Track
            const ISF::ISFParticle *parent                  = curISP;
            HepMC::GenParticle* generationZeroTruthParticle = nullptr;
            ::iGeant4::ISFG4Helper::attachTrackInfoToNewG4Track( *aTrack_2nd,
                                                       *parent,
                                                       Secondary,
                                                       generationZeroTruthParticle );

            HepMC::GenParticle* truthParticle = nullptr;
            returnParticleToISF(aTrack_2nd, parent, truthParticle, nextGeoID_2nd);
          }
        }

      } // <-- end loop over new secondaries

      return;
    }

    ISF::TruthBinding* TrackProcessorUserActionPassBack::newTruthBinding(const G4Track* aTrack, HepMC::GenParticle* truthParticle) const
    {
      auto* trackInfo = ::iGeant4::ISFG4Helper::getISFTrackInfo(*aTrack);
      if (!trackInfo) {
        G4ExceptionDescription description;
        description << G4String("newTruthBinding: ") + "No TrackInformation associated with G4Track (trackID: "
                    << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                    << ", parentID " << aTrack->GetParentID() << ")";
        G4Exception("iGeant4::TrackProcessorUserActionPassBack", "NoTrackInformation", FatalException, description);
        return nullptr; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
      }

      HepMC::GenParticle*         primaryHepParticle = const_cast<HepMC::GenParticle*>(trackInfo->GetPrimaryHepMCParticle());
      HepMC::GenParticle*  generationZeroHepParticle = const_cast<HepMC::GenParticle*>(trackInfo->GetHepMCParticle());

      ISF::TruthBinding* tBinding = new ISF::TruthBinding(truthParticle, primaryHepParticle, generationZeroHepParticle);

      return tBinding;
    }

    ISF::ISFParticle* TrackProcessorUserActionPassBack::newISFParticle(G4Track* aTrack,
                                                                       const ISF::ISFParticle* parentISP,
                                                                       HepMC::GenParticle* truthParticle,
                                                                       AtlasDetDescr::AtlasRegion  nextGeoID)
    {
      ISF::TruthBinding* tBinding = newTruthBinding(aTrack, truthParticle);

      ISF::ISFParticle* isp = ::iGeant4::ISFG4Helper::convertG4TrackToISFParticle( *aTrack,
                                                                         *parentISP,
                                                                         tBinding );

      if (nextGeoID!=AtlasDetDescr::fUndefinedAtlasRegion) {
        isp->setNextGeoID( AtlasDetDescr::AtlasRegion(nextGeoID) );
        isp->setNextSimID( parentISP->nextSimID() );
      }

      return isp;
    }

    void TrackProcessorUserActionPassBack::returnParticleToISF( G4Track *aTrack,
                                                                const ISF::ISFParticle* parentISP,
                                                                HepMC::GenParticle* truthParticle,
                                                                AtlasDetDescr::AtlasRegion nextGeoID )
    {
      // kill track inside G4
      aTrack->SetTrackStatus( fStopAndKill );

      // create new ISFParticle and attach it to current G4Track
      ISF::ISFParticle *newISP = newISFParticle( aTrack, parentISP, truthParticle, nextGeoID );

      // update TrackInformation
      auto trackInfo = ::iGeant4::ISFG4Helper::getISFTrackInfo(*aTrack);
      trackInfo->SetReturnedToISF( true );
      trackInfo->SetBaseISFParticle( newISP );

      // push the particle back to ISF
      m_particleBrokerQuick->push(newISP, parentISP);

      return;
    }

  } // iGeant4


} // G4UA
