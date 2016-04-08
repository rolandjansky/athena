/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/ScoringPlane.h"
#include "G4Step.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TTree.h"
#include "TFile.h"


ScoringPlane::ScoringPlane(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),
    m_tree0(0),m_tree1(0),m_evt(0),m_ntr(0),
    m_pdg(0),m_cha(0),m_ene(0),m_vx(0),m_vy(0),m_vz(0),m_x0(0),m_y0(0),m_z0(0),m_t0(0),
    m_px0(0),m_py0(0),m_pz0(0),m_x1(0),m_y1(0),m_z1(0),m_t1(0),m_px1(0),m_py1(0),
    m_pz1(0),m_x(0),m_y(0),m_z(0),m_plane(22600),m_pkill(0),m_fname("ufo.root") {

  declareProperty("Plane",m_plane);
  declareProperty("PKill",m_pkill);
  declareProperty("FName",m_fname);


}

void ScoringPlane::BeginOfEvent(const G4Event* /*anEvent*/) {
  m_evt++;
  m_ntr=0;
}

void ScoringPlane::EndOfEvent(const G4Event* /*anEvent*/) {
  m_tree0->Fill();
}

StatusCode ScoringPlane::initialize() {

  m_tree0 = new TTree("t0", "ATHENA event tree");

  m_tree0->Branch("evt", &m_evt, "evt/I");
  m_tree0->Branch("ntr", &m_ntr, "ntr/I");

  m_tree1 = new TTree("t1", "ATHENA particle tree");

  m_tree1->Branch("evt", &m_evt, "evt/I");
  m_tree1->Branch("ntr", &m_ntr, "ntr/I");
  m_tree1->Branch("pdg", &m_pdg, "pdg/I");
  m_tree1->Branch("cha", &m_cha, "cha/D");
  m_tree1->Branch("ene", &m_ene, "ene/D");
  m_tree1->Branch("x0",  &m_x0,  "x0/D");
  m_tree1->Branch("y0",  &m_y0,  "y0/D");
  m_tree1->Branch("z0",  &m_z0,  "z0/D");
  m_tree1->Branch("t0",  &m_t0,  "t0/D");
  m_tree1->Branch("px0", &m_px0, "px0/D");
  m_tree1->Branch("py0", &m_py0, "py0/D");
  m_tree1->Branch("pz0", &m_pz0, "pz0/D");
  m_tree1->Branch("x1",  &m_x1,  "x1/D");
  m_tree1->Branch("y1",  &m_y1,  "y1/D");
  m_tree1->Branch("z1",  &m_z1,  "z1/D");
  m_tree1->Branch("t1",  &m_t1,  "t1/D");
  m_tree1->Branch("px1", &m_px1, "px1/D");
  m_tree1->Branch("py1", &m_py1, "py1/D");
  m_tree1->Branch("pz1", &m_pz1, "pz1/D");
  m_tree1->Branch("vx",  &m_vx,  "vx/D");
  m_tree1->Branch("vy",  &m_vy,  "vy/D");
  m_tree1->Branch("vz",  &m_vz,  "vz/D");
  m_tree1->Branch("x",   &m_x,   "x/D");
  m_tree1->Branch("y",   &m_y,   "y/D");
  m_tree1->Branch("z",   &m_z,   "z/D");

  m_evt = 0;

  ATH_MSG_INFO( "ScoringPlane: placing scoring plane at [mm]: " << m_plane );
  ATH_MSG_INFO( "ScoringPlane: stop and kill particles: "       << m_pkill );
  ATH_MSG_INFO( "ScoringPlane: output root filename: "          << m_fname );

  return StatusCode::SUCCESS;
  
}

StatusCode ScoringPlane::finalize() {

  TFile* file = new TFile(m_fname.c_str(), "RECREATE", "ATHENA ufo simulation");

  m_tree0->Write();
  m_tree1->Write();

  file->Close();

  return StatusCode::SUCCESS;
}

void ScoringPlane::Step(const G4Step* aStep) {

  m_z0 = aStep->GetPreStepPoint()->GetPosition().z();
  m_z1 = aStep->GetPostStepPoint()->GetPosition().z();

  if (m_z0*m_plane < 0)                     return; // take only particles on one side
  if (m_z1*m_plane < 0)                     return; // take only particles on one side
  if (std::fabs(m_z0) < std::fabs(m_plane)) return; // take only particles flowing towards the IP
  if (std::fabs(m_z1) > std::fabs(m_plane)) return; // take only particles flowing towards the IP

  m_ntr++;

  m_pdg = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  m_cha = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
  m_ene = aStep->GetTrack()->GetTotalEnergy();
  m_vx  = aStep->GetTrack()->GetVertexPosition().x();
  m_vy  = aStep->GetTrack()->GetVertexPosition().y();
  m_vz  = aStep->GetTrack()->GetVertexPosition().z();

  m_x0  = aStep->GetPreStepPoint()->GetPosition().x();
  m_y0  = aStep->GetPreStepPoint()->GetPosition().y();
  m_t0  = aStep->GetPreStepPoint()->GetGlobalTime()/CLHEP::ns;
  m_px0 = aStep->GetPreStepPoint()->GetMomentum().x();
  m_py0 = aStep->GetPreStepPoint()->GetMomentum().y();
  m_pz0 = aStep->GetPreStepPoint()->GetMomentum().z();

  m_x1  = aStep->GetPostStepPoint()->GetPosition().x();
  m_y1  = aStep->GetPostStepPoint()->GetPosition().y();
  m_t1  = aStep->GetPostStepPoint()->GetGlobalTime()/CLHEP::ns;
  m_px1 = aStep->GetPostStepPoint()->GetMomentum().x();
  m_py1 = aStep->GetPostStepPoint()->GetMomentum().y();
  m_pz1 = aStep->GetPostStepPoint()->GetMomentum().z();

  m_z = m_plane;
  m_x = m_x0 + (m_z0!=m_z1 ? (m_x1-m_x0)/(m_z1-m_z0)*(m_z-m_z0) : (m_x1-m_x0)*0.5);
  m_y = m_y0 + (m_z0!=m_z1 ? (m_y1-m_y0)/(m_z1-m_z0)*(m_z-m_z0) : (m_y1-m_y0)*0.5);

  m_tree1->Fill();

  if      (m_pkill == 1) aStep->GetTrack()->SetTrackStatus(fStopAndKill);
  else if (m_pkill == 2) aStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);

  ATH_MSG_DEBUG(
   	   " z0: " << std::setw(10) << m_z0
   	<< " z1: " << std::setw(10) << m_z1
   	<< " x0: " << std::setw(10) << m_x0
   	<< " x1: " << std::setw(10) << m_x1
   	<< " x:  " << std::setw(10) << m_x
   	<< " y0: " << std::setw(10) << m_y0
   	<< " y1: " << std::setw(10) << m_y1
   	<< " y:  " << std::setw(10) << m_y );
}




StatusCode ScoringPlane::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
