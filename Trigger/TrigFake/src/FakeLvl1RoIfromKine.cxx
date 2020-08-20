/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************************e

NAME:     FakeLvl1RoIfromKine.cxx
PACKAGE:

AUTHORS: J. Baines        john.baines@rl.ac.uk based on TrigSteer/Lvl1Converstion by
         G. Comune        gianluca.comune@cern.ch (http://cern.ch/gcomune)

CREATED:  Nov, 2002

PURPOSE:  Fake Lvl1 RoI for seeding Lvl2

Modified :


********************************************************************/

#include <stdlib.h>
#include <vector>
#include <utility>
// INCLUDE GAUDI HEADER FILES:
#include "Gaudi/Property.h"
#include "GaudiKernel/ISvcLocator.h"

# include "CLHEP/Units/SystemOfUnits.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "FakeLvl1RoIfromKine.h"
#include "FakeRoI.h"
#include "Trajectory.h"


#include "TrigInterfaces/AlgoConfig.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "CLHEP/Vector/ThreeVector.h"

#include "TrigT1Result/EMTauRoI.h"
#include "TrigT1Result/JetEnergyResult.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"



using namespace TrigFake; // for Trajectory
using namespace LVL1;


////////////////////////////////////////////////////////////////
//  CONSTRUCTOR:

FakeLvl1RoIfromKine::FakeLvl1RoIfromKine(const std::string& name, const std::string& type,
						   const IInterface* parent)
  : HLT::LvlConverter(name, type, parent),
    m_roiId(0),
    m_mcEventCollectionName("")
{

  declareProperty("McEventCollectionName", m_mcEventCollectionName="GEN_EVENT");

  declareProperty( "FakeEmTauRoiParticleIDs", m_emTauRoiParticles);
  declareProperty( "FakeEmTauRoiPtMin", m_emTauRoiPtMin=20.*CLHEP::GeV);
  declareProperty( "FakeEmTauRoiEtaMax", m_emTauRoiEtaMax=2.5);

  declareProperty( "FakeMuonRoiParticleIDs",  m_muonRoiParticles);
  declareProperty( "FakeMuonRoiPtMin",  m_muonRoiPtMin=20.*CLHEP::GeV);
  declareProperty( "FakeMuonRoiEtaMax",  m_muonRoiEtaMax=2.5);

  declareProperty( "FakeJetRoiParticleIDs",   m_jetRoiParticles);
  declareProperty( "FakeJetRoiPtMin",   m_jetRoiPtMin=20.*CLHEP::GeV);
  declareProperty( "FakeJetRoiEtaMax",   m_jetRoiEtaMax=2.5);

  declareProperty( "FakeEmRoiLabel",   m_emRoiLabel  ="EM10");
  declareProperty( "FakeMuonRoiLabel", m_muonRoiLabel="MU06");
  declareProperty( "FakeJetRoiLabel",  m_jetRoiLabel ="JET20");
  declareProperty( "FakeTauRoiLabel",  m_tauRoiLabel ="HA10");

}

// DESTRUCTOR:

FakeLvl1RoIfromKine::~FakeLvl1RoIfromKine()
{ }

/////////////////////////////////////////////////////////////////
// INITIALIZE METHOD:

HLT::ErrorCode FakeLvl1RoIfromKine::hltInitialize()
{
  //  if ( m_particleDataTable.empty() ) {
  //(*m_log) << MSG::WARNING << "FakeLvl1RoIfromKine failed to read particleDataTable" << endmsg;
  //return StatusCode::FAILURE;
  // }

  if (! m_emTauRoiParticles.empty()) {
    std::vector<int>::iterator emParticle=m_emTauRoiParticles.begin();
    std::vector<int>::iterator lastEmParticle=m_emTauRoiParticles.end();
    msg(MSG::INFO) << " Forming fake EMROI (" << m_emRoiLabel << ") from the following kine particle IDs : ";
    for ( ; emParticle != lastEmParticle ; emParticle++ ) {
      msg() << *emParticle << " " ;
    }
    msg() << endmsg;
    ATH_MSG_INFO(" with ET > " << m_emTauRoiPtMin <<
                 " and |eta| < " << m_emTauRoiEtaMax);
  } else {
    ATH_MSG_INFO(" Not forming fake EMROI (" << m_emRoiLabel << ")");
  }

  if ( ! m_muonRoiParticles.empty()) {
    std::vector<int>::iterator muParticle=m_muonRoiParticles.begin();
    std::vector<int>::iterator lastMuParticle=m_muonRoiParticles.end();
    msg(MSG::INFO) << " Forming fake MUONROI (" << m_muonRoiLabel << ") from the following kine particle IDs : ";
    for ( ; muParticle != lastMuParticle ; muParticle++ ) {
      msg() << *muParticle << " " ;
    }
    msg() << endmsg;
    ATH_MSG_INFO(" with pT > " << m_muonRoiPtMin <<
                 " and |eta| < " << m_muonRoiEtaMax);
  } else {
    ATH_MSG_INFO(" Not forming fake MUONROI (" << m_muonRoiLabel << ")");
  }


  if ( ! m_jetRoiParticles.empty()) {
    std::vector<int>::iterator muParticle=m_jetRoiParticles.begin();
    std::vector<int>::iterator lastMuParticle=m_jetRoiParticles.end();
    msg(MSG::INFO) << " Forming fake JETROI (" <<  m_jetRoiLabel << ") from the following kine particle IDs : ";
    for ( ; muParticle != lastMuParticle ; muParticle++ ) {
      msg() << *muParticle << " " ;
    }
    msg() << endmsg;
    ATH_MSG_INFO(" with pT > " << m_jetRoiPtMin<<
                 " and |eta| < " << m_jetRoiEtaMax);
  } else {
    ATH_MSG_INFO(" Not forming fake JETROI (" <<  m_jetRoiLabel << ")");
  }

  return HLT::OK;
}

/////////////////////////////////////////////////////////////////
// FINALIZE METHOD:

HLT::ErrorCode FakeLvl1RoIfromKine::hltFinalize()
{
  return HLT::OK;
}

/////////////////////////////////////////////////////////////////
// EXECUTE METHOD:

HLT::ErrorCode FakeLvl1RoIfromKine::hltExecute( std::vector<HLT::SteeringChain*>& activeSteeringChains )
{
  ATH_MSG_DEBUG("Executing FakeLvl1RoIfromKine");

  // activate all configured chains:
  ATH_MSG_DEBUG("activating all configured chains.");

  for (std::vector<HLT::SteeringChain*>::const_iterator it = m_configuredChains->begin();
       it != m_configuredChains->end(); ++it) {
    // set to active:
    (*it)->setActive();
    // set pass through state:
    (*it)->setPassThroughState();
    activeSteeringChains.push_back( (*it) );
  }


  //  HLT::TriggerElement* initialTE = m_config->getNavigation()->getInitialNode();
  //  (*m_log) << MSG::DEBUG << "initial Navigation node created." << endmsg;

  m_roiId=0;
  std::vector<FakeRoI> *roiList;
  roiList = createRoIfromMC();


  int nCreatedRoI=0;
  std::vector< FakeRoI>::iterator firstroi =  roiList->begin();
  std::vector< FakeRoI>::iterator lastRoi =   roiList->end();
  for ( std::vector< FakeRoI>::iterator iroi = firstroi; iroi != lastRoi; iroi++) {

    // skip duplicate roi
    bool doneAlready = false;
    std::vector< FakeRoI>::iterator jroi = firstroi;
    for (; jroi != iroi; jroi++ ) {
      if ( iroi->type() == jroi->type()){
	double deta = fabs (jroi->eta() - iroi->eta());
	double dphi = (jroi->phi() - iroi->phi());
	double absdphi = fabs(dphi);
	if (2.*M_PI-absdphi < absdphi) {
	  if (dphi > 0) { dphi = dphi - 2.*M_PI;} else { dphi = 2*M_PI - absdphi;}
	}
        if ((fabs(deta) < 0.1) && (fabs (dphi) < 0.1)) doneAlready=true;
      }
    }
    if (not doneAlready) {
      if ( (this)->createRoI(*iroi).isSuccess()) nCreatedRoI++;
    }
  }

  if ( nCreatedRoI == 0) {
    ATH_MSG_DEBUG("FakeLvl1RoIfromKine did not create any RoI");
} else {
    ATH_MSG_DEBUG("FakeLvl1RoIfromKine created " << nCreatedRoI << " RoI");
  }

  delete roiList;

  return HLT::OK;
}

StatusCode FakeLvl1RoIfromKine::createRoI(FakeRoI &fakeRoi) {



  HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( m_config->getNavigation()->getInitialNode() );
  const float zspread = 168.;
  
  TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, 0, m_roiId,
							   fakeRoi.eta(),fakeRoi.eta()-0.1,fakeRoi.eta()+0.1,
							   fakeRoi.phi(),fakeRoi.phi()-0.1,fakeRoi.phi()+0.1,
							   0.-zspread, zspread);
  std::string key;
  m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );

  // build threshold TE
  unsigned int teId;
  TrigConf::HLTTriggerElement::getId(fakeRoi.type().c_str(), teId);
  m_config->getNavigation()->addNode(roiTE, teId);

  m_roiId++;

  /*
    if(fakeRoi.type() == m_emRoiLabel) getStepSequencerPointer()->addTE(createSeed((key2keyStore*)0x00000000,fakeRoi.eta(), fakeRoi.phi(), Frame::instance()->getId2Str(m_emRoiLabel), m_roiId++));
    if(fakeRoi.type() == m_muonRoiLabel) getStepSequencerPointer()->addTE(createSeed((key2keyStore*)0x00000000,fakeRoi.eta(), fakeRoi.phi(), Frame::instance()->getId2Str(m_muonRoiLabel) ,m_roiId++));
    if(fakeRoi.type() == m_jetRoiLabel) getStepSequencerPointer()->addTE(createSeed((key2keyStore*)0x00000000,fakeRoi.eta(), fakeRoi.phi(), Frame::instance()->getId2Str(m_jetRoiLabel) ,m_roiId++));
    if(fakeRoi.type() == m_tauRoiLabel) getStepSequencerPointer()->addTE(createSeed((key2keyStore*)0x00000000,fakeRoi.eta(), fakeRoi.phi(), Frame::instance()->getId2Str(m_tauRoiLabel) ,m_roiId++));
  */


  ATH_MSG_DEBUG(" REGTEST created an " <<  fakeRoi.type() << " RoI " << *roiDescriptor);


  return StatusCode::SUCCESS;
}

