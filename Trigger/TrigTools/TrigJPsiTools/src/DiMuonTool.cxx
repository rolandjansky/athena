/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***********************************************************************
// DiMuonTool.cxx
// Creates an object holding a pair of tracks

// AUTHORS: Weina Ji (weina.ji@cern.ch)
//          Peter Wijeratne (paw@hep.ucl.ac.uk)
// ***********************************************************************

#include "TLorentzVector.h"
#include "TrigJPsiTools/DiMuonTool.h"

#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

DiMuonTool::DiMuonTool()
{

  m_doTagAndProbe=false;
  m_trackMass = 105.66*CLHEP::MeV;
  m_tracks.clear();

}

DiMuonTool::~DiMuonTool() { }

const Wrapper::MuonTrack* DiMuonTool::TaggedTrack() { 
  /*std::cout << "DiMuonTool::TaggedTrack -- m_tracks size: "    << m_tracks.size() << std::endl;
  std::cout << "DiMuonTool::TaggedTrack -- m_tracks[0]: "      << m_tracks[0] << std::endl;
  std::cout << "DiMuonTool::TaggedTrack -- m_tracks[1]: "      << m_tracks[1] << std::endl;
  std::cout << "DiMuonTool::TaggedTrack -- return m_tracks[0]" << std::endl;*/
  return m_tracks[0];
}

const Wrapper::MuonTrack* DiMuonTool::ProbedTrack() {
  /*std::cout << "DiMuonTool::ProbedTrack -- m_tracks size: "    << m_tracks.size() << std::endl;
  std::cout << "DiMuonTool::ProbedTrack -- m_tracks[0]: "      << m_tracks[0] << std::endl;
  std::cout << "DiMuonTool::ProbedTrack -- m_tracks[1]: "      << m_tracks[1] << std::endl;
  std::cout << "DiMuonTool::ProbedTrack -- return m_tracks[1]" << std::endl;*/
  return m_tracks[1];
}

bool DiMuonTool::doTagAndProbe() {return m_doTagAndProbe;}
double DiMuonTool::InvariantMass() {
  double invmass;
  TLorentzVector particle1,particle2,v;
  
  if (m_tracks.size()==2) {
    const Wrapper::MuonTrack* track1=m_tracks[0];
    const Wrapper::MuonTrack* track2=m_tracks[1];
    double Mass = m_trackMass;
    particle1.SetPtEtaPhiE(track1->pt(),track1->eta(),track1->phi(),sqrt(pow(Mass,2)+pow(track1->px(),2)+pow(track1->py(),2)+pow(track1->pz(),2)));
    particle2.SetPtEtaPhiE(track2->pt(),track2->eta(),track2->phi(),sqrt(pow(Mass,2)+pow(track2->px(),2)+pow(track2->py(),2)+pow(track2->pz(),2)));
    v=particle1+particle2;
    invmass = v.Mag();
  }
  else invmass=0;

  return invmass; //CLHEP::MeV
}

double DiMuonTool::pt() {
  double transmom;

  if (m_tracks.size()==2) {
    const Wrapper::MuonTrack* track1=m_tracks[0];
    const Wrapper::MuonTrack* track2=m_tracks[1];
    double px = track1->px()+track2->px();
    double py = track1->py()+track2->py();
    transmom=sqrt(px*px+py*py);
  }
  else transmom=0;
 
  return transmom;  //CLHEP::MeV
}

double DiMuonTool::eta() {
  double eta;

  if (m_tracks.size()==2) {
    const Wrapper::MuonTrack* track1=m_tracks[0];
    const Wrapper::MuonTrack* track2=m_tracks[1];

    double px=track1->px()+track2->px();
    double py = track1->py()+track2->py();
    double pz = track1->pz()+track2->pz();
    double p=sqrt(px*px+py*py+pz*pz);
    eta=0.5*log((p+pz)/(p-pz));
  }
  else eta=0;
    
  return eta;
}

double DiMuonTool::phi() {  
  double phi;

  if (m_tracks.size()==2) {
    const Wrapper::MuonTrack* track1=m_tracks[0];
    const Wrapper::MuonTrack* track2=m_tracks[1];

    double px = track1->px()+track2->px();
    double py = track1->py()+track2->py();
    double p=sqrt(px*px+py*py);
    phi=acos(px/p);
    if (py<0.) phi=-1.*phi; 
  }
  else phi=0;

  return phi;
}

double DiMuonTool::CurvatureDiff() {
  double asym;

  if (m_tracks.size()==2) {
    const Wrapper::MuonTrack* track1=m_tracks[0]->charge()>0?m_tracks[0]:m_tracks[1];
    const Wrapper::MuonTrack* track2=m_tracks[1]->charge()>0?m_tracks[1]:m_tracks[0];

    double qoverpt1=track1->charge()/track1->pt();
    double qoverpt2=track2->charge()/track2->pt();
    asym=fabs(qoverpt1)-fabs(qoverpt2);
  }
  else asym=0;

  return asym; //[CLHEP::MeV]^-1
}

void DiMuonTool::clear()
{
  m_doTagAndProbe=false;
  m_trackMass = 105.66*CLHEP::MeV;
  for(unsigned i=0; i--; ) delete m_tracks[i];
  m_tracks.clear();

}
