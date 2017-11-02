/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackProcessorUserActionFullG4.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TrackProcessorUserActionFullG4.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/EntryLayer.h"

#include "ISF_Interfaces/IParticleBroker.h"

// ISF Geant4 includes
#include "ISF_Geant4Event/ISFG4Helper.h"
#include "ISF_Geant4Event/ISFG4GeoHelper.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

#include "MCTruth/TrackHelper.h"

#include "SimHelpers/StepHelper.h"
#include "StoreGate/StoreGateSvc.h"

// Geant4 includes
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"

#include <iostream>

namespace G4UA{

  namespace iGeant4{

    TrackProcessorUserActionFullG4::TrackProcessorUserActionFullG4(const Config& config)
      : TrackProcessorUserActionBase(),m_config(config),
        m_entryLayerToolQuick(nullptr),
        m_geoIDSvcQuick(nullptr),
	m_currentTrack(nullptr)
    {
      
      if(4<m_config.verboseLevel)
        {
          G4cout << "create TrackProcessorUserActionFullG4" << G4endl;
        }

      m_nextGeoID = m_config.truthVolLevel>1?AtlasDetDescr::fAtlasCavern:AtlasDetDescr::fUndefinedAtlasRegion;

      if ( !m_config.entryLayerTool.empty() ) {
        if ( m_config.entryLayerTool.retrieve().isFailure()) {
          G4ExceptionDescription description;
          description << G4String("TrackProcessorUserActionFullG4: ") + "Could not retrieve ISF Entry Layer Tool: " << m_config.entryLayerTool;;
          G4Exception("G4UA::iGeant4::TrackProcessorUserActionFullG4", "NoISFEntryLayerTool", FatalException, description);
          return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        }
        m_entryLayerToolQuick = &(*m_config.entryLayerTool);
      }

      if ( !m_config.geoIDSvc.empty() ) {
        if (m_config.geoIDSvc.retrieve().isFailure()) {
          G4ExceptionDescription description;
          description << G4String("TrackProcessorUserActionFullG4: ") + "Could not retrieve ISF GeoID Svc: " << m_config.geoIDSvc;
          G4Exception("G4UA::iGeant4::TrackProcessorUserActionFullG4", "NoISFGeoIDSvc", FatalException, description);
          return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        }

        m_geoIDSvcQuick = &(*m_config.geoIDSvc);
      }

      m_entryLayerMap["CALO::CALO"]       = m_config.truthVolLevel+1;
      m_entryLayerMap["MUONQ02::MUONQ02"] = m_config.truthVolLevel+1;
      m_entryLayerMap["IDET::IDET"]       = m_config.truthVolLevel+1;

      ::iGeant4::ISFG4GeoHelper::checkVolumeDepth( G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume() , m_config.truthVolLevel);
    }

