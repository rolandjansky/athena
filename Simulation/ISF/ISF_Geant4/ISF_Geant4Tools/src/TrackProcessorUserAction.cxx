/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

#include "SimHelpers/StepHelper.h"

#include "MCTruth/VTrackInformation.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackHelper.h"

#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ITruthBinding.h"
#include "ISF_Event/EntryLayer.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"


#include "ISF_Interfaces/IParticleBroker.h"

#include "ISF_HepMC_Event/HepMC_TruthBinding.h"

#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"
//#include "G4VPhysicalVolume.hh"

#include "ISF_Geant4Tools/TrackProcessorUserAction.h"

#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TruthStrategyManager.h"

#include <iostream>

iGeant4::TrackProcessorUserAction::TrackProcessorUserAction(const std::string& type,
                                                            const std::string& name,
                                                            const IInterface* parent)
  : ITrackProcessorUserAction(name),
    AthAlgTool(type,name,parent),
    m_particleBroker("ISF::ParticleBroker/ISF_ParticleStackService",name),
    m_geoIDSvc("ISF::GeoIDSvc/ISF_GeoIDSvc",name),
    m_geoIDSvcQuick(0),
    m_idR(0.), m_idZ(0.),
    m_caloRmean(0.),  m_caloZmean(0.),
    m_muonRmean(0.), m_muonZmean(0.),
    m_cavernRmean(0.),  m_cavernZmean(0.),
    m_volumeOffset(1),
    m_minHistoryDepth(0)
{

  ATH_MSG_DEBUG("create TrackProcessorUserAction name: "<<name);

  declareInterface<ITrackProcessorUserAction>(this);

  declareProperty("ParticleBroker", m_particleBroker, "ISF Particle Broker Svc");
  declareProperty("GeoIDSvc"      , m_geoIDSvc      , "ISF GeoID Svc"          );
  declareProperty("Geant4OnlyMode", m_geant4OnlyMode=false);
  declareProperty("TruthVolumeLevel", m_truthVolLevel=1, "Level in geo hierarchy for the truth volumes");

}

