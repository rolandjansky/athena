///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHVScaleRetriever.cxx 
// Implementation file for class LArHVScaleRetriever
// Author: P-A Delsart delsart at in2p3.fr
/////////////////////////////////////////////////////////////////// 

// LArTools includes
#include "LArHVScaleRetriever.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
LArHVScaleRetriever::LArHVScaleRetriever( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   )
  , m_ilarhvcorrtool("LArHVCorrToolDefault")
  , m_keyHVScaleCorr("LArHVScaleCorr")
  , m_isMC(false)
{
  declareInterface< ILArHVCorrTool >(this);
  
  declareProperty( "IsMC", m_isMC );
  declareProperty( "LArHVCorrTool", m_ilarhvcorrtool );
  declareProperty("KeyHVScaleCorr",m_keyHVScaleCorr);
}

// Destructor
///////////////
LArHVScaleRetriever::~LArHVScaleRetriever()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode LArHVScaleRetriever::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if( m_isMC ) {
    ATH_CHECK( m_keyHVScaleCorr.initialize() );
    m_ilarhvcorrtool.disable();
  }else {
    CHECK( m_ilarhvcorrtool.retrieve() ) ;  
  }
  
  return StatusCode::SUCCESS;
}


float LArHVScaleRetriever::Scale(const HWIdentifier& id)  const  {
  if(m_isMC) {
    SG::ReadCondHandle<ILArHVScaleCorr> corr (m_keyHVScaleCorr);
    return (*corr)->HVScaleCorr(id) ;
  }
  return m_ilarhvcorrtool->Scale(id);
}

float LArHVScaleRetriever::Scale(const Identifier& id)  const  {
  if(m_isMC) {
    SG::ReadCondHandle<ILArHVScaleCorr> corr (m_keyHVScaleCorr);
    return (*corr)->HVScaleCorr(id) ;
  }
  return m_ilarhvcorrtool->Scale(id);
}


StatusCode LArHVScaleRetriever::LoadCalibration(IOVSVC_CALLBACK_ARGS_P(i,k)) {
  if(!m_isMC) return m_ilarhvcorrtool->LoadCalibration(i,k) ;
  return StatusCode::FAILURE;
}

StatusCode LArHVScaleRetriever::record() {
  if(!m_isMC) return m_ilarhvcorrtool->record();
  return StatusCode::FAILURE;
}

bool LArHVScaleRetriever::updateOnLastCallback() const {
  if(!m_isMC) return m_ilarhvcorrtool->updateOnLastCallback();
  return false;
}
