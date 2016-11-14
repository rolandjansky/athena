/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetTileCorrection includes
#include "JetTileCorrectionAlg.h"
#include "JetTileCorrection/JetTileCorrectionTool.h"

// EDM include(s):
#include "xAODJet/JetContainer.h"

#include "PathResolver/PathResolver.h"
#include "AsgTools/ToolStore.h"

// Global accessors and decorators
static SG::AuxElement::Accessor<unsigned int> acc_tileok("TileStatus");
static SG::AuxElement::Accessor<float> acc_ptraw("Ptraw");

using namespace CP;

JetTileCorrectionAlg::JetTileCorrectionAlg( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAnalysisAlgorithm( name, pSvcLocator )
  , m_jtcTool( "CP::JetTileCorrectionTool/JetTileCorrectionTool", this )
{
  
  declareProperty( "JetContainer", m_jetKey = "AntiKt4EMTopoJets" );
  declareProperty( "JetTileCorrectionTool",   m_jtcTool      );  
}


JetTileCorrectionAlg::~JetTileCorrectionAlg() {

}


StatusCode JetTileCorrectionAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode JetTileCorrectionAlg::firstExecute() {
  ATH_MSG_INFO ("First execute " << name() << "...");

  ATH_CHECK( m_jtcTool.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode JetTileCorrectionAlg::finalize() {

  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode JetTileCorrectionAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  
  // Retrieve the jets:
  const xAOD::JetContainer* jets = 0;
  ATH_CHECK( evtStore()->retrieve( jets, m_jetKey ) );

  ATH_MSG_INFO("--------------------");
  ATH_MSG_INFO( "Number of jets: " << jets->size() );
  
  // Loop over them:
  xAOD::JetContainer::const_iterator jet_itr = jets->begin();
  xAOD::JetContainer::const_iterator jet_end = jets->end();
  for( ; jet_itr != jet_end; ++jet_itr ){
    
    //if ((*jet_itr)->pt() < 20000. || fabs((*jet_itr)->eta()) > 2.8) continue;
    
    // copy constant objects to non-constant
    xAOD::Jet* jet = 0;                                                                                                                                                           
    jet = new xAOD::Jet();                                                                                                                                                        
    jet->makePrivateStore( **jet_itr );
        
    //--- apply tile dead module correction
    CP::CorrectionCode retCode = m_jtcTool->applyCorrection(*jet);

    if ( retCode == CP::CorrectionCode::OutOfValidityRange ){
      ATH_MSG_VERBOSE("No valid pt/eta range. No correction applied.");
    }
    else if( retCode != CP::CorrectionCode::Ok ){
      ATH_MSG_ERROR("Failed to apply JetTileCorrection!");
      //return StatusCode::FAILURE;
    }

    unsigned int j_status = acc_tileok(*jet);
    std::string str_status="";
    
    if(j_status == (unsigned int)JTC::TS::GOOD)
      str_status = "NotAffected";
    else if(j_status == (unsigned int)JTC::TS::EDGE)
      str_status = "EdgeAffected";
    else if(j_status == (unsigned int)JTC::TS::CORE)
      str_status = "CoreAffected";
    else
      str_status = "Unknown";

    ATH_MSG_INFO("Jet status : " << str_status << ", Pt raw = " << acc_ptraw(*jet)*0.001 << " GeV, Pt corrected = " << jet->pt()*0.001 << " GeV");
  }
  ATH_MSG_INFO("--------------------\n");
  
  return StatusCode::SUCCESS;
}

  
  

