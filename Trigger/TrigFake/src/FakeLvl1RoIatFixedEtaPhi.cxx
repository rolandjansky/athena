/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     FakeLvl1RoIatFixedEtaPhi.cxx
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

# include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "FakeLvl1RoIatFixedEtaPhi.h"
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




////////////////////////////////////////////////////////////////
//  CONSTRUCTOR:

FakeLvl1RoIatFixedEtaPhi::FakeLvl1RoIatFixedEtaPhi(const std::string& name, const std::string& type,
						   const IInterface* parent)
  : HLT::LvlConverter(name, type, parent), 
    m_roiId(0),
    m_tauRoiPhi(99.),
    m_tauRoiEta(0.),
    m_emRoiPhi(99.),
    m_emRoiEta(0.),
    m_muonRoiPhi(99.),
    m_muonRoiEta(0.),
    m_jetRoiPhi(99.),
    m_jetRoiEta(0.),
    m_lvl1ID(1)
{
  declareProperty( "FakeEmRoiPhi", m_emRoiPhi=99.);
  declareProperty( "FakeEmRoiEta", m_emRoiEta);

  declareProperty( "FakeTauRoiPhi", m_tauRoiPhi=99.);
  declareProperty( "FakeTauRoiEta", m_tauRoiEta);

  declareProperty( "FakeMuonRoiPhi", m_muonRoiPhi=99.);
  declareProperty( "FakeMuonRoiEta", m_muonRoiEta);

  declareProperty( "FakeJetRoiPhi", m_jetRoiPhi=99.);
  declareProperty( "FakeJetRoiEta", m_jetRoiEta);

  declareProperty( "FakeEmRoiLabel",   m_emRoiLabel  ="EM01");
  declareProperty( "FakeMuonRoiLabel", m_muonRoiLabel="MU06");
  declareProperty( "FakeJetRoiLabel",  m_jetRoiLabel ="JT00");
  declareProperty( "FakeTauRoiLabel",  m_tauRoiLabel ="TAU01");

}

// DESTRUCTOR:

FakeLvl1RoIatFixedEtaPhi::~FakeLvl1RoIatFixedEtaPhi()
{ }



/////////////////////////////////////////////////////////////////
// INITIALIZE METHOD:

HLT::ErrorCode FakeLvl1RoIatFixedEtaPhi::hltInitialize()
{
  msg() << MSG::INFO << " FakeLvl1RoIatFixedEtaPhi creating the following RoI:  " << endmsg;
  if ( m_emRoiPhi <= M_PI && m_emRoiPhi >= -M_PI ) {
    msg() << MSG::INFO << " Forming fake EMROI with name " << m_emRoiLabel  << " at phi =  " <<
      m_emRoiPhi << " eta = "  << m_emRoiEta << endmsg;

  } else {
    msg() << MSG::INFO << " Not forming fake EMROI" << endmsg;
  }

  if ( m_tauRoiPhi <= M_PI && m_tauRoiPhi >= -M_PI ) {
    msg() << MSG::INFO << " Forming fake TAUROI with name " << m_tauRoiLabel  << " at phi =  " <<
      m_tauRoiPhi << " eta = "  << m_tauRoiEta << endmsg;

  } else {
    msg() << MSG::INFO << " Not forming fake TAUROI" << endmsg;
  }



  if ( m_muonRoiPhi <= M_PI && m_muonRoiPhi >= -M_PI ) {
    msg() << MSG::INFO << " Forming fake MUONROI with name " << m_muonRoiLabel  << "  at phi =  " <<
      m_muonRoiPhi << " eta = "   << m_muonRoiEta << endmsg;

  } else {
    msg() << MSG::INFO << " Not forming fake MUONROI" << endmsg;
  }


  if ( m_jetRoiPhi <= M_PI && m_jetRoiPhi >= -M_PI ) {
    msg() << MSG::INFO << " Forming fake JETROI with name " << m_jetRoiLabel  << "  at phi =  " <<
      m_jetRoiPhi << " eta = "  <<  m_jetRoiEta << endmsg;

  } else {
    msg() << MSG::INFO << " Not forming fake JETROI" << endmsg;
  }

  msg() << MSG::INFO << name() << ": Initialization completed successfully" << endmsg;

  return HLT::OK;
}


/////////////////////////////////////////////////////////////////
// EXECUTE METHOD:

