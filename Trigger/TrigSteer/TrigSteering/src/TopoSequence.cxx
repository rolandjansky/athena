/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : TopoSequence
 *
 * @brief  one sequence node, i.e.
 *         input TE types -> N x Fex algorithm, [Hypo algorithm] -> output TE type
 *
 * @author Ingrid Deigaard     <ingrid.deigaard@cern.ch>     Nikhef, NL
 *
 * File and Version Information:
 * $Id: TopoSequence.cxx 780626 2016-10-27 01:00:33Z ssnyder $
 **********************************************************************************/

#include <iostream>

#include "TrigSteering/Sequence.h"
#include "TrigSteering/TopoSequence.h"

#include <algorithm>
#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/TECreateAlgo.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigInterfaces/ExceptionHandler.h"
#include "TrigNavigation/TriggerElement.h"

#include "AthenaKernel/Timeout.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/GaudiException.h"
#include "eformat/Issue.h"

#include "GaudiKernel/ServiceHandle.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "GaudiKernel/AlgTool.h"

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/CompositeTOB.h"


#include "TrigStorageDefinitions/EDM_TypeInfo.h"
#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"


#include "boost/lexical_cast.hpp"

using namespace HLT;
using namespace std;

TopoSequence::TopoSequence(HLT::AlgoConfig* config, unsigned int outputType,TCS::TopoSteering *ts) :
   Sequence( std::vector<unsigned int>(), outputType, /*firstAlgo=*/ nullptr, std::vector<HLT::Algo*>(), config, std::vector<unsigned int>()),
   m_topoSteer(ts)
{
   TrigConf::HLTTriggerElement::getLabel(outputType, m_name);

   if (m_config->getMsgLvl() <=MSG::DEBUG)
      m_config->getMsgStream() << MSG::DEBUG << "Constructing TopoSequence '" << m_name << "'" << endmsg;

}



TriggerElement * 
TopoSequence::getTEFromRoiWord(uint32_t roiWord) const {
   if(m_roiWord2TEMapping == nullptr ) {
      m_config->getMsgStream() << MSG::WARNING << "In TopoSequence '" << m_name << "' no roiWord to TE mapping available " << endmsg;
      return nullptr;
   }
   auto te = m_roiWord2TEMapping->find(roiWord);
   if(te == m_roiWord2TEMapping->end()) {
      m_config->getMsgStream() << MSG::WARNING << "In TopoSequence '" << m_name << "' could not find a TriggerElement for ROI " << roiWord << endmsg;
      return nullptr;
   }

   return te->second;
}


struct InsertionStructure {
   xAOD::TrigComposite * composite;
   vector<xAOD::EmTauRoI *> emTauComponents;
};

