/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AtlasHepMC/GenEvent.h"

#include "ForwardTransportFast/ForwardTransportFast.h"
#include "ForwardTracker/Particle.h"
#include "ForwardTracker/Point.h"

ForwardTransportFast::ForwardTransportFast(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_partPropSvc("PartPropSvc","PartPropSvc"),
  m_fwdSvc("ForwardTransportSvc", name)
{
  declareProperty("ForwardTransportSvc", m_fwdSvc);
}

StatusCode ForwardTransportFast::initialize() {

  if (m_partPropSvc.retrieve().isFailure()) ATH_MSG_ERROR(" Could not initialize Particle Properties Service");
  
  m_pdt = m_partPropSvc->PDT();

  if (m_fwdSvc.retrieve().isFailure()) ATH_MSG_ERROR("ForwardTransportSvc was not correct initialized");
  
  m_fwdTrack.initialize(m_fwdSvc->getConfigData());

  ATH_MSG_INFO(" transportFlag " << m_fwdSvc->getTransportFlag());
  ATH_MSG_INFO(" etaCut        " << m_fwdSvc->getEtaCut());
  ATH_MSG_INFO(" xiCut         " << m_fwdSvc->getXiCut());
  ATH_MSG_INFO(" fillRootTree  " << m_fwdSvc->getFillRootTree());
  ATH_MSG_INFO(" rootFilePath  " << m_fwdSvc->getRootFilePath());
  ATH_MSG_INFO(" MCkey         " << m_fwdSvc->getMCkey());
 
  return StatusCode::SUCCESS;
}

StatusCode ForwardTransportFast::execute() {

  const McEventCollection* mcCollptr;
  
  if (evtStore()->retrieve(mcCollptr, m_fwdSvc->getMCkey()).isFailure()) {

    ATH_MSG_WARNING(" Could not retrieve McEventCollection: " << m_fwdSvc->getMCkey());

    return StatusCode::SUCCESS;
  } 
  
  for (McEventCollection::const_iterator itr = mcCollptr->begin(); itr != mcCollptr->end(); ++itr) {

    // FIXME: This algorithm is explicitly modifying an object
    //        resident in the event store.  This will not work
    //        with AthenaMT!
    HepMC::GenEvent* gEvent = const_cast<HepMC::GenEvent*>(*itr); 

    std::vector<int>               fPidVector;
    std::vector<HepMC::FourVector> fPosVector;
    std::vector<HepMC::FourVector> fMomVector;
    
    for (HepMC::GenEvent::particle_const_iterator p = gEvent->particles_begin(); p != gEvent->particles_end(); ++p) {
  
      HepMC::GenParticle* gParticle = (*p); 

      if (gParticle->status() != 1) continue; // take only stable particles
      if (gParticle->end_vertex())  continue; // skip decay vertices
      
      gParticle->set_status(201);

      HepMC::FourVector Position = gParticle->production_vertex()->position();
      HepMC::FourVector Momentum = gParticle->momentum();
      
      int pdgcode = gParticle->pdg_id();

      G4ThreeVector position(Position.x(), Position.y(), Position.z());
      double        time    (Position.t());
      G4ThreeVector momentum(Momentum.px(), Momentum.py(), Momentum.pz()); 
      double        energy  (Momentum.e());
      
      if (!m_fwdSvc->selectedParticle(momentum, pdgcode)) continue;
      
      m_fwdSvc->fillMcInfo(position, time, momentum, energy);

      HepPDT::ParticleData* pData = m_pdt->particle(pdgcode); if (!pData) { ATH_MSG_WARNING("Cannot get HepPDT::ParticleData pointer"); continue; }
      
      ATH_MSG_INFO(" pdgCode: " << pdgcode << " energy[GeV]: " << energy/CLHEP::GeV << " charge: " << pData->charge());
      
      ForwardTracker::Particle fParticle = ForwardTracker::Particle(position.x()/CLHEP::m,
								    position.y()/CLHEP::m, 
								    position.z()/CLHEP::m,
								    momentum.x()/CLHEP::GeV,
								    momentum.y()/CLHEP::GeV, 
								    momentum.z()/CLHEP::GeV,
 								    fabs(pData->charge())>0);
      
      ATH_MSG_INFO(fParticle);

      bool isTransported = m_fwdTrack.TrackParticle(fParticle);

      ATH_MSG_INFO(m_fwdTrack.fPar());

      ForwardTracker::Point fPos = m_fwdTrack.fPos();
      ForwardTracker::Point fMom = m_fwdTrack.fMom();
      
      position.set(fPos.x()*CLHEP::m,   fPos.y()*CLHEP::m,   fPos.z()*CLHEP::m);
      momentum.set(fMom.x()*CLHEP::GeV, fMom.y()*CLHEP::GeV, fMom.z()*CLHEP::GeV);

      m_fwdSvc->addMcVertex(position, time, momentum, energy, pdgcode, isTransported, gEvent);

      if (!isTransported) continue;

      fPidVector.push_back(pdgcode);
      fPosVector.push_back(HepMC::FourVector(fPos.x()*CLHEP::m,   fPos.y()*CLHEP::m,   fPos.z()*CLHEP::m,   time));
      fMomVector.push_back(HepMC::FourVector(fMom.x()*CLHEP::GeV, fMom.y()*CLHEP::GeV, fMom.z()*CLHEP::GeV, energy));
    }

    for (int i=0; i<(int)fPidVector.size(); i++) { // add vertices for G4 tracking (status code = 1)  

      HepMC::GenVertex*   gVertex   = new HepMC::GenVertex  (fPosVector.at(i));
      HepMC::GenParticle* gParticle = new HepMC::GenParticle(fMomVector.at(i), fPidVector.at(i), 1);
      
      gVertex->add_particle_out(gParticle);
      
      gEvent->add_vertex(gVertex);
    }
  }
  
  return StatusCode::SUCCESS;
}