StatusCode iGeant4::TrackProcessorUserAction::initialize()
{
  ATH_MSG_DEBUG("Initializing TrackProcessorUserAction");

  if (m_particleBroker.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve ISF Particle Broker: " << m_particleBroker);
    return StatusCode::FAILURE;
  }

  if (m_geoIDSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve ISF GeoID Svc: " << m_geoIDSvc);
    return StatusCode::FAILURE;
  }
  m_geoIDSvcQuick = &(*m_geoIDSvc);


  m_entryLayerMap["CALO::CALO"] = m_truthVolLevel+1;
  m_entryLayerMap["MUONQ02::MUONQ02"] = m_truthVolLevel+1;
  m_entryLayerMap["IDET::IDET"] = m_truthVolLevel+1;

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::TrackProcessorUserAction::finalize()
{
  return StatusCode::SUCCESS;
}


void iGeant4::TrackProcessorUserAction::BeginOfRunAction(const G4Run*)
{
  // get the geometry manager and check how many layers are present.
  G4TransportationManager *transportationManager(G4TransportationManager::GetTransportationManager());
  G4LogicalVolume *world((*(transportationManager->GetWorldsIterator()))->GetLogicalVolume());
  ATH_MSG_VERBOSE("World G4LogicalVolume Name: " << world->GetName() << " has " << world->GetNoDaughters() << " daughters.");
  if ("World::World"==world->GetName())
    {
      ATH_MSG_INFO("Atlas::Atlas is not the world volume, so assume we are in a cosmics job.");
      //Cosmics-specific configuration.
      m_volumeOffset=2;
      m_minHistoryDepth=1;
    }
}

void iGeant4::TrackProcessorUserAction::EndOfRunAction(const G4Run*)
{

}

void iGeant4::TrackProcessorUserAction::BeginOfEventAction(const G4Event*)
{
  //std::cout<<"clearing ISFParticle map"<<std::endl;
  m_parentISPmap.clear();

}
void iGeant4::TrackProcessorUserAction::EndOfEventAction(const G4Event*)
{

}


void iGeant4::TrackProcessorUserAction::SteppingAction(const G4Step* aStep)
{

  G4Track* aTrack = aStep->GetTrack();

  // Dirty trick to speed up the lookup
  static int trackID=-999;
  static ISF::ISFParticle* isp=0;
  if (aTrack->GetTrackID()!=trackID){
    trackID=aTrack->GetTrackID();
    isp = m_parentISPmap[trackID];
    if (!isp) ATH_MSG_ERROR("no ISFParticle!");
  }

  //std::cout<<"retrieved isp "<<isp<<" for trackID "<<trackID<<std::endl;

  // reset nextGeoID for this particle so ISFTrajectory::AppendStep will have correct nextGeoID
  if (m_geant4OnlyMode) {
    AtlasDetDescr::AtlasRegion nextG4GeoID = nextGeoId(aStep);
    if(!isp) {
      G4ExceptionDescription description;
      description << G4String("SteppingAction: ") + "NULL ISFParticle pointer for current G4Step (trackID "
                  << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                  << ", parentID " << aTrack->GetParentID() << ")";
      G4Exception("iGeant4::TrackProcessorUserAction", "NoISFParticle", FatalException, description);
    } else if (isp->nextGeoID()!=nextG4GeoID) { isp->setNextGeoID( AtlasDetDescr::AtlasRegion(nextG4GeoID) ); }
  }

  // check if dead track
  if (aTrack->GetTrackStatus()==fStopAndKill) {
    ATH_MSG_DEBUG("dead particle, returning");
    return;
  }


  // check if particle left detector volume
  if (aStep->GetPostStepPoint()->GetPhysicalVolume()==0) {
    // left world
    return;
  }


  // Don't give particle to ISF if doing a zero step
  // or if very low energy, don't give particle to ISF
  if ( !m_geant4OnlyMode && ( ( aTrack->GetTrackLength() == 0.) ||
                              ( aTrack->GetKineticEnergy() < 0.05) ) ) {
    return;
  }


  //
  // check if track changed volume
  //


  if(!isp) {
    G4ExceptionDescription description;
    description << G4String("SteppingAction: ") + "NULL ISFParticle pointer for current G4Step (trackID "
                << aTrack->GetTrackID() << ", track pos: "<<aTrack->GetPosition() << ", mom: "<<aTrack->GetMomentum()
                << ", parentID " << aTrack->GetParentID() << ")";
    G4Exception("iGeant4::TrackProcessorUserAction", "NoISFParticle2", FatalException, description);
  }
  // get geoID from parent
  AtlasDetDescr::AtlasRegion curGeoID=isp?isp->nextGeoID():AtlasDetDescr::fUndefinedAtlasRegion;

  ATH_MSG_DEBUG("got geoID = "<<curGeoID);


  // Detector boundaries defined by central GeoIDSvc
  const G4StepPoint *preStep =aStep->GetPreStepPoint();
  const G4StepPoint *postStep=aStep->GetPostStepPoint();
  const G4VPhysicalVolume *preVol =preStep->GetPhysicalVolume();
  const G4VPhysicalVolume *postVol=postStep->GetPhysicalVolume();
  const G4ThreeVector &postPos  = postStep->GetPosition();
  const G4ThreeVector &postMom  = postStep->GetMomentum();
  AtlasDetDescr::AtlasRegion  nextGeoID =
    m_geoIDSvcQuick->identifyNextGeoID( postPos.x(), postPos.y(), postPos.z(), postMom.x(), postMom.y(), postMom.z() );

  ATH_MSG_DEBUG("next geoID = "<<nextGeoID);

  if ( (m_geant4OnlyMode && preVol==postVol) ||
       (!m_geant4OnlyMode && nextGeoID==curGeoID) ) {
    ATH_MSG_DEBUG("track stays inside "<<curGeoID);

    return;

  }


  //
  // have good particle, now process at boundary
  //


  if (!m_geant4OnlyMode) {

    // push new ISFParticle on stack
    ISF::ISFParticle* parent = isp;
    isp = newISFParticle(aTrack, parent, nextGeoID);

    int trackID = aTrack->GetTrackID();
    m_parentISPmap[trackID]=isp;
    //std::cout<<"set ISPmap for trackID "<<trackID<<" to "<<isp<<std::endl;

    m_particleBroker->push(isp, parent);

    // kill this track in Geant4
    aTrack->SetTrackStatus(fStopAndKill);
  }
  else {


    TrackHelper tHelp(aTrack);

    // only process particle at entry layer if primary or registered secondary

    if ( ! tHelp.IsSecondary() ) {

      // get entry layer
      ISF::EntryLayer layer = entryLayer(aStep);

      // update nextGeoID using Geant4 boundary definition
      nextGeoID = nextGeoId(aStep);

      bool newISP=false;
      if (layer!=ISF::fUnsetEntryLayer) {

        // easier to create a new ISF particle than update existing one
        ISF::ISFParticle* parent = isp;
        isp=newISFParticle(aTrack, parent, nextGeoID);
        m_parentISPmap[trackID]=isp;

        //std::cout<<"setting ISFParticle to "<<isp<<" for trackID "<<trackID<<" (G4-only  mode)"<<std::endl;

        newISP=true;

        m_particleBroker->registerParticle(isp, layer, newISP);
      }
      isp->setNextGeoID( AtlasDetDescr::AtlasRegion(nextGeoID) );
      const G4ThreeVector& particlePosition = aStep->GetPostStepPoint()->GetPosition();//Eigen compatibility HACK
      isp->updatePosition(Amg::Vector3D(particlePosition.x(), particlePosition.y(), particlePosition.z()));//Eigen compatibility HACK
      const G4ThreeVector& particleMomentum = aStep->GetPostStepPoint()->GetMomentum();//Eigen compatibility HACK
      isp->updateMomentum(Amg::Vector3D(particleMomentum.x(), particleMomentum.y(), particlePosition.z()));//Eigen compatibility HACK
    }

  }

  return;
}

void iGeant4::TrackProcessorUserAction::PostUserTrackingAction(const G4Track* /*aTrack*/)
{

  return;
}

void iGeant4::TrackProcessorUserAction::PreUserTrackingAction(const G4Track* aTrack)
{

  G4Track* inT = const_cast<G4Track*> (aTrack);
  TrackHelper trackHelper(inT);

  EventInformation* eventInfo = static_cast<EventInformation*>
    (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());

  // set associated ISF particle
  TrackHelper tHelp(aTrack);
  ISF::ISFParticle* isp = 0;
  TrackInformation* trackInfo=
    dynamic_cast<TrackInformation*>(aTrack->GetUserInformation());

  if (trackInfo && trackInfo->GetISFParticle()) {

    //std::cout<<"have trackInfo and ISFParticle"<<std::endl;

    isp = const_cast<ISF::ISFParticle*> (trackInfo->GetISFParticle());

    if (!isp) ATH_MSG_ERROR("no ISFParticle in trackInfo for trackID "<<aTrack->GetTrackID()<<"!");

    int trackID=aTrack->GetTrackID();
    if (m_parentISPmap.find(trackID)==m_parentISPmap.end()) {
      m_parentISPmap[trackID]=isp;
      //std::cout<<"setting ISFParticle to "<<isp<<" for trackID "<<trackID<<" (have trackInfo)"<<std::endl;
    }
  }
  else if (aTrack->GetParentID()) {
    //std::cout<<"no trackInfo or no ISFParticle"<<std::endl;

    int parentID=aTrack->GetParentID();
    isp = m_parentISPmap[parentID];

    //std::cout<<"got isp from parentID "<<parentID<<std::endl;

    int trackID=aTrack->GetTrackID();
    if (m_parentISPmap.find(trackID)==m_parentISPmap.end()) {
      m_parentISPmap[trackID]=isp;
      //std::cout<<"setting ISFParticle to "<<isp<<" for trackID "<<trackID<<std::endl;
    }

  }

  if (trackInfo && isp)
    trackInfo->SetISFParticle(const_cast<const ISF::ISFParticle*>(isp));


  if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary()) {
    HepMC::GenParticle* part=const_cast<HepMC::GenParticle*>(trackHelper.GetTrackInformation()->
                                                             GetHepMCParticle());

    if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);

    eventInfo->SetCurrentlyTraced(part);
  }

  // see if primary particle
  int pID = aTrack->GetParentID();
  bool isPrimary = !pID;

  if (!isPrimary) return;

  //
  // Get PrimaryParticleInformation from G4PrimaryParticle (assigned by TransportTool::addPrimaryVertex)
  // The ISFParticle should always exist, and the HepMC::GenParticle should exist if a primary EvGen particle (secondaries passed from G4 back to ISF for subsequent processing with G4 will have null pointer for HepMC::GenParticle)
  //

  PrimaryParticleInformation* ppInfo = dynamic_cast <PrimaryParticleInformation*> (aTrack->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation());

  // get parent
  const ISF::ISFParticle* parent = ppInfo?ppInfo->GetISFParticle():0;

  if (!parent) ATH_MSG_ERROR("no ISFParticle in PrimaryParticleInformation for "<<aTrack->GetTrackID()<<"!");

  // Set the event info properly
  eventInfo->SetCurrentISFPrimary(parent);

  // for particles passed to G4 from other simulations
  if (!m_geant4OnlyMode && !isp) {
    int trackID=aTrack->GetTrackID();
    m_parentISPmap[trackID]=const_cast<ISF::ISFParticle*>(parent);
  }

  return;
}


