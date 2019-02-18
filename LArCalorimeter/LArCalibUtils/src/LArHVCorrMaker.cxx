/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "LArCalibUtils/LArHVCorrMaker.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArIdentifier/LArOnlineID.h"

#include <math.h>
#include <unistd.h>


LArHVCorrMaker::LArHVCorrMaker(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_lar_on_id(0),
    m_hvCorrTool("LArHVCorrTool",this)
{
  declareProperty("HVCorrTool", m_hvCorrTool);
}

//---------------------------------------------------------------------------
LArHVCorrMaker::~LArHVCorrMaker()
{}

//---------------------------------------------------------------------------
StatusCode LArHVCorrMaker::initialize()
{
  ATH_MSG_INFO ( "  in initialize " );
  ATH_CHECK( m_hvCorrTool.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_lar_on_id,"LArOnlineID") );
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArHVCorrMaker::execute()
{
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArHVCorrMaker::stop()
{
  ATH_MSG_INFO ( " in stop" );
  ATH_CHECK( m_hvCorrTool->record() );

  const ILArHVScaleCorr* scaletool = nullptr;
  ATH_CHECK( detStore()->retrieve(scaletool,"LArHVScaleCorr") );


  if (msgLvl(MSG::DEBUG)) { 
    // get HWIdentifier iterator
    std::vector<HWIdentifier>::const_iterator it  = m_lar_on_id->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e= m_lar_on_id->channel_end();
    // loop over Identifiers
    for(;it!=it_e;++it)
    {    
      const HWIdentifier id  = *it;  
      float scale = scaletool->HVScaleCorr(id);
      ATH_MSG_DEBUG (  m_lar_on_id->show_to_string(id) << " " << scale );
    }
  }

  return StatusCode::SUCCESS;
}
