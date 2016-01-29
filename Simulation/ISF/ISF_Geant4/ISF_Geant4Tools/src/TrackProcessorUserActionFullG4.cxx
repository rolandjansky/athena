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
#include "ISF_Event/ITruthBinding.h"
#include "ISF_Event/EntryLayer.h"

#include "ISF_HepMC_Event/HepMC_TruthBinding.h"

#include "ISF_Interfaces/IParticleBroker.h"

// Athena includes
#include "AtlasDetDescr/AtlasRegion.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

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
//#include "G4VPhysicalVolume.hh"

#include <iostream>

iGeant4::TrackProcessorUserActionFullG4::TrackProcessorUserActionFullG4(const std::string& type,
                                                                        const std::string& name,
                                                                        const IInterface* parent)
  : ITrackProcessorUserAction(name),
    TrackProcessorUserActionBase(name),
    AthAlgTool(type,name,parent),
    m_entryLayerTool(""),
    m_entryLayerToolQuick(0),
    m_hasCavern(true)
{

  ATH_MSG_DEBUG("create TrackProcessorUserActionFullG4 name: "<<name);

  declareInterface<ITrackProcessorUserAction>(this);

  declareProperty("EntryLayerTool", m_entryLayerTool, "ISF Entry Layer Tool");
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
      
      const ISF::DetRegionSvcIDPair detRegionSimSvcPair(nextGeoID, ISF::fUndefinedSimID );

      ISF::ISFParticle *tmpISP = ISFG4Helpers::convertG4TrackToISFParticle( *aTrack,
                                                                            detRegionSimSvcPair,
                                                                            curISP->getPrimaryTruthParticle(),
                                                                            nullptr // truthBinding
                                                                           );

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
  // Static so that it will keep the value from the previous step
  static AtlasDetDescr::AtlasRegion nextGeoID = m_truthVolLevel>1?AtlasDetDescr::fAtlasCavern:AtlasDetDescr::fUndefinedAtlasRegion;
  static const G4Track* aTrack = 0;
  StepHelper step(aStep);

  // Protect against being in a mother volume, then reset it to undefined if we don't know where we are
  //  Since we start particles in the beampipe (for the most part) this will rarely result in returning
  //  an undefined region answer; since G4 works on a stack rather than a queue, this is safe-ish
  if (step.PostStepBranchDepth()<m_truthVolLevel){
    if (m_truthVolLevel>1 &&
        step.PostStepBranchDepth()>m_truthVolLevel-1 && m_hasCavern &&
        step.GetPostStepLogicalVolumeName(m_truthVolLevel-1).find("CavernInfra") ) nextGeoID = AtlasDetDescr::fAtlasCavern;
    return nextGeoID;
  }
  if (aTrack != aStep->GetTrack()){
    // First step with this track!
    nextGeoID = AtlasDetDescr::fUndefinedAtlasRegion;
    aTrack = aStep->GetTrack();
  } // Otherwise use the cached value via the static

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
    checkVolumeDepth( G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume() , m_truthVolLevel );
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
    // We are in trouble
    //ATH_MSG_ERROR("vol1: "<<step.GetPostStepLogicalVolumeName(1)<<", vol2: "<<step.GetPostStepLogicalVolumeName(2)<<", postname="<<postname<<", returning undefined geoID");
    G4ThreeVector myPos = aStep->GetPostStepPoint()->GetPosition();
    ATH_MSG_ERROR("Returning undefined geoID from " << step.GetPostStepLogicalVolume() << " requesting " << step.GetPostStepLogicalVolume(m_truthVolLevel) << " at " << myPos.x() << " " << myPos.y() << " " << myPos.z() );
  }

  return nextGeoID;
}

bool iGeant4::TrackProcessorUserActionFullG4::checkVolumeDepth( G4LogicalVolume * lv , int volLevel , int d ) {
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