//________________________________________________________________________
ISF::ISFParticle*
iGeant4::TrackProcessorUserAction::newISFParticle(G4Track* aTrack,
                                                  const ISF::ISFParticle* parent,
                                                  AtlasDetDescr::AtlasRegion  nextGeoID)
{

  G4ParticleDefinition* particleDefinition = aTrack->GetDefinition();
  //      const G4DynamicParticle* dynamicParticle = aTrack->GetDynamicParticle();

  const G4ThreeVector g4pos = aTrack->GetPosition();
  const double gTime = aTrack->GetGlobalTime();
  const HepGeom::Point3D<double> position(g4pos.x(),g4pos.y(),g4pos.z());

  G4ThreeVector g4mom = aTrack->GetMomentum();
  const HepGeom::Vector3D<double> momentum(g4mom.x(),g4mom.y(),g4mom.z());

  double mass    = particleDefinition->GetPDGMass();
  double charge  = particleDefinition->GetPDGCharge();
  int    pdgID   = particleDefinition->GetPDGEncoding();


  //  *AS* check if TrackInformation is available
  ISF::ITruthBinding* tBinding=0;
  Barcode::ParticleBarcode barcode=Barcode::fUndefinedBarcode;
  VTrackInformation * trackInfo= static_cast<VTrackInformation*>(aTrack->GetUserInformation());
  if (trackInfo && (trackInfo->GetHepMCParticle() ||
                    dynamic_cast<TrackBarcodeInfo*>(trackInfo))) {
    barcode=trackInfo->GetParticleBarcode();
    HepMC::GenParticle* genpart= const_cast<HepMC::GenParticle*>(trackInfo->GetHepMCParticle());
    if (genpart) {
      tBinding=new ISF::HepMC_TruthBinding(*genpart);
      // particle should be already know to McTruth Tree
      tBinding->setPersistency( true);
    }
  }


  ISF::ISFParticle* isp= new ISF::ISFParticle( position,
                                               momentum,
                                               mass,
                                               charge,
                                               pdgID,
                                               gTime,
                                               *parent,
                                               barcode,
                                               tBinding );

  if (nextGeoID!=AtlasDetDescr::fUndefinedAtlasRegion) {
    isp->setNextGeoID( AtlasDetDescr::AtlasRegion(nextGeoID) );
    isp->setNextSimID( parent->nextSimID() );
  }

  if (trackInfo && trackInfo->GetISFParticle())
    trackInfo->SetISFParticle(isp);

  int trackID=aTrack->GetTrackID();
  m_parentISPmap[trackID]=isp;
  //std::cout<<"setting ISFParticle to "<<isp<<" for trackID "<<trackID<<" (from new ISFParticle)"<<std::endl;

  return isp;
}