    void TrackProcessorUserActionFullG4::ISFSteppingAction(const G4Step* aStep, ISF::ISFParticle *curISP)
    {
      G4Track*           aTrack   = aStep->GetTrack();
      //int               aTrackID  = aTrack->GetTrackID(); // Only used in DEBUG messages
      G4TrackStatus aTrackStatus  = aTrack->GetTrackStatus();

      const G4StepPoint *preStep  = aStep->GetPreStepPoint();
      const G4StepPoint *postStep = aStep->GetPostStepPoint();

      
      AtlasDetDescr::AtlasRegion nextG4GeoID = ::iGeant4::ISFG4GeoHelper::nextGeoId(aStep,
										    m_config.
                                                                                    truthVolLevel,
                                                                                    m_geoIDSvcQuick);
      if ( curISP->nextGeoID()!=nextG4GeoID ) {
        curISP->setNextGeoID( nextG4GeoID );
      }

      // check if dead track
      if ( aTrackStatus==fStopAndKill ) {
        // ATH_MSG_DEBUG("Stepping dead G4Track, returning. TrackLength="<<aTrack->GetTrackLength()<<
        //               " TrackEkin="<<aTrack->GetKineticEnergy()<<" TrackID="<<aTrackID);
        return;
      }

      const G4VPhysicalVolume *preVol  = preStep->GetPhysicalVolume();
      const G4VPhysicalVolume *postVol = postStep->GetPhysicalVolume();

      // check if particle left detector volume
      if ( postVol==0 ) {
        // ATH_MSG_DEBUG("G4Step not in physical volume, returning. TrackLength="<<
        //               aTrack->GetTrackLength()<<" TrackEkin="<<aTrack->GetKineticEnergy()<<
        //               " TrackID="<<aTrackID);
        // left world
        return;
      }

      // check if particle is within same physical volume
      if ( preVol==postVol ) {
        // ATH_MSG_DEBUG("G4Track stays inside current volume");
        return;
      }

      //
      // this point is only reached if particle has crossed
      // a sub-det boundary in the Geant4-only mode
      //

      TrackHelper tHelp(aTrack);

      // only process particle at entry layer if primary or registered secondary

      if ( ! tHelp.IsSecondary() ) {

        // get entry layer
        ISF::EntryLayer layer = entryLayer(aStep);

        if (layer!=ISF::fUnsetEntryLayer) {


          AtlasDetDescr::AtlasRegion nextGeoID = ::iGeant4::ISFG4GeoHelper::nextGeoId(aStep,
                                                                                      m_config.truthVolLevel,
                                                                                      m_geoIDSvcQuick);

          ISF::ISFParticle *tmpISP = ::iGeant4::ISFG4Helper::convertG4TrackToISFParticle( *aTrack,
                                                                                *curISP,
                                                                                nullptr // truthBinding
                                                                                );
          tmpISP->setNextGeoID(nextGeoID);
          tmpISP->setNextSimID(ISF::fUndefinedSimID);

          auto generationZeroBarcode = tHelp.GetBarcode();
          tmpISP->setBarcode(generationZeroBarcode);

          tmpISP->setNextGeoID( nextGeoID );

          // inform the entry layer tool about this particle
          m_entryLayerToolQuick->registerParticle( *tmpISP, layer);

          delete tmpISP;
        }

      }

      return;
    }

    ISF::EntryLayer TrackProcessorUserActionFullG4::entryLayer(const G4Step* aStep)
    {
      //
      // this is the same prescription for getting the entry layer as in
      // MCTruth/MCTruthSteppingAction
      //

      const G4StepPoint *preStep =aStep->GetPreStepPoint();
      const G4StepPoint *postStep=aStep->GetPostStepPoint();

      G4TouchableHistory *preTHist=(G4TouchableHistory *)preStep->GetTouchable();
      G4TouchableHistory *postTHist=(G4TouchableHistory *)postStep->GetTouchable();
      int nLev1 = preTHist->GetHistoryDepth();
      int nLev2 = postTHist->GetHistoryDepth();

      std::map<std::string, int, std::less<std::string> >::const_iterator it;

      std::string vname1;
      bool pass=false;
      for (it=m_entryLayerMap.begin(); it!=m_entryLayerMap.end(); it++) {

        int il=(*it).second;

        if (il<=(nLev1+1)) {
          vname1=preTHist->GetVolume(nLev1-il+1)->GetName();

          if (vname1!=(*it).first) continue;

          if (il<=(nLev2+1)) {
            if (vname1==postTHist->GetVolume(nLev2-il+1)->GetName()) continue;
          }

          pass=true;
          break;
        }
      }


      ISF::EntryLayer layer=ISF::fUnsetEntryLayer;
      if (pass) {
        if      (vname1=="IDET::IDET")       layer=ISF::fAtlasCaloEntry;
        else if (vname1=="CALO::CALO")       layer=ISF::fAtlasMuonEntry;
        else if (vname1=="MUONQ02::MUONQ02") layer=ISF::fAtlasMuonExit;
      }

      return layer;
    }

    void TrackProcessorUserActionFullG4::PreUserTrackingAction(const G4Track* aTrack){
      // reset geoId, call upstream method
      m_nextGeoID = m_config.truthVolLevel>1?AtlasDetDescr::fAtlasCavern:AtlasDetDescr::fUndefinedAtlasRegion;
      m_currentTrack = aTrack;
      TrackProcessorUserActionBase::PreUserTrackingAction(aTrack);
      return;
    }

  } // iGeant4

} //G4UA
