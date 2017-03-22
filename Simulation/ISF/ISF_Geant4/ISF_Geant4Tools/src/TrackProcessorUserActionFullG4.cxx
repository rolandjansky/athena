/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackProcessorUserActionFullG4.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TrackProcessorUserActionFullG4.h"

// includes from this package
#include "ISFG4Helpers.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/EntryLayer.h"

#include "ISF_Interfaces/IParticleBroker.h"

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

iGeant4::TrackProcessorUserActionFullG4::TrackProcessorUserActionFullG4(const std::string& type,
                                                                        const std::string& name,
                                                                        const IInterface* parent)
  : TrackProcessorUserActionBase(type,name,parent),
    m_entryLayerTool(""),
    m_entryLayerToolQuick(nullptr),
    m_geoIDSvc("",name),
    m_geoIDSvcQuick(nullptr),
    m_hasCavern(true)
{

  ATH_MSG_DEBUG("create TrackProcessorUserActionFullG4 name: "<<name);

  declareProperty("EntryLayerTool",   m_entryLayerTool,  "ISF Entry Layer Tool"                        );
  declareProperty("GeoIDSvc",         m_geoIDSvc,        "ISF GeoIDService"                            );
  declareProperty("TruthVolumeLevel", m_truthVolLevel=1, "Level in geo hierarchy for the truth volumes");
}

