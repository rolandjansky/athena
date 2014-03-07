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
  Algorithm(name, pSvcLocator),
  m_log(0),
  m_detectorStore(0),
  m_onlineHelper(0)
{
}


LArSimpleShapeDumper::~LArSimpleShapeDumper() 
{
}


StatusCode LArSimpleShapeDumper::initialize()
{
  m_log = new MsgStream(msgSvc(), name());

  StatusCode sc = service("DetectorStore", m_detectorStore);
  if (sc.isFailure()) {
    (*m_log) << MSG::FATAL << "Unable to locate Service DetectorStore" << endreq;
    return sc;
  }
  
  sc = m_detectorStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure() || !m_onlineHelper) {
    (*m_log) << MSG::FATAL << "Could not get LArOnlineID helper !" << endreq;
    return sc;
  }
  
  sc = m_detectorStore->regHandle(m_shape, "LArShape");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Cannot get register handle for LArShape" << m_detectorStore->dump() << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS; 
}


StatusCode LArSimpleShapeDumper::execute()
{
  static bool first = true;
  if (first) {
    first = false;
    if (!m_shape || !m_shape.cptr()) {
      (*m_log) << MSG::FATAL << "Could not retrieve shape object, abort!" << endreq;
      return StatusCode::FAILURE;
    }
    const LArShapeComplete* shapeObj = dynamic_cast<const LArShapeComplete*>(m_shape.cptr());
    if (!shapeObj) {
      (*m_log) << MSG::FATAL << "Shape object is not of type LArShapeComplete ?!" << endreq;
      return StatusCode::FAILURE;
    }  
    for (unsigned int k = 0; k < 195073; k++) {
      HWIdentifier channelID = m_onlineHelper->channel_Id(IdentifierHash(k));
      for (int g = 0; g < 3; g++) { 
        CaloGain::CaloGain gain = (CaloGain::CaloGain)g;
        if (shapeObj->nTimeBins(channelID, gain) == 0) {
          (*m_log) << MSG::WARNING 
          << "Shape object for channel " << k 
          << " (" << (m_onlineHelper->barrel_ec(channelID) ? "EMEC" : "EMB")
          << "_" << (m_onlineHelper->pos_neg(channelID) ? "C" : "A")
          << " FT = " << m_onlineHelper->feedthrough(channelID)
          << ", slot = " <<  m_onlineHelper->slot(channelID)
          << ", channel = " <<  m_onlineHelper->channel(channelID)
          << ") and gain " << g << " has no shape data!" << endreq;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode LArSimpleShapeDumper::finalize()
{
  delete m_log;
  return StatusCode::SUCCESS;
}