HLT::ErrorCode
TopoSequence::execute()
{

   if (m_alreadyExecuted){
      if (m_config->getMsgLvl() <=MSG::DEBUG)
         m_config->getMsgStream() << MSG::DEBUG << "TopoSequence already executed -> doing nothing" << endmsg;
      return m_execErrorCode;
   }

   m_alreadyExecuted = true;
   m_execErrorCode = OK;

   if (m_config->getMsgLvl() <=MSG::DEBUG)
      m_config->getMsgStream() << MSG::DEBUG << "Executing TopoSequence '" << m_name << "'" << endmsg;

   // execute the topo trigger
   m_topoSteer->executeTrigger(m_name);

   // the pass-fail decision
   //   bool success = 
   m_topoSteer->simulationResult().triggerDecision(m_name);

   // access the output
   const TCS::TOBArray* topoOutput = m_topoSteer->simulationResult().triggerOutput(m_name);

   if (m_config->getMsgLvl() <=MSG::DEBUG)
      m_config->getMsgStream() << MSG::DEBUG << "TopoSequence '" << m_name << "' successfully executed topo alg. Output size " << topoOutput->size() << endmsg;


   // get initial leaf in Navigation
   //    HLT::TriggerElement* initialTE = m_config->getNavigation()->getInitialNode();
   //    m_config->getMsgStream() << MSG::DEBUG << "initial Navigation node created" << endmsg;

   // create the TEs (one per topo output and activate them)

   unsigned int teId;
   TrigConf::HLTTriggerElement::getId(m_name.c_str(),teId);

//    xAOD::TrigCompositeContainer * compCont = new xAOD::TrigCompositeContainer();
//    xAOD::TrigCompositeAuxContainer * compAux = new xAOD::TrigCompositeAuxContainer();
//    compCont->setStore(compAux);
//   std::string sgkey("");
//   unsigned int compositeCount(0);


   /**
    * Creating TrigComposites for HLT algorithms and offline analysis
    *
    * 1. Loop over the topo output and for each CompositeTOB do:
    * 2. Create a TrigComposite
    * 3. For each composite component create an xAOD equivalent (EmTau/Muon/Jet/MET) and record with navigation
    * 4. Retrieve ElementLinks to the component objects and attach to TrigComposite
    * 5. Record the TrigComposite with the navigation
    *

   */

   // 1. loop over the topo output
   for(const TCS::GenericTOB * tob : *topoOutput) {

      const TCS::CompositeTOB * comptob = dynamic_cast<const TCS::CompositeTOB*>(tob);
      
      if(comptob == nullptr) {
         m_config->getMsgStream() << MSG::ERROR << "Output of topo trigger '" << m_name << "' is not a CompositeTOB" << endmsg;
         m_execErrorCode = HLT::FATAL;
         continue;
      }

      if (m_config->getMsgLvl() <=MSG::DEBUG)
         m_config->getMsgStream() << MSG::DEBUG << "TopoSequence '" << m_name << "' Combination has size " << comptob->components().size() << ":" << endmsg;


      // FOR EACH COMPOSITE TOB DO:

      /****************
       *
       * FIRST PART: create the navigation structure
       *
       **/

      // get the roiWord for each component of the composite and the corresponding TriggerElement
      vector<TriggerElement*> componentTEs;
      for(const TCS::GenericTOB * tob : comptob->components()) {

         uint32_t roiWord = tob->roiWord();

         auto findTE = m_roiWord2TEMapping->find(roiWord);

         if(findTE != m_roiWord2TEMapping->end()) {
            if (m_config->getMsgLvl() <=MSG::DEBUG) {
               string parentTEName("");
               TrigConf::HLTTriggerElement::getLabel(findTE->second->getId(), parentTEName);
               m_config->getMsgStream() << MSG::DEBUG << "TopoSequence '" << m_name << "'  roiWord " << roiWord << ", parent TE " << findTE->second << ", id = " << findTE->second->getId() << " (" << parentTEName << ")" << endmsg;
            }
            componentTEs.push_back( findTE->second );
         } else {
            m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "'. No TriggerElement found for roiWord " << roiWord << endmsg;
         }
      }

      auto * nav = m_config->getNavigation();

      HLT::TriggerElement* outputTE = nav->addNode(componentTEs, teId);

      outputTE->setActiveState(true);


//       /****************
//        *
//        * SECOND PART: attach features to
//        *
//        **/

//       std::cout << "MARK A1" << std::endl;

//       // 2. create a new TrigComposite
//       xAOD::TrigComposite * composite = new xAOD::TrigComposite();
//       std::cout << "MARK A2" << std::endl;
//       string compositeName = m_name+"_obj_"+boost::lexical_cast<std::string,unsigned int>(compositeCount++);
//       //       std::cout << "MARK A3 " << compositeName << std::endl;
//       //       composite->setName( compositeName ); // crashes
//       std::cout << "MARK A4" << std::endl;
//       compCont->push_back( composite );

//       std::cout << "MARK B" << std::endl;

//       // loop over the components of the CompositeTOB
//       uint x=0;
//       uint emCount(0),tauCount(0),muonCount(0),jetCount(0);
//       for(const TCS::GenericTOB * tob : comptob->components()) {

//          std::cout << "MARK C" << std::endl;

//          if (m_config && m_config->getMsgLvl() <=MSG::DEBUG) {
//             m_config->getMsgStream() << MSG::DEBUG << "TopoSequence " << m_name << " [" << x++ << "] : generic tob energy of type " << TCS::inputTypeAsString(tob->tobType()) << " ["<<tob->tobType()<<"]: " << tob->Et() << ", eta: " << tob->eta() << ", phi: " << tob->phi() << endmsg;
//          }

//          std::cout << "MARK D" << std::endl;

//          // Cluster
//          if(tob->tobType()==TCS::CLUSTER) {
//             // create and fill EmTauRoI
//             xAOD::EmTauRoI * roi = new xAOD::EmTauRoI();
//             roi->makePrivateStore();
//             roi->initialize(0xa0000000,(float)tob->eta(),(float)tob->phi());
//             roi->setEmClus(tob->Et());

//             // attach to navigation
//             if( ! m_config->getNavigation()->attachFeature( outputTE, roi, HLT::Navigation::ObjectToStoreGate, sgkey, "L1TopoEM" ) ) {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to attach xAOD::EmTauRoI to navigation" << endmsg;            
//             }
//             ElementLink<xAOD::EmTauRoIContainer> link;
//             if( m_config->getNavigation()->getRecentFeatureLink<xAOD::EmTauRoIContainer,xAOD::EmTauRoI>( outputTE, link, "L1TopoEM") ) {
//                if(link.cptr() != nullptr) {
//                   if (m_config && m_config->getMsgLvl() <=MSG::DEBUG) {
//                      const xAOD::EmTauRoI * linkedROI = * link.cptr();
//                      m_config->getMsgStream() << MSG::DEBUG << "EmTauRoI from EL with et = " << linkedROI->emClus() << ", eta = " << linkedROI->eta() << ", phi = " << linkedROI->phi() << endmsg;
//                   }
//                   // attaching to TrigComposite
//                   std::string name( "EM" + boost::lexical_cast<std::string,unsigned int>(emCount++) );
//                   composite->setObjectLink( name, link );
//                } else {
//                   if (m_config)
//                      m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': retrieved ElementLink<xAOD::EmTauRoiContainer> for composite component (em) with empty link ptr" << endmsg;
//                }
//             } else {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to retrieve ElementLink<xAOD::EmTauRoiContainer> for composite component (em)" << endmsg;
//             }
//          }
         
//          std::cout << "MARK E" << std::endl;


//          if(tob->tobType()==TCS::TAU) {
//             // create and fill EmTauRoI
//             xAOD::EmTauRoI * roi = new xAOD::EmTauRoI();
//             roi->makePrivateStore();
//             roi->initialize(0xa0000000,(float)tob->eta(),(float)tob->phi());
//             roi->setEmClus(tob->Et());

//             // attach to navigation
//             if( ! m_config->getNavigation()->attachFeature( outputTE, roi, HLT::Navigation::ObjectToStoreGate, sgkey, "L1TopoTau" ) ) {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to attach xAOD::EmTauRoI to navigation" << endmsg;            
//             }
//             ElementLink<xAOD::EmTauRoIContainer> link;
//             if( m_config->getNavigation()->getRecentFeatureLink<xAOD::EmTauRoIContainer,xAOD::EmTauRoI>( outputTE, link, "L1TopoTau") ) {
//                if(link.cptr() != nullptr) {
//                   if (m_config && m_config->getMsgLvl() <=MSG::DEBUG) {
//                      const xAOD::EmTauRoI * linkedROI = * link.cptr();
//                      m_config->getMsgStream() << MSG::DEBUG << "EmTauRoI from EL with et = " << linkedROI->emClus() << ", eta = " << linkedROI->eta() << ", phi = " << linkedROI->phi() << endmsg;
//                   }
//                   // attaching to TrigComposite
//                   std::string name( "TAU" + boost::lexical_cast<std::string,unsigned int>(tauCount++) );
//                   composite->setObjectLink( name, link );
//                } else {
//                   if (m_config)
//                      m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': retrieved ElementLink<xAOD::EmTauRoiContainer> for composite component (tau) with empty link ptr" << endmsg;
//                }
//             } else {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to retrieve ElementLink<xAOD::EmTauRoiContainer> for composite component (tau)" << endmsg;
//             }
//          }

//          std::cout << "MARK F" << std::endl;


//          if(tob->tobType()==TCS::MUON) {
//             // create and fill EmTauRoI
//             xAOD::MuonRoI * roi = new xAOD::MuonRoI();
//             roi->makePrivateStore();
//             roi->initialize(0x0,(float)tob->eta(),(float)tob->phi(),"MU"+boost::lexical_cast<string,int>(tob->Et()), tob->Et());

//             // attach to navigation
//             if( ! m_config->getNavigation()->attachFeature( outputTE, roi, HLT::Navigation::ObjectToStoreGate, sgkey, "L1TopoMuon" ) ) {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to attach xAOD::MuonRoI to navigation" << endmsg;            
//             }
//             ElementLink<xAOD::MuonRoIContainer> link;
//             if( m_config->getNavigation()->getRecentFeatureLink<xAOD::MuonRoIContainer,xAOD::MuonRoI>( outputTE, link, "L1TopoMuon") ) {
//                if(link.cptr() != nullptr) {
//                   if (m_config && m_config->getMsgLvl() <=MSG::DEBUG) {
//                      const xAOD::MuonRoI * linkedROI = * link.cptr();
//                      m_config->getMsgStream() << MSG::DEBUG << "MuonRoI from EL with et = " << linkedROI->thrValue() << ", eta = " << linkedROI->eta() << ", phi = " << linkedROI->phi() << endmsg;
//                   }
//                   // attaching to TrigComposite
//                   std::string name( "MUON" + boost::lexical_cast<std::string,unsigned int>(muonCount++) );
//                   composite->setObjectLink( name, link );
//                } else {
//                   if (m_config)
//                      m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': retrieved ElementLink<xAOD::MuonRoiContainer> for composite component with empty link ptr" << endmsg;
//                }
//             } else {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to retrieve ElementLink<xAOD::MuonRoiContainer> for composite component" << endmsg;
//             }
//          }

//          std::cout << "MARK G" << std::endl;


//          if(tob->tobType()==TCS::JET) {
//             // create and fill EmTauRoI
//             xAOD::JetRoI * roi = new xAOD::JetRoI();
//             roi->makePrivateStore();
//             roi->initialize(0x0,(float)tob->eta(),(float)tob->phi());
//             roi->setEt4x4(tob->EtNarrow());
//             roi->setEt8x8(tob->EtWide());

//             // attach to navigation
//             if( ! m_config->getNavigation()->attachFeature( outputTE, roi, HLT::Navigation::ObjectToStoreGate, sgkey, "L1TopoJet" ) ) {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to attach xAOD::JetRoI to navigation" << endmsg;            
//             }
//             ElementLink<xAOD::JetRoIContainer> link;
//             if( m_config->getNavigation()->getRecentFeatureLink<xAOD::JetRoIContainer,xAOD::JetRoI>( outputTE, link, "L1TopoJet") ) {
//                if(link.cptr() != nullptr) {
//                   if (m_config && m_config->getMsgLvl() <=MSG::DEBUG) {
//                      const xAOD::JetRoI * linkedROI = * link.cptr();
//                      m_config->getMsgStream() << MSG::DEBUG << "JetRoI from EL with Et8x8 = " << linkedROI->et8x8() << ", Et4x4 = " << linkedROI->et4x4() << ", eta = " << linkedROI->eta() << ", phi = " << linkedROI->phi() << endmsg;
//                   }
//                   // attaching to TrigComposite
//                   std::string name( "JET" + boost::lexical_cast<std::string,unsigned int>(jetCount++) );
//                   composite->setObjectLink( name, link );
//                } else {
//                   if (m_config)
//                      m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': retrieved ElementLink<xAOD::JetRoiContainer> for composite component with empty link ptr" << endmsg;
//                }
//             } else {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to retrieve ElementLink<xAOD::JetRoiContainer> for composite component" << endmsg;
//             }
//          }

         
//          std::cout << "MARK H" << std::endl;

//          if(tob->tobType()==TCS::MET) {
//             // create and fill EmTauRoI
//             xAOD::TrigComposite * met = new xAOD::TrigComposite();
//             met->makePrivateStore();
//             met->setName("EnergySum");
//             met->setDetail("Et", (int)tob->Et());
//             met->setDetail("Ex", (int)0);
//             met->setDetail("Ey", (int)0);
//             //met->setDetail("Ex", (int)tob->Ex());
//             //met->setDetail("Ey", (int)tob->Ey());

//             // attach to navigation
//             if( ! m_config->getNavigation()->attachFeature( outputTE, met, HLT::Navigation::ObjectToStoreGate, sgkey, "L1TopoMET" ) ) {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to attach xAOD::JetRoI to navigation" << endmsg;            
//             }
//             ElementLink<xAOD::TrigCompositeContainer> link;
//             if( m_config->getNavigation()->getRecentFeatureLink<xAOD::TrigCompositeContainer,xAOD::TrigComposite>( outputTE, link, "L1TopoMET") ) {
//                if(link.cptr() != nullptr) {
//                   if (m_config && m_config->getMsgLvl() <=MSG::INFO) {
//                      const xAOD::TrigComposite * linkedROI = * link.cptr();
//                      float et,ex,ey;
//                      linkedROI->getDetail("Et",et);
//                      linkedROI->getDetail("Ex",ex);
//                      linkedROI->getDetail("Ey",ey);
//                      m_config->getMsgStream() << MSG::INFO << "TrigComposite " << linkedROI->name() << " from EL with et = " << et << ", Ex = " << ex << ", Ey = " << ey << endmsg;
//                   }
//                   // attaching to TrigComposite
//                   std::string name( "EnergySum" );
//                   composite->setObjectLink( name, link );
//                } else {
//                   if (m_config)
//                      m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': retrieved ElementLink<xAOD::TrigCompositeContainer> for composite component with empty link ptr" << endmsg;
//                }
//             } else {
//                if (m_config)
//                   m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed to retrieve ElementLink<xAOD::TrigCompositeRoiContainer> for composite component" << endmsg;
//             }
//          }

//          std::cout << "MARK I" << std::endl;


//       }

//       std::cout << "MARK J" << std::endl;

//       // add the composite to the collection
//       if( ! m_config->getNavigation()->attachFeature( outputTE, compCont, HLT::Navigation::ObjectToStoreGate, sgkey, "L1TopoComposite" ) ) {
//          if (m_config)
//             m_config->getMsgStream() << MSG::ERROR << "TopoSequence '" << m_name << "': failed attaching TrigCompositeContainer to SG" << endmsg;
//       }

//       std::cout << "MARK K" << std::endl;


   }

   return m_execErrorCode;
}
