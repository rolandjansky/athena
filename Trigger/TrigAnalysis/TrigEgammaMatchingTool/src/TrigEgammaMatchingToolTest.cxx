/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaMatchingToolTest.h"
#include "GaudiKernel/Property.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "DecisionHandling/HLTIdentifier.h"
#include <typeinfo>

#include "boost/algorithm/string.hpp"

using TrigCompositeUtils::LinkInfo;
using TrigCompositeUtils::findLink;
using TrigCompositeUtils::Decision;


using namespace Trig;
using namespace TrigCompositeUtils;




//**********************************************************************
namespace Trig{
  TrigEgammaMatchingToolTest::
    TrigEgammaMatchingToolTest(const std::string& name, 
            ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool",this)
  {
    declareProperty("TrigEgammaMatchingTool",m_matchTool);
    declareProperty("TriggerList",m_triggerList);
  }

  //**********************************************************************

  TrigEgammaMatchingToolTest::~TrigEgammaMatchingToolTest() { }

  //**********************************************************************

  StatusCode TrigEgammaMatchingToolTest::initialize() {
      
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools...");
    if ( (m_trigdec.retrieve()).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
      return StatusCode::FAILURE;
    }
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if(sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
      return sc;
    }
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode TrigEgammaMatchingToolTest::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  void TrigEgammaMatchingToolTest::inspect(const std::string trigger,const xAOD::Egamma *eg){

    if (eg) {
      
      ATH_MSG_INFO("Getting all associated objects for " << trigger);

      auto vec_trigel = match()->features<xAOD::TrigElectron>(trigger);
      ATH_MSG_INFO("The TrigElectron vec has " << vec_trigel.size() << " elements");

      auto vec_el = match()->features<xAOD::Electron>(trigger);
      ATH_MSG_INFO("The Electron vec has " << vec_el.size() << " elements");

      auto vec_emcluster = match()->features<xAOD::TrigEMCluster>(trigger);
      ATH_MSG_INFO("The TrigEMCluster vec has " << vec_emcluster.size() << " elements");




      // Creae the trig data pointer
      TrigEgammaMatchingUtils::Element obj(trigger);
      
      match()->match( eg, trigger , obj);

      ATH_MSG_INFO( "Checking matching methods... for trigger " << trigger );
      if ( obj.isValid() )
      {
        ATH_MSG_INFO( "Matched!");

        ATH_MSG_INFO( "Trying to retireve the TrigEMCluster from the element" );
        auto *emCluster = match()->getFeature<xAOD::TrigEMCluster>( obj );
        if( emCluster ){
          ATH_MSG_INFO( "emCluster = "<< emCluster );
        }else{
          ATH_MSG_INFO("TrigEMCluster is not valid.");
        }


        ATH_MSG_INFO( "Trying to retireve the TrigElectron from the element" );
        auto vec_el = match()->getFeatures<xAOD::TrigElectron>( obj );

        if( vec_el.isValid() ){
           ATH_MSG_INFO( "trigger electron container with "<< vec_el.size() << " objects");
        }else{
          ATH_MSG_INFO("TrigElectron vec is not valid.");
        }

        ATH_MSG_INFO( "Trying to retireve the CaloCluster from the element" );
        auto vec_cl = match()->getFeatures<xAOD::CaloCluster>( obj );


        if( vec_cl.isValid() ){
          ATH_MSG_INFO( "trigger calo cluster container with "<< vec_cl.size() << " objects");
        }else{
          ATH_MSG_INFO("CaloCluster vec is not valid.");
        }

        ATH_MSG_INFO("Trying to get all decisions steps:");

        //bool passedL1Calo   = match()->ancestorL1Passed(finalFC);
        bool passedL2Calo   = match()->ancestorPassed<xAOD::TrigEMClusterContainer>(obj);
        bool passedL2       = match()->ancestorPassed<xAOD::TrigElectronContainer>(obj);
        bool passedEFCalo   = match()->ancestorPassed<xAOD::CaloClusterContainer>(obj);
        bool passedEF       = match()->ancestorPassed<xAOD::ElectronContainer>(obj);

        //ATH_MSG_INFO( "Passed L1Calo  : "<< passedL1Calo );
        ATH_MSG_INFO( "Passed L2Calo  : "<< passedL2Calo );
        ATH_MSG_INFO( "Passed L2      : "<< passedL2     );
        ATH_MSG_INFO( "Passed EFCalo  : "<< passedEFCalo );
        ATH_MSG_INFO( "Passed EF      : "<< passedEF     );


      }

    }
    else ATH_MSG_DEBUG("REGTEST: eg pointer null!");
  }


  //**********************************************************************

  StatusCode TrigEgammaMatchingToolTest::execute() {   
      
    ATH_MSG_INFO ("Executing " << name() << "...");

      const xAOD::ElectronContainer *offElectrons = 0;
      if ( (m_storeGate->retrieve(offElectrons,"Electrons")).isFailure() ){
          ATH_MSG_DEBUG("Failed to retrieve offline Electrons ");
      }


      if(offElectrons) ATH_MSG_INFO("Offline Electron container size " << offElectrons->size());
      
      for(const auto &trigger : m_triggerList){
          if(!offElectrons) continue;
          for(const auto& eg : *offElectrons){
              ATH_MSG_DEBUG("REGTEST:: Electron offline (eta="<<eg->eta()<<",phi="<<eg->phi()<<")");
              inspect(trigger,eg);
          } //End loop of offline electrons
      } // End loop over trigger list
      


      return StatusCode::SUCCESS;
  } // End execute
} //End namespace Trig
//**********************************************************************