std::vector<FakeRoI> * FakeLvl1RoIfromKine::createRoIfromMC() {

  std::vector<FakeRoI> *fakeRoiList = new std::vector<FakeRoI>;

  //  Get the collection generator events (physics and pile-up) making up this event

  const McEventCollection* mcEvents;
  StatusCode sc = evtStore()->retrieve( mcEvents, m_mcEventCollectionName );
  if( sc.isFailure() ) {
    ATH_MSG_INFO("Could not retrieve mcEventCollection with key " << m_mcEventCollectionName);
    return fakeRoiList;
  };

  // Loop over the generator events
  McEventCollection::const_iterator  evt = mcEvents->begin();
  //McEventCollection::const_iterator  lastevt = mcEvents->end();

  // for ( ; evt != lastevt; evt++) {
  double qq = 0;

  // loop over Particles in this generator event
  for ( HepMC::GenEvent::particle_const_iterator p = (*evt)->particles_begin();
	p != (*evt)->particles_end(); ++p ) {

    int pdgid= (*p)->pdg_id();

    std::vector<int>::iterator emParticle=m_emTauRoiParticles.begin();
    std::vector<int>::iterator lastEmParticle=m_emTauRoiParticles.end();
    bool formEmTauRoi=false;
    for ( ; emParticle != lastEmParticle ; emParticle++ ) {
      if (pdgid == *emParticle && (*p)->momentum().perp() > m_emTauRoiPtMin &&
	 (*p)->momentum().pseudoRapidity() < m_emTauRoiEtaMax ) formEmTauRoi=true;
    }
    if ( formEmTauRoi ) {
      // temp fix:
      if (pdgid == 22) {
	qq = 0.;
      } else if (pdgid > 0) { qq = -1; } else { qq = 1; };
      Trajectory track((*p)->momentum().px(), (*p)->momentum().py(), (*p)->momentum().pz(),
		       (*p)->production_vertex()->position().x(),
		       (*p)->production_vertex()->position().y(),
		       (*p)->production_vertex()->position().z(), qq);

      ATH_MSG_VERBOSE(" Forming EMROI (" << m_emRoiLabel << ") from kine ID " << pdgid << " charge " << qq);
      ATH_MSG_VERBOSE(" Address " << std::hex << (*p) << std::dec);
      ATH_MSG_VERBOSE(" px " << (*p)->momentum().px() <<
                      " py " <<  (*p)->momentum().py() << " pz " << (*p)->momentum().pz());
      ATH_MSG_VERBOSE(" vx " << (*p)->production_vertex()->position().x() <<
                      " vy " <<  (*p)->production_vertex()->position().y() <<
                      " vz " << (*p)->production_vertex()->position().z());
      ATH_MSG_VERBOSE(" pt " << (*p)->momentum().perp() <<
                      " phi " << (*p)->momentum().phi() << " eta " << track.eta());
      ATH_MSG_VERBOSE(" Closest approach to origin : d0 " << track.d0() <<
                      " phi0 " << track.phi0() << " z0 " << track.z0());
#define RCAL 147.
#define ZCAL 380.
      std::pair<double, double> etaPhi = track.etaPhiAtCylinder(RCAL, ZCAL);

      ATH_MSG_VERBOSE(" At calorimeter : phi " << etaPhi.second <<
	" eta " << etaPhi.first);
      FakeRoI fakeRoi(m_emRoiLabel, etaPhi.second, etaPhi.first);
      fakeRoiList->push_back(fakeRoi);
    }

    // MUONROI

    std::vector<int>::iterator muParticle=m_muonRoiParticles.begin();
    std::vector<int>::iterator lastMuParticle=m_muonRoiParticles.end();
    bool formMuonRoi=false;
    for ( ; muParticle != lastMuParticle ; muParticle++ ) {
      if (pdgid == *muParticle && (*p)->momentum().perp() > m_muonRoiPtMin &&
	 (*p)->momentum().pseudoRapidity() < m_muonRoiEtaMax) formMuonRoi=true;
    }
    if ( formMuonRoi ) {

      ATH_MSG_VERBOSE(" Forming MUONROI (" << m_muonRoiLabel << ") from kine ID " << pdgid <<
                      " pt " << (*p)->momentum().perp() <<
                      " phi " << (*p)->momentum().phi() <<
                      " eta " << (*p)->momentum().pseudoRapidity());


      Trajectory track((*p)->momentum().px(), (*p)->momentum().py(), (*p)->momentum().pz(),
		       (*p)->production_vertex()->position().x(),
		       (*p)->production_vertex()->position().y(),
		       (*p)->production_vertex()->position().z(), qq);

#define RCAL 147.
#define ZCAL 380.
      std::pair<double, double> etaPhi = track.etaPhiAtCylinder(RCAL, ZCAL);

      ATH_MSG_VERBOSE(" At ID outer radius : phi " << etaPhi.second <<
                      " eta " << etaPhi.first);
      FakeRoI fakeRoi(m_muonRoiLabel, etaPhi.second, etaPhi.first);
      fakeRoiList->push_back(fakeRoi);

    }

    // JETROI

    std::vector<int>::iterator jetParticle=m_jetRoiParticles.begin();
    std::vector<int>::iterator lastJetParticle=m_jetRoiParticles.end();
    bool formJetRoi=false;
    for ( ; jetParticle != lastJetParticle ; jetParticle++ ) {
      if (pdgid == *jetParticle && (*p)->momentum().perp() > m_jetRoiPtMin &&
	 (*p)->momentum().pseudoRapidity() < m_jetRoiEtaMax) formJetRoi=true;
    }
    if ( formJetRoi ) {

      ATH_MSG_VERBOSE(" Forming JETROI (" <<  m_jetRoiLabel << ") from kine ID " << pdgid <<
                      " pt " << (*p)->momentum().perp() <<
                      " phi " << (*p)->momentum().phi() <<
                      " eta " << (*p)->momentum().pseudoRapidity());
      FakeRoI fakeRoi(m_jetRoiLabel,(*p)->momentum().phi() ,(*p)->momentum().pseudoRapidity());
      fakeRoiList->push_back(fakeRoi);
    }

  }
  //}
  return fakeRoiList;
}
