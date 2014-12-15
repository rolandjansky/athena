/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TestElectronIsoAthena.cxx 635994 2014-12-15 14:37:46Z morrisj $

// Local include(s):
#include "TestElectronIsoAthena.h"

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"

// other include(s):
#include <cmath>

namespace CP 
{

  TestElectronIsoAthena::TestElectronIsoAthena( const std::string& name, ISvcLocator* svcLoc ) :
    AthAlgorithm( name, svcLoc ),
    m_sgKey("ElectronCollection"),
    m_isoTool(nullptr)
  {  
    declareProperty( "IsoTool", m_isoTool );
  }

  StatusCode TestElectronIsoAthena::initialize()
  {
    ATH_CHECK( m_isoTool.retrieve() );
    
    // Setup isolation in various different ways
    // Fixed 95% efficiency
    ATH_CHECK( m_isoTool->configureEfficiencyBasedIsolation( xAOD::Iso::etcone20 , "0.95" ) );
    ATH_CHECK( m_isoTool->configureEfficiencyBasedIsolation( xAOD::Iso::ptcone30 , "0.95" ) );
    
    // Exponential function
  //   ATH_CHECK( m_isoTool->configureEfficiencyBasedIsolation( xAOD::Iso::topoetcone20 , "(1.0-(TMath::Exp(-x/15.)))" );
    
    
    // Configure separate isolation conditions to study multijet backgrounds
    ATH_CHECK( m_isoTool->configureCutBasedIsolationBackground( xAOD::Iso::etcone20 , 6000.0 ) );
    ATH_CHECK( m_isoTool->configureCutBasedIsolationBackground( xAOD::Iso::ptcone30 , 6000.0 ) );
    
    
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }

  StatusCode TestElectronIsoAthena::execute()
  {
    const xAOD::ElectronContainer* xaod(nullptr);
    ATH_CHECK( evtStore()->retrieve( xaod , m_sgKey ) );
    ATH_MSG_INFO("-----------");
    
    for( auto x : *xaod ){
      // some basic selections
      if( x->pt() > 20000.0 ){
        float feta = fabs( x->caloCluster()->eta() );
        if( feta < 2.47 ){
          if( !(feta > 1.37 && feta < 1.52) ){      
            
            ATH_MSG_INFO(" Testing Electron with pT = "<<x->pt()<<" , cluster eta = "<<x->caloCluster()->eta() );
            
            ATH_MSG_INFO("  Testing analysis isolation conditions");
            // lets test the isolation
            if( m_isoTool->accept( *x ) ){
              ATH_MSG_INFO("   --> passes isolation" );
            }
            else{
              ATH_MSG_INFO("   --> fails isolation" );
            }
            
            std::shared_ptr<std::map<TString,float>> cutValues = m_isoTool->getCutValues( *x );
            for( std::map<TString,float>::const_iterator i=cutValues->begin();i!=cutValues->end();++i ){
              ATH_MSG_INFO("    Cut for "<<(*i).first<<" is <= "<<(*i).second <<" GeV");
            }
            
            const Root::TAccept& acceptDecision = m_isoTool->getTAccept();
            for(unsigned int a=0;a<acceptDecision.getNCuts();++a){
              ATH_MSG_INFO("    Cut decision for "<<acceptDecision.getCutName(a)<<" is "<<acceptDecision.getCutResult(a));
            }
            
            
            ATH_MSG_INFO("  Testing background isolation conditions");
            // lets test the isolation
            if( m_isoTool->acceptBackground( *x ) ){
              ATH_MSG_INFO("   --> passes background isolation" );
            }
            else{
              ATH_MSG_INFO("   --> fails background isolation" );
            }
            
            std::shared_ptr<std::map<TString,float>> cutValuesBackground = m_isoTool->getCutValuesBackground( *x );
            for( std::map<TString,float>::const_iterator i=cutValuesBackground->begin();i!=cutValuesBackground->end();++i ){
              ATH_MSG_INFO("    Background cut for "<<(*i).first<<" is <= "<<(*i).second <<" GeV");
            } 
            
            const Root::TAccept& acceptDecisionBackground = m_isoTool->getTAcceptBackground();
            for(unsigned int a=0;a<acceptDecisionBackground.getNCuts();++a){
              ATH_MSG_INFO("    Background cut decision for "<<acceptDecisionBackground.getCutName(a)<<" is "<<acceptDecisionBackground.getCutResult(a));
            }          
            
            
          }
        }
      }
    }
    
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }
}
