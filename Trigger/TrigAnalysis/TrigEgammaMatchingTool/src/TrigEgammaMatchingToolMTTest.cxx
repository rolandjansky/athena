/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaMatchingToolMTTest.h"
#include "DecisionHandling/TrigCompositeUtils.h"


using namespace TrigCompositeUtils;



TrigEgammaMatchingToolMTTest::TrigEgammaMatchingToolMTTest(const std::string& name,  ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm( name, pSvcLocator ),
  m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingToolMT",this)

{
  declareProperty("TrigEgammaMatchingToolMT",m_matchTool);
  declareProperty("TriggerList",m_triggerList);
}



//**********************************************************************

StatusCode TrigEgammaMatchingToolMTTest::initialize() {
    
  ATH_MSG_INFO("Initializing " << name() << "...");
  
  ATH_CHECK( m_trigdec.retrieve() );
  ATH_CHECK( m_matchTool.retrieve());
  ATH_CHECK(m_electronKey.initialize()); 
 
  
  
  return StatusCode::SUCCESS;
}



//**********************************************************************

void TrigEgammaMatchingToolMTTest::inspect(const std::string trigger,const xAOD::Egamma *eg) const {

  if (eg) {
    
    ATH_MSG_INFO("Getting all associated objects for " << trigger);
    
    const TrigCompositeUtils::Decision *dec=nullptr;
    
    m_matchTool->match( eg, trigger , dec);

    if ( dec )
    {
      ATH_MSG_INFO( "Matched!");


      auto l1_link = m_matchTool->getFeature<TrigRoiDescriptorCollection>(dec, trigger);
      auto emCluster_link = m_matchTool->getFeature<xAOD::TrigEMClusterContainer>( dec , trigger);
      auto trig_el_links = m_matchTool->getFeatures<xAOD::TrigElectronContainer>( dec, trigger );
      auto cl_links = m_matchTool->getFeatures<xAOD::CaloClusterContainer>( dec , trigger);
      auto el_links = m_matchTool->getFeatures<xAOD::ElectronContainer>( dec , trigger);

      if( l1_link.isValid() ){
        ATH_MSG_INFO( "We found the Roi object link" );
        // Let's get the EMTau
        auto l1 = m_matchTool->getL1Feature( l1_link.source );
        if(l1)
          ATH_MSG_INFO( "We found the EmTau object" );
        ATH_MSG_INFO( "L1 object state is assigned as " << (l1_link.state==ActiveState::ACTIVE ? "Active" : "Not active") );
      }

      if( emCluster_link.isValid() ){
        ATH_MSG_INFO( "We found the EmCluster object link" );
        ATH_MSG_INFO( "L2Calo object state is assigned as " << (emCluster_link.state==ActiveState::ACTIVE ? "Active" : "Not active") );
      }

      if (trig_el_links.size() > 0){
        ATH_MSG_INFO ( "We found " << trig_el_links.size() << " TrigElectron link objects for this decision souce." );
        for ( auto featLinkInfo : trig_el_links ){
          ATH_MSG_INFO( "L2Electron object state is assigned as " << (featLinkInfo.state==ActiveState::ACTIVE ? "Active" : "Not active") );
        }
      }

      if (cl_links.size() > 0){
        ATH_MSG_INFO ( "We found " << cl_links.size() << " CaloCluster link objects for this decision souce." );
        for ( auto featLinkInfo : cl_links ){
          ATH_MSG_INFO( "EFCalo object state is assigned as " << (featLinkInfo.state==ActiveState::ACTIVE ? "Active" : "Not active") );
        }
      }

      if (el_links.size() > 0){
        ATH_MSG_INFO ( "We found " << el_links.size() << " CaloCluster link objects for this decision souce." );
        for ( auto featLinkInfo : el_links ){
          ATH_MSG_INFO( "EFCalo object state is assigned as " << (featLinkInfo.state==ActiveState::ACTIVE ? "Active" : "Not active") );
        }
      }
      

      bool passedHLT    =  m_matchTool->ancestorPassed<xAOD::ElectronContainer> (dec, trigger , "HLT_egamma_Electrons");
      bool passedEFCalo =  m_matchTool->ancestorPassed<xAOD::CaloClusterContainer> (dec, trigger , "HLT_CaloEMClusters");
      bool passedL2     =  m_matchTool->ancestorPassed<xAOD::TrigElectronContainer> (dec, trigger , "HLT_FastElectrons");
      bool passedL2Calo =  m_matchTool->ancestorPassed<xAOD::TrigEMClusterContainer> (dec, trigger , "HLT_FastCaloEMClusters");
      bool passedL1Calo =  m_matchTool->ancestorPassed<TrigRoiDescriptorCollection> (dec, trigger , "initialRois");
      
      ATH_MSG_INFO( "L1Calo passed : "<< passedL1Calo   );
      ATH_MSG_INFO( "L2Calo passed : "<< passedL2Calo   );
      ATH_MSG_INFO( "L2     passed : "<< passedL2       );
      ATH_MSG_INFO( "EFCalo passed : "<< passedEFCalo   );
      ATH_MSG_INFO( "HLT    passed : "<< passedHLT      );

    }else{
      ATH_MSG_INFO( "Not Matched! There is no Dec object for this trigger " << trigger );
    }

  }
  else ATH_MSG_INFO("REGTEST: eg pointer null!");
}


//**********************************************************************

StatusCode TrigEgammaMatchingToolMTTest::fillHistograms( const EventContext& ctx ) const {   
    
    ATH_MSG_INFO ("Fill histograms " << name() << "...");


    SG::ReadHandle<xAOD::ElectronContainer> el_cont(m_electronKey, ctx);

    if( !el_cont.isValid() ){
        ATH_MSG_WARNING("Container "<< m_electronKey << " does not exist or is empty");
        return StatusCode::SUCCESS;
    }


    ATH_MSG_INFO("Offline Electron container size " << el_cont->size());
    
    for(const auto &trigger : m_triggerList){
        for(const auto& eg : *el_cont){
            ATH_MSG_INFO("REGTEST:: Electron offline (eta="<<eg->eta()<<",phi="<<eg->phi()<<")");
            inspect(trigger,eg);

        } //End loop of offline electrons
    } // End loop over trigger list
    


    return StatusCode::SUCCESS;
} // End execute






