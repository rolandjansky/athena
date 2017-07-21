/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PhysicsValidationUserAction.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "PhysicsValidationUserAction.h"

// package includes
#include "Geant4TruthIncident.h"

//ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ParticleClipboard.h"

#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/IParticleHelper.h"

//Athena includes
#include "AtlasDetDescr/AtlasRegion.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/VTrackInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"

// Geant4 includes
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"

//External includes
#include "HepMC/GenVertex.h"
// ROOT includes
#include "TTree.h"
// STL includes
#include <iostream>

iGeant4::PhysicsValidationUserAction::PhysicsValidationUserAction(const std::string& type,
                                                                  const std::string& name,
                                                                  const IInterface* parent)
  : UserActionBase(type,name,parent),
    m_UASvc("UserActionSvc",name),
    m_particleBroker("ISF::ParticleBroker/ISF_ParticleStackService",name),
    m_particleHelper("ISF::ParticleHelper/ISF_ParticleHelper"),
    m_geoIDSvc("ISF::GeoIDSvc/ISF_GeoIDSvc",name),
    m_geoIDSvcQuick(0),
    m_validationOutput(true),
    m_thistSvc("THistSvc",name),
    //m_validationStream("ISFMaterial"),
    m_validationStream("ISFG4SimKernel"),
    // branches
    m_wzOaTr(0), m_thIn(0), m_phIn(0), m_dIn(0),
    m_thEnd(0), m_phEnd(0), m_dEnd(0),
    m_X0(0), m_L0(0), m_wZ(0), m_dt(0),
    // more branches
    m_interactions(nullptr),
    m_process(0), m_pdg_mother(0), m_gen_mother(0), m_nChild(0),
    m_vtx_dist(0), m_vtx_theta(0), m_vtx_phi(0), m_vtx_e_diff(0),
    m_vtx_p_diff(0), m_vtx_plong_diff(0), m_vtx_pperp_diff(0),
    m_p_mother(0), m_radLength(0),
    //Values taken from G4DetectorEnvelopes/EnvelopeGeometryManager.h
    m_idR(1150.-1.e-5), m_idZ(3490.),
    m_caloRmean(0.5*(40.+4250.)),  m_caloZmean(0.5*(3490.+6740.)),
    m_muonRmean(0.5*(60.+30000.)), m_muonZmean(0.5*(6740.+30000.)),
    m_cavernRmean(300000.0),  m_cavernZmean(300000.0),
    m_volumeOffset(1),
    m_minHistoryDepth(0),
    m_currentTrack(0)
{

  ATH_MSG_DEBUG("create PhysicsValidationUserAction name: "<<name);

  //declareInterface<IUserAction>(this);

  // validation output section
  declareProperty( "ValidationOutput",
                   m_validationOutput = true,
                   "If turned on, write out a ROOT tree.");
  declareProperty("ValidationStreamName",
                  m_validationStream = "ISFG4SimKernel",
                  "Name of the output stream" );
  declareProperty("THistService",
                  m_thistSvc,
                  "The THistSvc" );
  declareProperty("ParticleBroker", m_particleBroker, "ISF Particle Broker Svc");
  declareProperty("ParticleHelper", m_particleHelper, "ISF Particle Helper"    );
  declareProperty("GeoIDSvc"      , m_geoIDSvc      , "ISF GeoID Svc"          );
  declareProperty("UserActionSvc",m_UASvc);
  declareProperty("ID_Rmax",m_idR);
  declareProperty("ID_Zmax",m_idZ);
  declareProperty("CaloRmean",m_caloRmean);
  declareProperty("CaloZmean",m_caloZmean);
  declareProperty("MuonRmean",m_muonRmean);
  declareProperty("MuonZmean",m_muonZmean);
  declareProperty("CavernRmean",m_cavernRmean);
  declareProperty("CavernZmean",m_cavernZmean);

}

