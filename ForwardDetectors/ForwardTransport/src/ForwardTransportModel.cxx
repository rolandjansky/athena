/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "GaudiKernel/ISvcLocator.h"
#include "G4FastSimulation/FastSimModelProxy.h"

#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Lambda.hh"

#include "ForwardTransport/ForwardTransportModel.h"
#include "ForwardTracker/Point.h"

static FastSimModelProxy<ForwardTransportModel> plugin("ForwardTransport");

ForwardTransportModel::ForwardTransportModel(std::string name):	
  FastSimModel(name)
{   
  if (Gaudi::svcLocator()->service("ForwardTransportSvc", m_fwdSvc).isFailure()) 
    ATH_MSG_INFO("ForwardTransportModel is not inizialized!");
  
  m_fwdTrack.initialize(m_fwdSvc->getConfigData());

  ATH_MSG_INFO(" transportFlag " << m_fwdSvc->getTransportFlag());
  ATH_MSG_INFO(" etaCut        " << m_fwdSvc->getEtaCut());
  ATH_MSG_INFO(" xiCut         " << m_fwdSvc->getXiCut());
  ATH_MSG_INFO(" fillRootTree  " << m_fwdSvc->getFillRootTree());
  ATH_MSG_INFO(" rootFilePath  " << m_fwdSvc->getRootFilePath());
  ATH_MSG_INFO(" MCkey         " << m_fwdSvc->getMCkey());
}

void ForwardTransportModel::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep) {

  // Depending on particle type and kinematics one can decide to kill the track, 
  // modify it or change it into something else (e.g. a parameterised shower).
  
  ATH_MSG_INFO("ForwardTransportModel::DoIt");

  HepMC::GenEvent* gEvent = m_fwdSvc->getEvent(); if (!gEvent) { ATH_MSG_WARNING("Cannot get HepMC::GenEvent pointer"); return; };

  int           pdgcode  = fastTrack.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding();
  double        charge   = fastTrack.GetPrimaryTrack()->GetDefinition()->GetPDGCharge();
  G4ThreeVector position = fastTrack.GetPrimaryTrack()->GetPosition();
  double        time     = fastTrack.GetPrimaryTrack()->GetGlobalTime();
  G4ThreeVector momentum = fastTrack.GetPrimaryTrack()->GetMomentum();  
  double        energy   = fastTrack.GetPrimaryTrack()->GetTotalEnergy();
  
  if (!m_fwdSvc->selectedParticle(momentum, pdgcode)) { KillPrimaryTrack(fastStep, energy); return; }

  m_fwdSvc->fillMcInfo(position, time, momentum, energy);

  ATH_MSG_INFO(" pdgCode: " << pdgcode << " energy[GeV]: " << energy/CLHEP::GeV << " charge: " << charge);

  ForwardTracker::Particle fParticle = ForwardTracker::Particle(position.x()/CLHEP::m,
								position.y()/CLHEP::m, 
								position.z()/CLHEP::m,
								momentum.x()/CLHEP::GeV,
								momentum.y()/CLHEP::GeV, 
								momentum.z()/CLHEP::GeV,
								fabs(charge)>0);
  ATH_MSG_INFO(fParticle);
  
  bool isTransported = m_fwdTrack.TrackParticle(fParticle);  

  ATH_MSG_INFO(m_fwdTrack.fPar());
  
  ForwardTracker::Point fPos = m_fwdTrack.fPos();
  ForwardTracker::Point fMom = m_fwdTrack.fMom();
  
  position.set(fPos.x()*CLHEP::m,   fPos.y()*CLHEP::m,   fPos.z()*CLHEP::m);
  momentum.set(fMom.x()*CLHEP::GeV, fMom.y()*CLHEP::GeV, fMom.z()*CLHEP::GeV);
  
  m_fwdSvc->addMcVertex(position, time, momentum, energy, pdgcode, isTransported, gEvent);

  if (!isTransported) { KillPrimaryTrack(fastStep, energy); return; }

  fastStep.ProposePrimaryTrackFinalPosition(position, false);
  fastStep.SetPrimaryTrackFinalMomentum    (momentum, false);
}

void ForwardTransportModel::KillPrimaryTrack(G4FastStep& fastStep, double energy) {

  fastStep.KillPrimaryTrack();
  fastStep.ProposePrimaryTrackPathLength(0.0);
  fastStep.ProposeTotalEnergyDeposited(energy);
}
