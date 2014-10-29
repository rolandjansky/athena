/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "G4DetectorEnvelopes/EnvelopeGeometryManager.h"

#include "MCTruth/VTrackInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"

#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ITruthBinding.h"
#include "ISF_Event/ParticleClipboard.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

#include "ISF_HepMC_Event/HepMC_TruthBinding.h"

#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/IParticleHelper.h"

#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
//#include "G4VPhysicalVolume.hh"
//#include "SimHelpers/SecondaryTracksHelper.h"

#include "ISF_Geant4Tools/PhysicsValidationUserAction.h"
#include "ISF_Geant4Tools/Geant4TruthIncident.h"

#include "MCTruth/TrackBarcodeInfo.h"

#include "HepMC/GenVertex.h"
// ROOT includes
#include "TTree.h"

#include <iostream>

iGeant4::PhysicsValidationUserAction::PhysicsValidationUserAction(const std::string& type,
                                                                  const std::string& name,
                                                                  const IInterface* parent)
  : IPhysicsValidationUserAction(name),
    AthAlgTool(type,name,parent),
    m_sHelper(FADS::FadsTrackingAction::GetTrackingAction()->GetTrackingManager()),
    m_particleBroker("ISF::ParticleBroker/ISF_ParticleStackService",name),
    m_particleHelper("ISF::ParticleHelper/ISF_ParticleHelper"),
    m_geoIDSvc("ISF::GeoIDSvc/ISF_GeoIDSvc",name),
    m_geoIDSvcQuick(0),
    m_validationOutput(true),
    m_thistSvc("THistSvc",name),
    //m_validationStream("ISFMaterial"),
    m_validationStream("ISFG4SimKernel"),
    m_idR(0.), m_idZ(0.),
    m_caloRmean(0.),  m_caloZmean(0.),
    m_muonRmean(0.), m_muonZmean(0.),
    m_cavernRmean(0.),  m_cavernZmean(0.),
    m_volumeOffset(1),
    m_minHistoryDepth(0)
{

  ATH_MSG_DEBUG("create PhysicsValidationUserAction name: "<<name);

  declareInterface<IPhysicsValidationUserAction>(this);

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

  m_geoIDSvcQuick = &(*m_geoIDSvc);

  // setup for validation mode
  if ( m_validationOutput) {

    // retrieve the histogram service
    if ( m_thistSvc.retrieve().isSuccess() ) {
      // Create the prefix of histogram names for the THistSvc
      const char *treeName="particles";
      const std::string prefix = "/" + m_validationStream + "/"+ treeName;
      m_particles = new TTree( treeName, treeName );
      m_particles->Branch("pdg"       , &m_pdg    , "pdg/I"         );
      m_particles->Branch("scIn"      , &m_scIn   , "scIn/I"         );
      m_particles->Branch("scOut"     , &m_scEnd  , "scOut/I"         );
      m_particles->Branch("gen"       , &m_gen    , "gen/I"         );
      m_particles->Branch("geoID"     , &m_geoID  , "geoID/I"         );
      m_particles->Branch("pth"       , &m_pth     , "pth/F"            );
      m_particles->Branch("pph"       , &m_pph     , "pph/F"            );
      m_particles->Branch("p"         , &m_p      , "p/F"            );
      m_particles->Branch("eloss"     , &m_eloss  , "eloss/F"            );
      m_particles->Branch("X0"        , &m_X0     , "X0/F"            );
      m_particles->Branch("L0"        , &m_L0     , "L0/F"            );
      m_particles->Branch("wZ"        , &m_wZ     , "wZ/F"            );
      m_particles->Branch("dt"        , &m_dt     , "dt/F"            );
      m_particles->Branch("thIn"      , &m_thIn   , "thIn/F"            );
      m_particles->Branch("phIn"      , &m_phIn   , "phIn/F"            );
      m_particles->Branch("dIn"       , &m_dIn    , "dIn/F"            );
      m_particles->Branch("thEnd"     , &m_thEnd  , "thEnd/F"            );
      m_particles->Branch("phEnd"     , &m_phEnd  , "phEnd/F"            );
      m_particles->Branch("dEnd"      , &m_dEnd   , "dEnd/F"            );

      // register the Tree to the THistSvc and return it's StatusCode
      ATH_CHECK(m_thistSvc->regTree( prefix, m_particles) );

      m_X0=0.;
      m_L0=0.;
      m_wZ=0.;

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
      m_interactions->Branch("p_mother"    , &m_p_mother    , "p_mother/F"         );
      m_interactions->Branch("pdg_child"   , m_pdg_child    , "pdg_child[nch]/I"         );
      m_interactions->Branch("fp_child"    , m_fp_child     , "fp_child[nch]/F"         );
      m_interactions->Branch("oa_child"    , m_oa_child     , "oa_child[nch]/F"         );
      // register the Tree to the THistSvc and return it's StatusCode
      ATH_CHECK(m_thistSvc->regTree( prefixInt, m_interactions) );

    }
  }

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::PhysicsValidationUserAction::finalize()
{
  return StatusCode::SUCCESS;
}


void iGeant4::PhysicsValidationUserAction::BeginOfRunAction(const G4Run*)
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

void iGeant4::PhysicsValidationUserAction::EndOfRunAction(const G4Run*)
{

}

void iGeant4::PhysicsValidationUserAction::BeginOfEventAction(const G4Event*)
{

  EnvelopeGeometryManager* gm=EnvelopeGeometryManager::GetGeometryManager();

  const double eps=1.e-5;

  m_idR=gm->IdetOuterRadius()-eps;
  m_idZ=gm->IdetMaxZ();

  m_caloRmean=0.5*(gm->CaloInnerRadius()+gm->CaloOuterRadius());
  m_caloZmean=0.5*(gm->IdetMaxZ()+gm->CaloMaxZ());

  m_muonRmean=0.5*(gm->MuonInnerRadius()+gm->MuonOuterRadius());
  m_muonZmean=0.5*(gm->MuonMaxZ()+gm->CaloMaxZ());

  m_cavernRmean=300000.0;//FIXME - requires updates to EnvelopeGeometryManager
  m_cavernZmean=300000.0;//FIXME - requires updates to EnvelopeGeometryManager

  ATH_MSG_VERBOSE("m_idRmax: "<<m_idR      <<", m_idZmax: "<<m_idZ);
  ATH_MSG_VERBOSE("m_caloR : "<<m_caloRmean<<", m_caloZ : "<<m_caloZmean);
  ATH_MSG_VERBOSE("m_muonR : "<<m_muonRmean<<", m_muonZ : "<<m_muonZmean);
  ATH_MSG_VERBOSE("m_muonR : "<<m_cavernRmean<<", m_muonZ : "<<m_cavernZmean);
  ATH_MSG_VERBOSE("m_cavernR : "<<m_cavernRmean<<", m_cavernZ : "<<m_cavernZmean);

  //m_idToStackParticleMap.clear();

}

void iGeant4::PhysicsValidationUserAction::EndOfEventAction(const G4Event*)
{
  m_X0=0.;
  m_L0=0.;
  m_wZ=0.;

}


void iGeant4::PhysicsValidationUserAction::SteppingAction(const G4Step* aStep)
{
  //std::cout<<"PhysicsValidationUserAction::SteppingAction"<<std::endl;

  // process identification
  const G4VProcess * process = aStep->GetPostStepPoint()->GetProcessDefinedStep();

  // info about generating particle
  G4Track * track = aStep->GetTrack();

  // material info
  G4StepPoint *preStep=aStep->GetPreStepPoint();
  G4StepPoint *postStep=aStep->GetPostStepPoint();

  // parent
  //const ISF::ISFParticle* parent= ISF::ParticleClipboard::getInstance().getParticle();
  // something is seriously wrong if there is no parent particle
  //assert(parent);

  G4ThreeVector mom = preStep->GetMomentum();
  G4ThreeVector pos = preStep->GetPosition();

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
    if (l0>0.) {
      m_L0 += stepLength/l0;
      const G4ElementVector* eVec = preStep->GetMaterial()->GetElementVector();
      const G4double* fVec = preStep->GetMaterial()->GetFractionVector();
      for (unsigned int i=0; i<eVec->size(); i++) {
        m_wZ += stepInX0*fVec[i]*((*eVec)[i]->GetZ());
        //std::cout <<"elements:"<<i<<","<<fVec[i]<<":"<<(*eVec)[i]->GetZ()<< ","<<m_wZ<<","<<m_wZ/m_X0<<std::endl;
        //m_wA += stepInX0*fVec[i]*((*eVec)[i]->GetA());
      }
    }
  }

  VTrackInformation * trackInfo= static_cast<VTrackInformation*>(track->GetUserInformation());
  HepMC::GenParticle* genpart= trackInfo ? const_cast<HepMC::GenParticle*>(trackInfo->GetHepMCParticle()):0;
  HepMC::GenVertex* vtx = genpart ? genpart->production_vertex() : 0;
  m_gen = genpart? 0 : -1;
  while (genpart && vtx ) {
    int pdgID=genpart->pdg_id();
    genpart = vtx->particles_in_size()>0 ? *(vtx->particles_in_const_begin()) : 0;
    if ( genpart && genpart->pdg_id() != pdgID ) m_gen++;
    vtx = genpart ? genpart->production_vertex() : 0;
  }

  // save interaction info (if any)
  if ( process && process->GetProcessSubType()>0 ) {
    ISF::Geant4TruthIncident truth( aStep, geoID, m_sHelper.NrOfNewSecondaries(), m_sHelper);
    unsigned int nSec = truth.numberOfSecondaries();
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
        G4ThreeVector secMom = truth.secondaryP(isec);
        if (isec<nSecMax) {
          m_pdg_child[isec+iPrimSurv] = truth.secondaryPdgCode(isec);
          m_fp_child[isec+iPrimSurv] = secMom.mag()/m_p_mother;
          m_oa_child[isec+iPrimSurv] = secMom*mom/m_p_mother/secMom.mag();
        }
        pbal -= secMom;
      }

      m_vtx_p_diff = pbal.mag();
      m_vtx_plong_diff = pbal*mom/m_p_mother;
      m_vtx_pperp_diff = sqrt(m_vtx_p_diff*m_vtx_p_diff-m_vtx_plong_diff*m_vtx_plong_diff);

      m_interactions->Fill();
    }
  }

  // crossing subdetector boundary ?
  G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();
  G4VPhysicalVolume *postVol=postStep->GetPhysicalVolume();

  if (postVol==0) {
    // left world -save info
    m_pdg = track->GetDefinition()->GetPDGEncoding();
    const G4VProcess* creation = track->GetCreatorProcess();
    m_scIn = creation? creation->GetProcessSubType() : -1;
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

    return;
  }

  // if particle killed, save the info
  if ( track->GetTrackStatus()!=fAlive ) {
    m_pdg = track->GetDefinition()->GetPDGEncoding();
    const G4VProcess* creation = track->GetCreatorProcess();
    m_scIn = creation? creation->GetProcessSubType() : -1;
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

    G4ParticleDefinition* particleDefinition = track->GetDefinition();

    const G4ThreeVector g4pos = track->GetPosition();
    //const double gTime = track->GetGlobalTime();
    const HepGeom::Point3D<double> position(g4pos.x(),g4pos.y(),g4pos.z());

    G4ThreeVector g4mom = track->GetMomentum();
    const HepGeom::Vector3D<double> momentum(g4mom.x(),g4mom.y(),g4mom.z());

    //double mass    = particleDefinition->GetPDGMass();
    //double charge  = particleDefinition->GetPDGCharge();
    int    pdgID   = particleDefinition->GetPDGEncoding();

    if (m_particleBroker) {

      // *AS* why ask stackSvc for current(), shouldn't better the TransportTool keep track?
      bool dead=false;
      if (track->GetTrackStatus()==fStopAndKill) {
        dead=true;
      }

      if (!dead) {

        // track info
        //VTrackInformation * trackInfo= static_cast<VTrackInformation*>(track->GetUserInformation());
        m_pdg = pdgID;
        const G4VProcess* creation = track->GetCreatorProcess();
        m_scIn = creation? creation->GetProcessSubType() : -1;
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

      }
      m_X0=0.;
    }
  }
}

void iGeant4::PhysicsValidationUserAction::PostUserTrackingAction(const G4Track* /*aTrack*/)
{
}

void iGeant4::PhysicsValidationUserAction::PreUserTrackingAction(const G4Track*)
{
  m_sHelper.ResetNrOfSecondaries();
}
