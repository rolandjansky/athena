/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/LArSimpleShapeDumper.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "LArElecCalib/ILArShape.h"
#include "LArRawConditions/LArShapeComplete.h"


LArSimpleShapeDumper::LArSimpleShapeDumper(const std::string & name, ISvcLocator * pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_onlineHelper(0)
{
}


LArSimpleShapeDumper::~LArSimpleShapeDumper() 
{
}


StatusCode LArSimpleShapeDumper::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  ATH_CHECK( detStore()->regHandle(m_shape, "LArShape") );
  return StatusCode::SUCCESS; 
}


StatusCode LArSimpleShapeDumper::execute()
{
  static bool first = true;
  if (first) {
    first = false;
    if (!m_shape || !m_shape.cptr()) {
      ATH_MSG_FATAL ( "Could not retrieve shape object, abort!" );
      return StatusCode::FAILURE;
    }
    const LArShapeComplete* shapeObj = dynamic_cast<const LArShapeComplete*>(m_shape.cptr());
    if (!shapeObj) {
      ATH_MSG_FATAL ( "Shape object is not of type LArShapeComplete ?!" );
      return StatusCode::FAILURE;
    }  
    for (unsigned int k = 0; k < 195073; k++) {
      HWIdentifier channelID = m_onlineHelper->channel_Id(IdentifierHash(k));
      for (int g = 0; g < 3; g++) { 
        CaloGain::CaloGain gain = (CaloGain::CaloGain)g;
        if (shapeObj->nTimeBins(channelID, gain) == 0) {
          ATH_MSG_WARNING 
            ( "Shape object for channel " << k 
              << " (" << (m_onlineHelper->barrel_ec(channelID) ? "EMEC" : "EMB")
              << "_" << (m_onlineHelper->pos_neg(channelID) ? "C" : "A")
              << " FT = " << m_onlineHelper->feedthrough(channelID)
              << ", slot = " <<  m_onlineHelper->slot(channelID)
              << ", channel = " <<  m_onlineHelper->channel(channelID)
              << ") and gain " << g << " has no shape data!" );
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode LArSimpleShapeDumper::finalize()
{
  return StatusCode::SUCCESS;
}