StatusCode iGeant4::PhysicsValidationUserAction::initialize()
{
  ATH_MSG_DEBUG("Initializing PhysicsValidationUserAction");

  if (m_particleBroker.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve ISF Particle Broker: " << m_particleBroker);
    return StatusCode::FAILURE;
  }

  if (m_particleHelper.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve " << m_particleHelper->name());
    return StatusCode::FAILURE;
  }

  if (m_geoIDSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve ISF GeoID Svc: " << m_geoIDSvc);
  }

  if (m_particleBroker.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve stackSvc");
    return StatusCode::FAILURE;
  }

  if (m_UASvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve UserActionSvc");
    return StatusCode::FAILURE;
  }


  m_sHelper=SecondaryTracksHelper(m_UASvc->TrackingManager());

  m_geoIDSvcQuick = &(*m_geoIDSvc);

  // setup for validation mode
  if ( m_validationOutput) {

    // retrieve the histogram service
    if ( m_thistSvc.retrieve().isSuccess() ) {
      // Create the prefix of histogram names for the THistSvc
      const char *treeName="particles";
      const std::string prefix = "/" + m_validationStream + "/"+ treeName;
      m_particles = new TTree( treeName, treeName );
      m_particles->Branch("pdg"       , &m_pdg    , "pdg/I"         );         // pdg id
      m_particles->Branch("scIn"      , &m_scIn   , "scIn/I"         );        // input process
      m_particles->Branch("scOut"     , &m_scEnd  , "scOut/I"         );       // endpoint process
      m_particles->Branch("gen"       , &m_gen    , "gen/I"         );         // generation (0 for primary)
      m_particles->Branch("geoID"     , &m_geoID  , "geoID/I"         );       // subdetector id
      m_particles->Branch("pth"       , &m_pth     , "pth/F"            );     // input momentum polar angle
      m_particles->Branch("pph"       , &m_pph     , "pph/F"            );     // input momemtum azimuthal angle
      m_particles->Branch("p"         , &m_p      , "p/F"            );        // input momentum
      m_particles->Branch("eloss"     , &m_eloss  , "eloss/F"            );    // energy loss
      m_particles->Branch("radloss"     , &m_radloss  , "radloss/F"            );   // radiative eloss
      m_particles->Branch("ionloss"     , &m_ionloss  , "ionloss/F"            );   // ionization eloss
      m_particles->Branch("wzOaTr"     , &m_wzOaTr  , "wzOaTr/F"            );      // zOverZtimesRho times dInX0
      m_particles->Branch("X0"        , &m_X0     , "X0/F"            );            // dInX0 (material thickness)
      m_particles->Branch("L0"        , &m_L0     , "L0/F"            );            // dInL0
      m_particles->Branch("wZ"        , &m_wZ     , "wZ/F"            );            // averageZ time dInX0
      m_particles->Branch("dt"        , &m_dt     , "dt/F"            );            // time interval
      m_particles->Branch("thIn"      , &m_thIn   , "thIn/F"            );          // polar angle input position
      m_particles->Branch("phIn"      , &m_phIn   , "phIn/F"            );          // azimuthal angle input position
      m_particles->Branch("dIn"       , &m_dIn    , "dIn/F"            );           // distance input position
      m_particles->Branch("thEnd"     , &m_thEnd  , "thEnd/F"            );         // polar angle exit position
      m_particles->Branch("phEnd"     , &m_phEnd  , "phEnd/F"            );         // azimuthal angle exit position
      m_particles->Branch("dEnd"      , &m_dEnd   , "dEnd/F"            );          // distance exit position

      // register the Tree to the THistSvc and return it's StatusCode
      ATH_CHECK(m_thistSvc->regTree( prefix, m_particles) );

      m_X0=0.;
      m_L0=0.;
      m_wZ=0.;
      m_wzOaTr=0.;

      const char *treeNameInt="interactions";
      const std::string prefixInt = "/" + m_validationStream + "/"+ treeNameInt;
      m_interactions = new TTree( treeNameInt, treeNameInt );
      m_interactions->Branch("process"     , &m_process  , "process/I"         );
      m_interactions->Branch("pdg_mother"  , &m_pdg_mother  , "pdg_mother/I"         );
      m_interactions->Branch("gen_mother"  , &m_gen_mother  , "gen_mother/I"         );
      m_interactions->Branch("nChild"      , &m_nChild      , "nch/I"         );
      m_interactions->Branch("vtx_dist"    , &m_vtx_dist    , "vtx_dist/F"         );
      m_interactions->Branch("vtx_theta"   , &m_vtx_theta   , "vtx_theta/F"         );
      m_interactions->Branch("vtx_phi"     , &m_vtx_phi     , "vtx_phi/F"         );
      m_interactions->Branch("vtx_e_diff"  , &m_vtx_e_diff  , "vtx_e_diff/F"         );
      m_interactions->Branch("vtx_p_diff"  , &m_vtx_p_diff  , "vtx_p_diff/F"         );
      m_interactions->Branch("vtx_plong_diff"  , &m_vtx_plong_diff  , "vtx_plong_diff/F"         );
      m_interactions->Branch("vtx_pperp_diff"  , &m_vtx_pperp_diff  , "vtx_pperp_diff/F"         );
      m_interactions->Branch("radLength"    , &m_radLength     , "radLength/F"         );
      m_interactions->Branch("p_mother"    , &m_p_mother    , "p_mother/F"         );
      m_interactions->Branch("pdg_child"   , m_pdg_child    , "pdg_child[nch]/I"         );
      m_interactions->Branch("fp_child"    , m_fp_child     , "fp_child[nch]/F"         );
      m_interactions->Branch("oa_child"    , m_oa_child     , "oa_child[nch]/F"         );
      // register the Tree to the THistSvc and return it's StatusCode
      ATH_CHECK(m_thistSvc->regTree( prefixInt, m_interactions) );

      m_radLength = 0.;

    }
  }

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::PhysicsValidationUserAction::finalize()
{
  return StatusCode::SUCCESS;
}


void iGeant4::PhysicsValidationUserAction::BeginOfRun(const G4Run*)
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

void iGeant4::PhysicsValidationUserAction::BeginOfEvent(const G4Event*)
{

  ATH_MSG_VERBOSE("m_idRmax: "<<m_idR      <<", m_idZmax: "<<m_idZ);
  ATH_MSG_VERBOSE("m_caloR : "<<m_caloRmean<<", m_caloZ : "<<m_caloZmean);
  ATH_MSG_VERBOSE("m_muonR : "<<m_muonRmean<<", m_muonZ : "<<m_muonZmean);
  ATH_MSG_VERBOSE("m_muonR : "<<m_cavernRmean<<", m_muonZ : "<<m_cavernZmean);
  ATH_MSG_VERBOSE("m_cavernR : "<<m_cavernRmean<<", m_cavernZ : "<<m_cavernZmean);

  m_currentTrack = -1;
  m_trackGenMap.clear();
  //m_idToStackParticleMap.clear();

}

void iGeant4::PhysicsValidationUserAction::EndOfEvent(const G4Event*)
{
  m_X0=0.;
  m_L0=0.;
  m_wZ=0.;
  m_wzOaTr=0.;

  m_radLength=0;
}


void iGeant4::PhysicsValidationUserAction::Step(const G4Step* aStep)
{
  //std::cout<<"PhysicsValidationUserAction::SteppingAction"<<std::endl;

  // process identification
  const G4VProcess * process = aStep->GetPostStepPoint()->GetProcessDefinedStep();

  // material info
  G4StepPoint *preStep=aStep->GetPreStepPoint();
  G4StepPoint *postStep=aStep->GetPostStepPoint();

  // parent
  //const ISF::ISFParticle* parent= ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  //assert(parent);

  G4ThreeVector mom = preStep->GetMomentum();
  G4ThreeVector pos = preStep->GetPosition();

  // info about generating particle
  G4Track * track = aStep->GetTrack();

  int trackID=track->GetTrackID();

  //std::cout <<"processing track:"<< trackID<<":"<< mom.mag()<< std::endl;

  if (trackID != m_currentTrack) {     // for new G4Track only

    m_pdg = track->GetDefinition()->GetPDGEncoding();
    const G4VProcess* creation = track->GetCreatorProcess();
    m_scIn = creation? creation->GetProcessSubType() : -1;

    VTrackInformation * trackInfo= static_cast<VTrackInformation*>(track->GetUserInformation());
    HepMC::GenParticle* genpart= trackInfo ? const_cast<HepMC::GenParticle*>(trackInfo->GetHepMCParticle()):0;
    HepMC::GenVertex* vtx = genpart ? genpart->production_vertex() : 0;
    m_gen = genpart? 0 : -1;

    if (genpart)  { // mc truth known
      while (genpart && vtx ) {
        int pdgID=genpart->pdg_id();
        HepMC::GenParticle* genmom = vtx->particles_in_size()>0 ? *(vtx->particles_in_const_begin()) : 0;
        if ( genmom && pdgID!=genmom->pdg_id() ) m_gen++;
        else if (vtx->particles_out_size()>0 && genpart!=*(vtx->particles_out_const_begin())) m_gen++;
        vtx = genmom ? genmom->production_vertex() : 0;
        genpart = genmom;
      }
    } else {
      // retrieve info from parent track
      int parentID=track->GetParentID();
      std::map<int, int>::iterator genIt = m_trackGenMap.find(parentID);
      if ( genIt != m_trackGenMap.end()) m_gen = (genIt->second >= 0) ? genIt->second+1 : genIt->second-1;
    }

    if (m_trackGenMap.find(trackID)==m_trackGenMap.end())  m_trackGenMap[trackID]=m_gen;

    m_currentTrack=trackID;

    m_radLength = 0.;
  }

  AtlasDetDescr::AtlasRegion geoID = m_geoIDSvcQuick->identifyNextGeoID( pos.x(),
                                                                         pos.y(),
                                                                         pos.z(),
                                                                         mom.x(),
                                                                         mom.y(),
                                                                         mom.z() );



  double stepLength = aStep->GetStepLength();
  double radLengthInX0  = preStep->GetMaterial()->GetRadlen();
  double l0  = preStep->GetMaterial()->GetNuclearInterLength();
  float stepInX0   = stepLength/radLengthInX0;

  if (stepInX0>1.e-06) {

    m_X0 += stepInX0;
    m_radLength += stepInX0;

    if (l0>0.) {
      m_L0 += stepLength/l0;
      // average Z/A over fraction of atoms rather than weight fraction
      // const G4double* fVec = preStep->GetMaterial()->GetFractionVector();      // mass fraction
      double totNA =  preStep->GetMaterial()->GetTotNbOfAtomsPerVolume();
      const G4ElementVector* eVec = preStep->GetMaterial()->GetElementVector();
      const G4double* atVector = preStep->GetMaterial() ->GetVecNbOfAtomsPerVolume();
      float mFactor =stepInX0* preStep->GetMaterial()->GetDensity();

      float zOverA = 0.;    float frSum = 0.;
      for (unsigned int i=0; i<eVec->size(); i++) {
        float fEl = atVector ?  atVector[i]/totNA : 0.;
        m_wZ += stepInX0*fEl*((*eVec)[i]->GetZ());
        //std::cout <<"elements:"<<i<<","<<fVec[i]<<":"<<(*eVec)[i]->GetZ()<< ","<<m_wZ<<","<<m_wZ/m_X0<<std::endl;
        //m_wA += stepInX0*fVec[i]*((*eVec)[i]->GetA());
        zOverA += fEl*((*eVec)[i]->GetZ())/((*eVec)[i]->GetA());
        frSum += fEl;
      }
      if (fabs(frSum-1.)>0.01)  ATH_MSG_DEBUG("G4 material description inconsistent, sum of element fractions:"<< frSum);
      m_wzOaTr += mFactor*zOverA;
    }

  }


    // save interaction info (if any)
    if ( process && process->GetProcessSubType()>0 && process->GetProcessSubType()!=91) {

    float eloss = postStep->GetMomentum().mag()-preStep->GetMomentum().mag();

    if (process->GetProcessSubType()==2 ) m_ionloss+=eloss;
    if (process->GetProcessSubType()==3 ) m_radloss+=eloss;

    EventInformation* eventInfo = static_cast<EventInformation*> (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
    VTrackInformation * trackInfo = static_cast<VTrackInformation*>(track->GetUserInformation());
    const auto baseISP = const_cast<ISF::ISFParticle*>( trackInfo->GetBaseISFParticle() );
    iGeant4::Geant4TruthIncident truth( aStep, *baseISP, geoID, m_sHelper.NrOfNewSecondaries(), m_sHelper, eventInfo);
    unsigned int nSec = truth.numberOfChildren();
    if (nSec>0 || track->GetTrackStatus()!=fAlive ) {      // save interaction info
      //std::cout <<"interaction:"<< process->GetProcessSubType() <<":"<<nSec<< std::endl;
      m_process=process->GetProcessSubType();
      m_pdg_mother = track->GetDefinition()->GetPDGEncoding();
      //VTrackInformation * trackInfo= static_cast<VTrackInformation*>(track->GetUserInformation());
      //m_gen_mother = (trackInfo && (trackInfo->GetHepMCParticle() ||
      //                            dynamic_cast<TrackBarcodeInfo*>(trackInfo))) ?
      //int(trackInfo->GetParticleBarcode()/1.e08) : -1;
      m_gen_mother = m_gen;
      G4ThreeVector mom = preStep->GetMomentum();
      m_p_mother   = mom.mag();

      m_vtx_dist    = postStep->GetPosition().mag();
      m_vtx_theta   = postStep->GetPosition().theta();
      m_vtx_phi     = postStep->GetPosition().phi();

      int iPrimSurv = track->GetTrackStatus()!=fAlive ? 0 : 1;
      m_nChild     = nSec+iPrimSurv;

      G4ThreeVector pbal(mom);

      if (iPrimSurv>0) {
        m_pdg_child[0] = m_pdg_mother;
        m_fp_child[0] = postStep->GetMomentum().mag()/m_p_mother;
        m_oa_child[0] = mom*postStep->GetMomentum()/m_p_mother/postStep->GetMomentum().mag();
        pbal -= postStep->GetMomentum();
      }

      unsigned int nSecMax = nSec+iPrimSurv> MAXCHILDREN ? MAXCHILDREN-iPrimSurv : nSec;
      for (unsigned int isec=0; isec< nSec; isec++) {
        G4ThreeVector secMom = truth.childP(isec);
        if (isec<nSecMax) {
          m_pdg_child[isec+iPrimSurv] = truth.childPdgCode(isec);
          m_fp_child[isec+iPrimSurv] = secMom.mag()/m_p_mother;
          m_oa_child[isec+iPrimSurv] = secMom*mom/m_p_mother/secMom.mag();
        }
        pbal -= secMom;
      }

      m_vtx_p_diff = pbal.mag();
      m_vtx_plong_diff = pbal*mom/m_p_mother;
      m_vtx_pperp_diff = sqrt(m_vtx_p_diff*m_vtx_p_diff-m_vtx_plong_diff*m_vtx_plong_diff);

      m_interactions->Fill();

      // reset the radiation length
      if (m_process==3) m_radLength = 0.;
    }
  }

  // crossing subdetector boundary ?
  G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();
  G4VPhysicalVolume *postVol=postStep->GetPhysicalVolume();

  if (postVol==0) {
    // left world -save info
    m_scEnd = 0;
    m_geoID = geoID;
    m_dt = track->GetLocalTime();

    m_pth = track->GetVertexMomentumDirection().theta();
    m_pph = track->GetVertexMomentumDirection().phi();
    m_p   = track->GetVertexKineticEnergy();
    m_eloss = track->GetKineticEnergy()-m_p;

    m_thIn= track->GetVertexPosition().theta();
    m_phIn= track->GetVertexPosition().phi();
    m_dIn=  track->GetVertexPosition().mag();

    m_thEnd=postStep->GetPosition().theta();
    m_phEnd=postStep->GetPosition().phi();
    m_dEnd=postStep->GetPosition().mag();

    m_particles->Fill();
    m_X0 = 0.;
    m_L0 = 0.;
    m_wZ = 0.;
    m_wzOaTr = 0.;

    m_radloss = 0.;
    m_ionloss = 0.;

    return;
  }

  // if particle killed, save the info
  if ( track->GetTrackStatus()!=fAlive ) {
    m_scEnd = process? process->GetProcessSubType() : -1;
    m_geoID = geoID;
    m_dt = track->GetLocalTime();

    m_pth = track->GetVertexMomentumDirection().theta();
    m_pph = track->GetVertexMomentumDirection().phi();
    m_p   = track->GetVertexKineticEnergy();
    m_eloss = track->GetKineticEnergy()-m_p;

    m_thIn= track->GetVertexPosition().theta();
    m_phIn= track->GetVertexPosition().phi();
    m_dIn=  track->GetVertexPosition().mag();

    m_thEnd=postStep->GetPosition().theta();
    m_phEnd=postStep->GetPosition().phi();
    m_dEnd=postStep->GetPosition().mag();

    m_particles->Fill();
    m_X0 = 0.;
    m_L0 = 0.;
    m_wZ = 0.;
    m_radloss = 0.;
    m_ionloss = 0.;
    m_wzOaTr = 0.;
    m_radLength = 0.;
  }

  if ( preVol==postVol ) return;      // assume boundaries do not cross G4Volumes

  // Detector boundaries defined by central GeoIDSvc
  const G4ThreeVector &postPos  = postStep->GetPosition();
  const G4ThreeVector &postMom  = postStep->GetMomentum();
  AtlasDetDescr::AtlasRegion  nextGeoID = m_geoIDSvcQuick->identifyNextGeoID( postPos.x(),
                                                                              postPos.y(),
                                                                              postPos.z(),
                                                                              postMom.x(),
                                                                              postMom.y(),
                                                                              postMom.z() );

  // save info if leaving the subdetector
  if ( nextGeoID == geoID) {
    ATH_MSG_DEBUG("track stays inside "<<geoID);
  } else {
    ATH_MSG_DEBUG("track moves from "<<geoID<<" to "<<nextGeoID);

    // Don't save if doing a zero step ?
    //if (aStep->GetTrack()->GetTrackLength() == 0.) {
    if (aStep->GetStepLength() == 0.) {
      return;
    }

    // don't change geometry assignment for validation ntuple
    m_geoID = geoID;

    //4ParticleDefinition* particleDefinition = track->GetDefinition();

    const G4ThreeVector g4pos = track->GetPosition();
    //const double gTime = track->GetGlobalTime();
    const HepGeom::Point3D<double> position(g4pos.x(),g4pos.y(),g4pos.z());

    G4ThreeVector g4mom = track->GetMomentum();
    const HepGeom::Vector3D<double> momentum(g4mom.x(),g4mom.y(),g4mom.z());

    //double mass    = particleDefinition->GetPDGMass();
    //double charge  = particleDefinition->GetPDGCharge();
    //int    pdgID   = particleDefinition->GetPDGEncoding();

    if (m_particleBroker) {

      // *AS* why ask stackSvc for current(), shouldn't better the TransportTool keep track?
      bool dead=false;
      if (track->GetTrackStatus()==fStopAndKill) {
        dead=true;
      }

      if (!dead) {

        // track info
        //VTrackInformation * trackInfo= static_cast<VTrackInformation*>(track->GetUserInformation());
        m_scEnd = 0;
        m_dt = track->GetLocalTime();

        m_pth = track->GetVertexMomentumDirection().theta();
        m_pph = track->GetVertexMomentumDirection().phi();
        m_p   = track->GetVertexKineticEnergy();
        m_eloss = track->GetKineticEnergy()-m_p;

        m_thIn= track->GetVertexPosition().theta();
        m_phIn= track->GetVertexPosition().phi();
        m_dIn=  track->GetVertexPosition().mag();

        m_thEnd=postStep->GetPosition().theta();
        m_phEnd=postStep->GetPosition().phi();
        m_dEnd=postStep->GetPosition().mag();

        m_particles->Fill();
        m_X0 = 0.;
        m_L0 = 0.;
        m_wZ = 0.;
        m_radloss = 0.;
        m_ionloss = 0.;
        m_wzOaTr= 0.;
      }
      m_X0=0.;
    }
  }
}

void iGeant4::PhysicsValidationUserAction::PreTracking(const G4Track*)
{
  m_sHelper.ResetNrOfSecondaries();
}


StatusCode iGeant4::PhysicsValidationUserAction::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}


namespace G4UA{

  namespace iGeant4 {

    PhysicsValidationUserAction::PhysicsValidationUserAction(const Config& config):
      m_config(config)
    {
      if(4<m_config.verboseLevel)
        {
          G4cout << "create PhysicsValidationUserAction" << G4endl;
        }
    }

    void PhysicsValidationUserAction::beginOfEvent(const G4Event*)
    {
      G4ExceptionDescription description;
      description << G4String("beginOfEvent: ") + " - this action has not been implemented yet.";
      G4Exception("G4UA::iGeant4::PhysicsValidationUserAction", "ActionNotImplemented", JustWarning, description);
      return;
    }

    void PhysicsValidationUserAction::endOfEvent(const G4Event*)
    {
      G4ExceptionDescription description;
      description << G4String("endOfEvent: ") + " - this action has not been implemented yet.";
      G4Exception("G4UA::iGeant4::PhysicsValidationUserAction", "ActionNotImplemented", JustWarning, description);
      return;
    }

    void PhysicsValidationUserAction::beginOfRun(const G4Run*)
    {
      G4ExceptionDescription description;
      description << G4String("beginOfRun: ") + " - this action has not been implemented yet.";
      G4Exception("G4UA::iGeant4::PhysicsValidationUserAction", "ActionNotImplemented", JustWarning, description);
      return;
    }

    void PhysicsValidationUserAction::processStep(const G4Step*)
    {
      G4ExceptionDescription description;
      description << G4String("processStep: ") + " - this action has not been implemented yet.";
      G4Exception("G4UA::iGeant4::PhysicsValidationUserAction", "ActionNotImplemented", JustWarning, description);
      return;
    }

    void PhysicsValidationUserAction::preTracking(const G4Track*)
    {
      G4ExceptionDescription description;
      description << G4String("preTracking: ") + " - this action has not been implemented yet.";
      G4Exception("G4UA::iGeant4::PhysicsValidationUserAction", "ActionNotImplemented", JustWarning, description);
      return;
    }

  } // namespace iGeant4

} // namespace G4UA