//________________________________________________________________________
ISF::EntryLayer
iGeant4::TrackProcessorUserAction::entryLayer(const G4Step* aStep)
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
iGeant4::TrackProcessorUserAction::nextGeoId(const G4Step* aStep)
{
  // Static so that it will keep the value from the previous step
  static AtlasDetDescr::AtlasRegion nextGeoID = m_truthVolLevel>1?AtlasDetDescr::fAtlasCavern:AtlasDetDescr::fUndefinedAtlasRegion;

  StepHelper step(aStep);

  // Protect against being in a mother volume, then reset it to undefined if we don't know where we are
  //  Since we start particles in the beampipe (for the most part) this will rarely result in returning
  //  an undefined region answer; since G4 works on a stack rather than a queue, this is safe-ish
  if (step.PostStepBranchDepth()<m_truthVolLevel){
    if (m_truthVolLevel>1 &&
        step.GetPostStepLogicalVolumeName(m_truthVolLevel-1).find("CavernInfra") ) nextGeoID = AtlasDetDescr::fAtlasCavern;
    return nextGeoID;
  }
  nextGeoID = AtlasDetDescr::fUndefinedAtlasRegion;

  static G4LogicalVolume * BPholder=0 , * IDholder=0 , * CALOholder=0 , * MUholder=0 , * TTRholder=0 ;
  if (BPholder==0){ // Initialize
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
    nextGeoID = step.GetPostStepLogicalVolumeName(m_truthVolLevel+1)=="BeamPipe::BeamPipeCentral"?AtlasDetDescr::fAtlasID:AtlasDetDescr::fAtlasForward;
  } else if ( TTRholder==step.GetPostStepLogicalVolume(m_truthVolLevel) ){
    nextGeoID = AtlasDetDescr::fAtlasCavern;
  } else if (step.GetPostStepLogicalVolumeName(m_truthVolLevel-1).find("CavernInfra")!=std::string::npos) {
    nextGeoID = AtlasDetDescr::fAtlasCavern;
  } else {
    // We are in trouble
    //ATH_MSG_ERROR("vol1: "<<step.GetPostStepLogicalVolumeName(1)<<", vol2: "<<step.GetPostStepLogicalVolumeName(2)<<", postname="<<postname<<", returning undefined geoID");
    G4ThreeVector myPos = aStep->GetPostStepPoint()->GetPosition();
    ATH_MSG_ERROR("Returning undefined geoID from " << step.GetPostStepLogicalVolume() << " requesting " << step.GetPostStepLogicalVolume(m_truthVolLevel) << " at " << myPos.x() << " " << myPos.y() << " " << myPos.z() );
  }

  return nextGeoID;
}

bool iGeant4::TrackProcessorUserAction::checkVolumeDepth( G4LogicalVolume * lv , int volLevel , int d ) const {
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
