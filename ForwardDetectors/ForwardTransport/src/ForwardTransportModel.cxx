/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTransportModel.h"

// Athena includes
#include "ForwardTracker/Point.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Lambda.hh"

#include "AtlasHepMC/GenEvent.h"

ForwardTransportModel::ForwardTransportModel(const std::string& name, const int verboseLevel, const std::string& FwdTrSvcName)
  : G4VFastSimulationModel(name)
  , m_fwdSvc(nullptr)
  , m_verboseLevel(verboseLevel)
  , m_FwdTrSvcName(FwdTrSvcName)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  if (svcLocator->service(FwdTrSvcName,m_fwdSvc).isFailure())
    {
      G4ExceptionDescription description;
      description << "ForwardTransportModel: Attempt to access ForwardTransportSvc failed.";
      G4Exception("ForwardTransportModel", "CouldNotAccessFwdSvc", FatalException, description);
      abort(); // to keep Coverity happy
    }

  m_fwdTrack.initialize(m_fwdSvc->getConfigData());

  if(m_verboseLevel>5)
    {
      G4cout << " transportFlag " << m_fwdSvc->getTransportFlag() << G4endl;
      G4cout << " etaCut        " << m_fwdSvc->getEtaCut() << G4endl;
      G4cout << " xiCut         " << m_fwdSvc->getXiCut() << G4endl;
      G4cout << " fillRootTree  " << m_fwdSvc->getFillRootTree() << G4endl;
      G4cout << " rootFilePath  " << m_fwdSvc->getRootFilePath() << G4endl;
      G4cout << " MCkey         " << m_fwdSvc->getMCkey() << G4endl;
    }
  return;
}

void ForwardTransportModel::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep) {

  // Depending on particle type and kinematics one can decide to kill the track,
  // modify it or change it into something else (e.g. a parameterised shower).

  if(m_verboseLevel>4)
    {
      G4cout <<"ForwardTransportModel::DoIt" << G4endl;
    }
  // FIXME: This algorithm is explicitly modifying an object
  //        resident in the event store.  This will not work
  //        with AthenaMT!
  HepMC::GenEvent* gEvent = const_cast<HepMC::GenEvent*>(m_fwdSvc->getEvent());
  if (!gEvent) { G4cout << "ForwardTransportModel::DoIt WARNING Cannot get HepMC::GenEvent pointer" << G4endl; return; };

  int           pdgcode  = fastTrack.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding();
  double        charge   = fastTrack.GetPrimaryTrack()->GetDefinition()->GetPDGCharge();
  G4ThreeVector position = fastTrack.GetPrimaryTrack()->GetPosition();
  double        time     = fastTrack.GetPrimaryTrack()->GetGlobalTime();
  G4ThreeVector momentum = fastTrack.GetPrimaryTrack()->GetMomentum();
  double        energy   = fastTrack.GetPrimaryTrack()->GetTotalEnergy();

  if (!m_fwdSvc->selectedParticle(momentum, pdgcode)) { KillPrimaryTrack(fastStep, energy); return; }

  m_fwdSvc->fillMcInfo(position, time, momentum, energy);

    if(m_verboseLevel>5)
    {
      G4cout <<" pdgCode: " << pdgcode << " energy[GeV]: " << energy/CLHEP::GeV << " charge: " << charge << G4endl;
    }
  ForwardTracker::Particle fParticle = ForwardTracker::Particle(position.x()/CLHEP::m,
                                                                position.y()/CLHEP::m,
                                                                position.z()/CLHEP::m,
                                                                momentum.x()/CLHEP::GeV,
                                                                momentum.y()/CLHEP::GeV,
                                                                momentum.z()/CLHEP::GeV,
                                                                fabs(charge)>0);
  if(m_verboseLevel>5)
    {
      G4cout << fParticle << G4endl;
    }
  bool isTransported = m_fwdTrack.TrackParticle(fParticle);

  if(m_verboseLevel>5)
    {
      G4cout << m_fwdTrack.fPar() << G4endl;
    }

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