StatusCode iGeant4::TrackProcessorUserActionFullG4::initialize()
{
  ATH_MSG_DEBUG("Initializing TrackProcessorUserActionFullG4");

  if ( !m_entryLayerTool.empty() ) {
    if ( m_entryLayerTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Could not retrieve ISF Entry Layer Tool: " << m_entryLayerTool);
      return StatusCode::FAILURE;
    }
    m_entryLayerToolQuick = &(*m_entryLayerTool);
  }

  if ( !m_geoIDSvc.empty() ) {
    if ( m_geoIDSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL("Could not retrieve ISF GeoID Svc: " << m_geoIDSvc);
      return StatusCode::FAILURE;
    }
    m_geoIDSvcQuick = &(*m_geoIDSvc);
  }

  m_entryLayerMap["CALO::CALO"]       = m_truthVolLevel+1;
  m_entryLayerMap["MUONQ02::MUONQ02"] = m_truthVolLevel+1;
  m_entryLayerMap["IDET::IDET"]       = m_truthVolLevel+1;

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::TrackProcessorUserActionFullG4::finalize()
{
  return StatusCode::SUCCESS;
}


void iGeant4::TrackProcessorUserActionFullG4::ISFSteppingAction(const G4Step* aStep, ISF::ISFParticle *curISP)
{
  G4Track*           aTrack   = aStep->GetTrack();
  int               aTrackID  = aTrack->GetTrackID();
  G4TrackStatus aTrackStatus  = aTrack->GetTrackStatus();

  const G4StepPoint *preStep  = aStep->GetPreStepPoint();
  const G4StepPoint *postStep = aStep->GetPostStepPoint();

  AtlasDetDescr::AtlasRegion nextG4GeoID = nextGeoId(aStep);
  if ( curISP->nextGeoID()!=nextG4GeoID ) {
    curISP->setNextGeoID( nextG4GeoID );
  }

  // check if dead track
  if ( aTrackStatus==fStopAndKill ) {
    ATH_MSG_DEBUG("Stepping dead G4Track, returning. TrackLength="<<aTrack->GetTrackLength()<<
                  " TrackEkin="<<aTrack->GetKineticEnergy()<<" TrackID="<<aTrackID);
    return;
  }

  const G4VPhysicalVolume *preVol  = preStep->GetPhysicalVolume();
  const G4VPhysicalVolume *postVol = postStep->GetPhysicalVolume();

  // check if particle left detector volume
  if ( postVol==0 ) {
    ATH_MSG_DEBUG("G4Step not in physical volume, returning. TrackLength="<<
                  aTrack->GetTrackLength()<<" TrackEkin="<<aTrack->GetKineticEnergy()<<
                  " TrackID="<<aTrackID);
    // left world
    return;
  }

  // check if particle is within same physical volume
  if ( preVol==postVol ) {
    ATH_MSG_DEBUG("G4Track stays inside current volume");
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

      AtlasDetDescr::AtlasRegion nextGeoID = nextGeoId(aStep);

      ISF::ISFParticle *tmpISP = ::iGeant4::ISFG4Helpers::convertG4TrackToISFParticle( *aTrack,
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

//________________________________________________________________________
ISF::EntryLayer
iGeant4::TrackProcessorUserActionFullG4::entryLayer(const G4Step* aStep)
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


//________________________________________________________________________
AtlasDetDescr::AtlasRegion
iGeant4::TrackProcessorUserActionFullG4::nextGeoId(const G4Step* aStep)
{
  static G4LogicalVolume * BPholder=0 , * IDholder=0 , * CALOholder=0 , * MUholder=0 , * TTRholder=0 ;
  if (BPholder==0){ // Initialize
    m_hasCavern=false;
    G4LogicalVolumeStore * lvs = G4LogicalVolumeStore::GetInstance();
    for (size_t i=0;i<lvs->size();++i){
      if ( !(*lvs)[i] ) continue;
      if ( (*lvs)[i]->GetName() == "BeamPipe::BeamPipe" ) BPholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "IDET::IDET" ) IDholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "CALO::CALO" ) CALOholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "MUONQ02::MUONQ02" ) MUholder = (*lvs)[i];
      else if ( (*lvs)[i]->GetName() == "TTR_BARREL::TTR_BARREL" ) TTRholder = (*lvs)[i];
    }
    this->checkVolumeDepth( G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume() , m_truthVolLevel );
  }

  // Static so that it will keep the value from the previous step
  static AtlasDetDescr::AtlasRegion nextGeoID = m_truthVolLevel>1?AtlasDetDescr::fAtlasCavern:AtlasDetDescr::fUndefinedAtlasRegion;
  static const G4Track* aTrack = 0;
  StepHelper step(aStep);

  if (aTrack != aStep->GetTrack()) {
    // First step with this track!
    nextGeoID = AtlasDetDescr::fUndefinedAtlasRegion;
    aTrack = aStep->GetTrack();
  } // Otherwise use the cached value via the static

  const G4StepPoint *postStep = aStep->GetPostStepPoint();

  bool leavingG4World       = postStep->GetStepStatus()==fWorldBoundary;
  bool simulatingCollisions = !m_hasCavern;
  if ( simulatingCollisions && leavingG4World ) {
    nextGeoID = AtlasDetDescr::fAtlasCavern;
    return nextGeoID;
  }

  // If in mother volume, use the ISF_GeoIDSvc to resolve the geoID
  if (step.PostStepBranchDepth()<m_truthVolLevel){
    nextGeoID = getNextGeoIDFromSvc(*postStep);
    return nextGeoID;
  }

  // Ordering inside out (most truth in the ID anyway...)
  if ( IDholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasID;
  } else if ( CALOholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasCalo;
  } else if ( MUholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasMS;
  } else if ( BPholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = (step.PostStepBranchDepth()>m_truthVolLevel && step.GetPostStepLogicalVolumeName(m_truthVolLevel+1)=="BeamPipe::BeamPipeCentral")?AtlasDetDescr::fAtlasID:AtlasDetDescr::fAtlasForward;
  } else if ( TTRholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasCavern;
  } else if (m_hasCavern && step.GetPostStepLogicalVolumeName(m_truthVolLevel-1).find("CavernInfra")!=std::string::npos) {
    nextGeoID = AtlasDetDescr::fAtlasCavern;
  } else {
    nextGeoID = getNextGeoIDFromSvc(*postStep);
  }

  // fall back to GeoIDSvc if unable to resolve nextGeoID up to this point
  if (nextGeoID == AtlasDetDescr::fUndefinedAtlasRegion) {
    nextGeoID = getNextGeoIDFromSvc(*postStep);
  }

  return nextGeoID;
}


AtlasDetDescr::AtlasRegion iGeant4::TrackProcessorUserActionFullG4::getNextGeoIDFromSvc(const G4StepPoint& postStep) const {
  const G4ThreeVector &postPos = postStep.GetPosition();
  AtlasDetDescr::AtlasRegion nextGeoID = m_geoIDSvcQuick->identifyGeoID(postPos.x(),
                                                                        postPos.y(),
                                                                        postPos.z());

  // if we ever run into problems with the current approach, the following
  // takes the particle's traveling direction into account for finding the
  // *next* volume it enters
  //const G4ThreeVector     &postMom  = postStep->GetMomentum();
  //nextGeoID = m_geoIDSvcQuick->identifyNextGeoID( postPos.x(),
  //                                                postPos.y(),
  //                                                postPos.z(),
  //                                                postMom.x(),
  //                                                postMom.y(),
  //                                                postMom.z() );

  return nextGeoID;
}


bool iGeant4::TrackProcessorUserActionFullG4::checkVolumeDepth( G4LogicalVolume * lv , int volLevel , int d ) {
  //FIXME - can replace all this code with similar methods to those in MCTruthBase/src/RecordingEnvelope.cxx

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
    m_hasCavern=true;
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

namespace G4UA{

  namespace iGeant4{

    TrackProcessorUserActionFullG4::TrackProcessorUserActionFullG4(const Config& config)
      : TrackProcessorUserActionBase(),m_config(config),
        m_entryLayerToolQuick(nullptr),
        m_geoIDSvcQuick(nullptr),
        m_hasCavern(true),
        m_currentTrack(nullptr),
        m_BPholder(nullptr), m_IDholder(nullptr), m_CALOholder(nullptr) , m_MUholder(nullptr), m_TTRholder(nullptr)
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

      m_hasCavern=false;
      G4LogicalVolumeStore * lvs = G4LogicalVolumeStore::GetInstance();
      for (size_t i=0;i<lvs->size();++i){
        if ( !(*lvs)[i] ) continue;
        if ( (*lvs)[i]->GetName() == "BeamPipe::BeamPipe" ) m_BPholder = (*lvs)[i];
        else if ( (*lvs)[i]->GetName() == "IDET::IDET" ) m_IDholder = (*lvs)[i];
        else if ( (*lvs)[i]->GetName() == "CALO::CALO" ) m_CALOholder = (*lvs)[i];
        else if ( (*lvs)[i]->GetName() == "MUONQ02::MUONQ02" ) m_MUholder = (*lvs)[i];
        else if ( (*lvs)[i]->GetName() == "TTR_BARREL::TTR_BARREL" ) m_TTRholder = (*lvs)[i];
      }
      this->checkVolumeDepth( G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume() , m_config.truthVolLevel );
    }

    void TrackProcessorUserActionFullG4::ISFSteppingAction(const G4Step* aStep, ISF::ISFParticle *curISP)
    {
      G4Track*           aTrack   = aStep->GetTrack();
      //int               aTrackID  = aTrack->GetTrackID(); // Only used in DEBUG messages
      G4TrackStatus aTrackStatus  = aTrack->GetTrackStatus();

      const G4StepPoint *preStep  = aStep->GetPreStepPoint();
      const G4StepPoint *postStep = aStep->GetPostStepPoint();

      AtlasDetDescr::AtlasRegion nextG4GeoID = nextGeoId(aStep);
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

          AtlasDetDescr::AtlasRegion nextGeoID = nextGeoId(aStep);

          ISF::ISFParticle *tmpISP = ::iGeant4::ISFG4Helpers::convertG4TrackToISFParticle( *aTrack,
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

    void TrackProcessorUserActionFullG4::preTracking(const G4Track* aTrack){
      // reset geoId, call upstream method
      m_nextGeoID = m_config.truthVolLevel>1?AtlasDetDescr::fAtlasCavern:AtlasDetDescr::fUndefinedAtlasRegion;
      m_currentTrack = aTrack;
      TrackProcessorUserActionBase::preTracking(aTrack);
      return;
    }

    AtlasDetDescr::AtlasRegion TrackProcessorUserActionFullG4::nextGeoId(const G4Step* aStep)
    {
      StepHelper step(aStep);

      if (m_currentTrack != aStep->GetTrack()) {
        // First step with this track!
        m_nextGeoID = AtlasDetDescr::fUndefinedAtlasRegion;
        m_currentTrack = aStep->GetTrack();
      } // Otherwise use the cached value via the member variable

      const G4StepPoint *postStep = aStep->GetPostStepPoint();

      bool leavingG4World       = postStep->GetStepStatus()==fWorldBoundary;
      bool simulatingCollisions = !m_hasCavern;
      if ( simulatingCollisions && leavingG4World ) {
        m_nextGeoID = AtlasDetDescr::fAtlasCavern;
        return m_nextGeoID;
      }

      // If in mother volume, use the ISF_GeoIDSvc to resolve the geoID
      if (step.PostStepBranchDepth()<m_config.truthVolLevel){
        m_nextGeoID = getNextGeoIDFromSvc(*postStep);
        return m_nextGeoID;
      }

      // Ordering inside out (most truth in the ID anyway...)
      if ( m_IDholder==step.GetPostStepLogicalVolume(m_config.truthVolLevel) ){
        m_nextGeoID = AtlasDetDescr::fAtlasID;
      } else if ( m_CALOholder==step.GetPostStepLogicalVolume(m_config.truthVolLevel) ){
        m_nextGeoID = AtlasDetDescr::fAtlasCalo;
      } else if ( m_MUholder==step.GetPostStepLogicalVolume(m_config.truthVolLevel) ){
        m_nextGeoID = AtlasDetDescr::fAtlasMS;
      } else if ( m_BPholder==step.GetPostStepLogicalVolume(m_config.truthVolLevel) ){
        m_nextGeoID = (step.PostStepBranchDepth()>m_config.truthVolLevel && step.GetPostStepLogicalVolumeName(m_config.truthVolLevel+1)=="BeamPipe::BeamPipeCentral")?AtlasDetDescr::fAtlasID:AtlasDetDescr::fAtlasForward;
      } else if ( m_TTRholder==step.GetPostStepLogicalVolume(m_config.truthVolLevel) ){
        m_nextGeoID = AtlasDetDescr::fAtlasCavern;
      } else if (m_hasCavern && step.GetPostStepLogicalVolumeName(m_config.truthVolLevel-1).find("CavernInfra")!=std::string::npos) {
        m_nextGeoID = AtlasDetDescr::fAtlasCavern;
      } else {
        m_nextGeoID = getNextGeoIDFromSvc(*postStep);
      }

      // fall back to GeoIDSvc if unable to resolve nextGeoID up to this point
      if (m_nextGeoID == AtlasDetDescr::fUndefinedAtlasRegion) {
        m_nextGeoID = getNextGeoIDFromSvc(*postStep);
      }

      return m_nextGeoID;
    }

    bool TrackProcessorUserActionFullG4::checkVolumeDepth( G4LogicalVolume * lv , int volLevel , int d ) {
      //FIXME - can replace all this code with similar methods to those in MCTruthBase/src/RecordingEnvelope.cxx

      if (lv==0) return false;
      bool Cavern = false;

      // Check the volumes rather explicitly
      if ( lv->GetName() == "BeamPipe::BeamPipe" ||
           lv->GetName() == "IDET::IDET" ||
           lv->GetName() == "CALO::CALO" ||
           lv->GetName() == "MUONQ02::MUONQ02" ||
           lv->GetName() == "TTR_BARREL::TTR_BARREL" ){
        if (d==volLevel){
          ;//ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
        } else {
          G4ExceptionDescription description;
          description << G4String("checkVolumeDepth: ") + "Volume " << lv->GetName() << " at depth " << d << " instead of depth " << volLevel;
          G4Exception("G4UA::iGeant4::TrackProcessorUserActionFullG4", "WrongDepth", FatalException, description);
          return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        } // Check of volume level
      } else if ( lv->GetName()=="BeamPipe::BeamPipeCentral" ){ // Things that are supposed to be one deeper
        if (d==volLevel+1){
          ;//ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
        } else {
          G4ExceptionDescription description;
          description << G4String("checkVolumeDepth: ") + "Volume " << lv->GetName() << " at depth " << d << " instead of depth " << volLevel+1;
          G4Exception("G4UA::iGeant4::TrackProcessorUserActionFullG4", "WrongDepth", FatalException, description);
          return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        } // Check of volume level
      } else if ( lv->GetName().find("CavernInfra")!=std::string::npos ){ // Things that are supposed to be one shallower
        m_hasCavern=true;
        if (d==volLevel-1){
          Cavern=true;
          //ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << d);
          // Note: a number of volumes exist with "CavernInfra" in the name at the wrong depth, so we just need to
          //   check that there's at least one at the right depth
        } // Check of volume level
      } // Check of volume name

      // Going through the volume depth
      for (int i=0; i<lv->GetNoDaughters(); ++i){
        Cavern = Cavern || checkVolumeDepth( lv->GetDaughter(i)->GetLogicalVolume() , volLevel , d+1 );
      }
      if (d==0 && !Cavern && volLevel>1){
        G4ExceptionDescription description;
        description << G4String("checkVolumeDepth: ") + "No CavernInfra volume registered at depth " << volLevel-1;
        G4Exception("G4UA::iGeant4::TrackProcessorUserActionFullG4", "WrongDepth", FatalException, description);
        return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
      }
      return Cavern;
    }

    AtlasDetDescr::AtlasRegion iGeant4::TrackProcessorUserActionFullG4::getNextGeoIDFromSvc(const G4StepPoint& postStep) const {
      const G4ThreeVector &postPos = postStep.GetPosition();
      AtlasDetDescr::AtlasRegion nextGeoID = m_geoIDSvcQuick->identifyGeoID(postPos.x(),
                                                                            postPos.y(),
                                                                            postPos.z());

      // if we ever run into problems with the current approach, the following
      // takes the particle's traveling direction into account for finding the
      // *next* volume it enters
      //const G4ThreeVector     &postMom  = postStep->GetMomentum();
      //nextGeoID = m_geoIDSvcQuick->identifyNextGeoID( postPos.x(),
      //                                                postPos.y(),
      //                                                postPos.z(),
      //                                                postMom.x(),
      //                                                postMom.y(),
      //                                                postMom.z() );

      return nextGeoID;
    }


  } // iGeant4

} //G4UA