HLT::ErrorCode FakeLvl1RoIatFixedEtaPhi::hltExecute( std::vector<HLT::SteeringChain*>& activeSteeringChains ) {





  msg() << MSG::DEBUG << "Executing FakeLvl1RoIatFixedEtaPhi" << endmsg;

  // activate all configured chains:
  msg() << MSG::DEBUG << "activating all configured chains." << endmsg;

  for (std::vector<HLT::SteeringChain*>::const_iterator it = m_configuredChains->begin();
       it != m_configuredChains->end(); ++it) {
    // set to active:
    (*it)->setActive();
    // set pass through state:
    (*it)->setPassThroughState();
    activeSteeringChains.push_back( (*it) );
  }

  HLT::TriggerElement* initialTE = m_config->getNavigation()->getInitialNode();
  msg() << MSG::DEBUG << "initial Navigation node created." << endmsg;

  // set Lvl1 ID in AlgoConfig:
  m_config->setLvl1Id(m_lvl1ID);


  m_roiId=0;
  const float zspread = 168.;
  if ( m_emRoiPhi <= M_PI && m_emRoiPhi >= -M_PI ) {
    //    getStepSequencerPointer()->addTE(createSeed((key2keyStore*)0x00000000,m_emRoiEta, m_emRoiPhi, Frame::instance()->getId2Str(m_emRoiLabel), m_roiId++));

    HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );
    TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, m_lvl1ID, m_roiId,
							     m_emRoiEta, m_emRoiEta-0.1,m_emRoiEta+0.1,
							     m_emRoiPhi,m_emRoiPhi-0.1,m_emRoiPhi+0.1,
							     0., -zspread, zspread);


    std::string key;
    m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );

    // build threshold TE
    unsigned int teId;
    TrigConf::HLTTriggerElement::getId(m_emRoiLabel.c_str(), teId);
    m_config->getNavigation()->addNode(roiTE, teId);

    msg() << MSG::DEBUG << "created "<< m_emRoiLabel << endmsg;
  }

  if ( m_tauRoiPhi <= M_PI && m_tauRoiPhi >= -M_PI ) {
    //    getStepSequencerPointer()->addTE(createSeed((key2keyStore*)0x00000000,m_tauRoiEta, m_tauRoiPhi, Frame::instance()->getId2Str(m_tauRoiLabel), m_roiId++));

    HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );
    TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, m_lvl1ID, m_roiId,
							     m_tauRoiEta,m_tauRoiEta-0.4,m_tauRoiEta+0.4,
							     m_tauRoiPhi,m_tauRoiPhi-0.4,m_tauRoiPhi+0.4,
							     0., -zspread, zspread);


    std::string key;
    m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );


    // build threshold TE
    unsigned int teId;
    TrigConf::HLTTriggerElement::getId(m_tauRoiLabel.c_str(), teId);
    m_config->getNavigation()->addNode(roiTE, teId);
    msg() << MSG::DEBUG << "created" << m_tauRoiLabel << endmsg;
  }

  if ( m_muonRoiPhi <= M_PI && m_muonRoiPhi >= -M_PI ) {
    //    getStepSequencerPointer()->addTE(createSeed((key2keyStore*)0x00000000, m_muonRoiEta, m_muonRoiPhi, Frame::instance()->getId2Str(m_muonRoiLabel) ,m_roiId++));
    HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );
    TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, m_lvl1ID, m_roiId,
							     m_muonRoiEta,m_muonRoiEta-0.1,m_muonRoiEta+0.1,
							     m_muonRoiPhi,m_muonRoiPhi-0.1,m_muonRoiPhi+0.1,
							     0., -zspread, zspread);




    std::string key;
    m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );


    // build threshold TE
    unsigned int teId;
    TrigConf::HLTTriggerElement::getId(m_muonRoiLabel.c_str(), teId);
    m_config->getNavigation()->addNode(roiTE, teId);
    msg() << MSG::DEBUG << "created"<< m_muonRoiLabel << endmsg;

  }

  if ( m_jetRoiPhi <= M_PI && m_jetRoiPhi >= -M_PI ) {
    //    getStepSequencerPointer()->addTE(createSeed((key2keyStore*)0x00000000,m_jetRoiEta, m_jetRoiPhi, Frame::instance()->getId2Str(m_jetRoiLabel) ,m_roiId++));
    HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );
    TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, m_lvl1ID, m_roiId,
							     m_jetRoiEta,m_jetRoiEta-0.4,m_jetRoiEta+0.4,
							     m_jetRoiPhi,m_jetRoiPhi-0.4,m_jetRoiPhi+0.4,   
							     0., -zspread, zspread);

    std::string key;
    m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );


    // build threshold TE
    unsigned int teId;
    TrigConf::HLTTriggerElement::getId(m_jetRoiLabel.c_str(), teId);
    m_config->getNavigation()->addNode(roiTE, teId);
    msg() << MSG::DEBUG << "created " << m_jetRoiLabel << endmsg;
  }

  m_lvl1ID++;
  return HLT::OK;
}

/////////////////////////////////////////////////////////////////
// FINALIZE METHOD:

HLT::ErrorCode FakeLvl1RoIatFixedEtaPhi::hltFinalize() 
{ 
  return HLT::OK; 
}
