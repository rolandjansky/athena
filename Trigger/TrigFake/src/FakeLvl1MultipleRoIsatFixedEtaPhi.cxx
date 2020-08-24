/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************


NAME:     FakeLvl1MultipleRoIsatFixedEtaPhi.cxx
PACKAGE:

AUTHORS: J. Boyd (copied from and then adapted FakeLvl1RoIatFixedEtaPhi

CREATED:  Apr, 2007

PURPOSE:  Fake Lvl1 RoI for seeding Lvl2

Modified :


********************************************************************/

#include <stdlib.h>
#include <vector>
#include <utility>
// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ISvcLocator.h"


# include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "FakeLvl1MultipleRoIsatFixedEtaPhi.h"
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

FakeLvl1MultipleRoIsatFixedEtaPhi::FakeLvl1MultipleRoIsatFixedEtaPhi(const std::string& name, const std::string& type,
						   const IInterface* parent)
  : HLT::LvlConverter(name, type, parent), 
    m_roiId(0),
    m_lvl1ID(1)
{
  declareProperty( "FakeEmRoiPhi", m_emRoiPhi);
  declareProperty( "FakeEmRoiEta", m_emRoiEta);

  declareProperty( "FakeTauRoiPhi", m_tauRoiPhi);
  declareProperty( "FakeTauRoiEta", m_tauRoiEta);

  declareProperty( "FakeMuonRoiPhi", m_muonRoiPhi);
  declareProperty( "FakeMuonRoiEta", m_muonRoiEta);

  declareProperty( "FakeJetRoiPhi", m_jetRoiPhi);
  declareProperty( "FakeJetRoiEta", m_jetRoiEta);

  declareProperty( "FakeEmRoiLabel",   m_emRoiLabel  );
  declareProperty( "FakeMuonRoiLabel", m_muonRoiLabel);
  declareProperty( "FakeJetRoiLabel",  m_jetRoiLabel );
  declareProperty( "FakeTauRoiLabel",  m_tauRoiLabel );

}

// DESTRUCTOR:

FakeLvl1MultipleRoIsatFixedEtaPhi::~FakeLvl1MultipleRoIsatFixedEtaPhi()
{ }



/////////////////////////////////////////////////////////////////
// INITIALIZE METHOD:

HLT::ErrorCode FakeLvl1MultipleRoIsatFixedEtaPhi::hltInitialize()
{
  // check that the vectors for emRoi are the same length
  if (m_emRoiPhi.size() != m_emRoiEta.size() || m_emRoiPhi.size() != m_emRoiLabel.size()) {    msg() << MSG::FATAL << " problem configuring EMROI sizes of Phi, Eta, Label vectors different, phi " << m_muonRoiPhi.size()  << "  eta " << m_emRoiEta.size()  << "  label "<<m_emRoiLabel.size()  <<endmsg;
  return HLT::BAD_JOB_SETUP;
  }
  if (m_emRoiPhi.size() == 0) msg() << MSG::INFO << " Not forming any fake EMROIs" << endmsg;

  for (size_t em=0;em<m_emRoiPhi.size();em++){
    if ( m_emRoiPhi.at(em) <= M_PI && m_emRoiPhi.at(em) >= -M_PI ) {
      msg() << MSG::INFO << " Forming fake EMROI with name " << m_emRoiLabel.at(em)  <<" at phi =  " <<
        m_emRoiPhi.at(em) << " eta = "  << m_emRoiEta.at(em) << endmsg;
    } else {
      msg() << MSG::INFO << " Not forming fake EMROI " <<m_emRoiLabel.at(em)  << " phi out of range"<< endmsg;
    }
  }

  // check that the vectors for tauRoi are the same length
  if (m_tauRoiPhi.size() != m_tauRoiEta.size() || m_tauRoiPhi.size() != m_tauRoiLabel.size()) {
    msg() << MSG::FATAL << " problem configuring TAUROI sizes of Phi, Eta, Label vectors different, phi " << m_muonRoiPhi.size()  << "  eta " << m_tauRoiEta.size()  << "  label "<< m_tauRoiLabel.size()  <<endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  if (m_tauRoiPhi.size() == 0) msg() << MSG::INFO << " Not forming any fake TAUROIs" <<
endmsg;
  for (size_t tau=0;tau<m_tauRoiPhi.size();tau++){
    if ( m_tauRoiPhi.at(tau) <= M_PI && m_tauRoiPhi.at(tau) >= -M_PI ) {
      msg() << MSG::INFO << " Forming fake TAUROI with name " << m_tauRoiLabel.at(tau)
<< " at phi =  " <<
        m_tauRoiPhi.at(tau) << " eta = "  << m_tauRoiEta.at(tau) << endmsg;
    } else {
      msg() << MSG::INFO << " Not forming fake TAUROI " <<m_tauRoiLabel.at(tau)  << " phi out of range"<< endmsg;
    }
  }
  // check that the vectors for muonRoi are the same length
  if (m_muonRoiPhi.size() != m_muonRoiEta.size() || m_muonRoiPhi.size() != m_muonRoiLabel.size()) {
    msg() << MSG::FATAL << " problem configuring MUONROI sizes of Phi, Eta, Label vectors different, phi " << m_muonRoiPhi.size()  << "  eta " << m_muonRoiEta.size()  << "  label "<< m_muonRoiLabel.size()  <<endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  if (m_muonRoiPhi.size() == 0) msg() << MSG::INFO << " Not forming any fake MUONROIs" << endmsg;

  // print out the MUONROIs that will be created...
  for (size_t mu=0;mu<m_muonRoiPhi.size();mu++){
    if ( m_muonRoiPhi.at(mu) <= M_PI && m_muonRoiPhi.at(mu) >= -M_PI ) {
      msg() << MSG::INFO << " Forming fake MUONROI " <<mu<< " with name " << m_muonRoiLabel.at(mu)  << "  at phi =  " <<
        m_muonRoiPhi.at(mu) << " eta = "   << m_muonRoiEta.at(mu) << endmsg;
    } else {
      msg() << MSG::INFO << " Not forming fake MUONROI " <<mu << "phi out of range " <<endmsg;
    }
  }


  // check that the vectors for jetRoi are the same length
  if (m_jetRoiPhi.size() != m_jetRoiEta.size() || m_jetRoiPhi.size() != m_jetRoiLabel.size()) {
    msg() << MSG::FATAL << " problem configuring JETROI sizes of Phi, Eta, Label vectors different, phi " << m_muonRoiPhi.size()  << "  eta " << m_jetRoiEta.size()  << "  label "<< m_jetRoiLabel.size()  <<endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  if (m_jetRoiPhi.size() == 0) msg() << MSG::INFO << " Not forming any fake JETROIs" <<
endmsg;
  for (size_t j=0;j<m_jetRoiPhi.size();j++){
    if ( m_jetRoiPhi.at(j) <= M_PI && m_jetRoiPhi.at(j) >= -M_PI ) {
      msg() << MSG::INFO << " Forming fake JETROI with name " << m_jetRoiLabel.at(j)  << "  at phi =  " <<
        m_jetRoiPhi.at(j) << " eta = "  <<  m_jetRoiEta.at(j) << endmsg;
    } else {
      msg() << MSG::INFO << " Not forming fake JETROI " <<m_jetRoiLabel.at(j)  << " phi out of range "<< endmsg;
    }
  }


  msg() << MSG::INFO << name() << ": Initialization completed successfully" << endmsg;

  return HLT::OK;
}


/////////////////////////////////////////////////////////////////
// EXECUTE METHOD:

HLT::ErrorCode FakeLvl1MultipleRoIsatFixedEtaPhi::hltExecute( std::vector<HLT::SteeringChain*>& activeSteeringChains ) {





  msg() << MSG::DEBUG << "Executing FakeLvl1MultipleRoIsatFixedEtaPhi" << endmsg;

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

  for (size_t em=0; em<m_emRoiPhi.size();em++){
    if ( m_emRoiPhi.at(em) <= M_PI && m_emRoiPhi.at(em) >= -M_PI ) {
      HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );
      TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, m_lvl1ID, m_roiId++,
							       m_emRoiEta.at(em), m_emRoiEta.at(em)-0.1,m_emRoiEta.at(em)+0.1,
							       m_emRoiPhi.at(em), m_emRoiPhi.at(em)-0.1, m_emRoiPhi.at(em)+0.1,
							       0.,-zspread,zspread);
      
      std::string key;
      m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );
      
      // build threshold TE
      unsigned int teId;
      TrigConf::HLTTriggerElement::getId(m_emRoiLabel.at(em).c_str(), teId);
      m_config->getNavigation()->addNode(roiTE, teId);
      
      msg() << MSG::DEBUG << "created "<< m_emRoiLabel.at(em) << endmsg;
    }
  }
  
  for (size_t t=0; t<m_tauRoiPhi.size();t++){
    if ( m_tauRoiPhi.at(t) <= M_PI && m_tauRoiPhi.at(t) >= -M_PI ) {
      HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );
      TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, m_lvl1ID, m_roiId++,
							       m_tauRoiEta.at(t), m_tauRoiEta.at(t)-0.4, m_tauRoiEta.at(t)+0.4,
							       m_tauRoiPhi.at(t), m_tauRoiPhi.at(t)-0.4, m_tauRoiPhi.at(t)+0.4,
							       0.,-zspread,zspread);
      
      std::string key;
      m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );

      // build threshold TE
      unsigned int teId;
      TrigConf::HLTTriggerElement::getId(m_tauRoiLabel.at(t).c_str(), teId);
      m_config->getNavigation()->addNode(roiTE, teId);
      msg() << MSG::DEBUG << "created" << m_tauRoiLabel.at(t) << endmsg;
    }
  }

  for (size_t m=0; m<m_muonRoiPhi.size();m++){
    if ( m_muonRoiPhi.at(m) <= M_PI && m_muonRoiPhi.at(m) >= -M_PI ) {
    HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );
    TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, m_lvl1ID, m_roiId++,
							     m_muonRoiEta.at(m),m_muonRoiEta.at(m)-0.1,m_muonRoiEta.at(m)+0.1,
							     m_muonRoiPhi.at(m),m_muonRoiPhi.at(m)-0.1,m_muonRoiPhi.at(m)+0.1,
							       0.,-zspread,zspread);

    std::string key;
    m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );

    // build threshold TE
    unsigned int teId;
    TrigConf::HLTTriggerElement::getId(m_muonRoiLabel.at(m).c_str(), teId);
    m_config->getNavigation()->addNode(roiTE, teId);
    msg() << MSG::DEBUG << "created"<< m_muonRoiLabel.at(m) << endmsg;
    }
  }

  for (size_t j=0; j<m_jetRoiPhi.size();j++){
    if ( m_jetRoiPhi.at(j) <= M_PI && m_jetRoiPhi.at(j) >= -M_PI ) {
      HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );
      TrigRoiDescriptor* roiDescriptor = new TrigRoiDescriptor(0, m_lvl1ID, m_roiId++,
							       m_jetRoiEta.at(j),m_jetRoiEta.at(j)-0.4,m_jetRoiEta.at(j)+0.4,
							       m_jetRoiPhi.at(j),m_jetRoiPhi.at(j)-0.4,m_jetRoiPhi.at(j)+0.4,
							       0.,-zspread,zspread);


      std::string key;
      m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "initialRoI" );
      
      // build threshold TE
      unsigned int teId;
      TrigConf::HLTTriggerElement::getId(m_jetRoiLabel.at(j).c_str(), teId);
      m_config->getNavigation()->addNode(roiTE, teId);
    msg() << MSG::DEBUG << "created " << m_jetRoiLabel.at(j) << endmsg;
    }
  }

  m_lvl1ID++;
  return HLT::OK;
}

/////////////////////////////////////////////////////////////////
// FINALIZE METHOD:

HLT::ErrorCode FakeLvl1MultipleRoIsatFixedEtaPhi::hltFinalize() 
{ 
  return HLT::OK; 
}
