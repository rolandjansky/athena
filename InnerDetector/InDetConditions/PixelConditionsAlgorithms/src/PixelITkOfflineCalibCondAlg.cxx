/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelITkOfflineCalibCondAlg.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/EventIDRange.h"
#include "PathResolver/PathResolver.h"
#include <memory>
#include <sstream>


PixelITkOfflineCalibCondAlg::PixelITkOfflineCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_readKey("ReadKey", "/PIXEL/PixReco"), // TO CHECK: is it the proper 21.9-friendly way?
  m_writeKey("WriteKey", "PixelITkOfflineCalibData"), // TO CHECK: is it the proper 21.9-friendly way?
  m_condSvc("CondSvc", name)
{
  declareProperty("InputSource",m_inputSource=2,"Source of data: 0 (none), 1 (text file), 2 (database)");
  declareProperty("PixelClusterErrorDataFile", m_textFileName="PixelITkClusterErrorData.txt","Read constants from this file");  
  declareProperty("DumpConstants", m_dump=0, "Dump constants to text file"); 
}



StatusCode PixelITkOfflineCalibCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelITkOfflineCalibCondAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  if (m_inputSource==2){
    if(m_readKey.key().empty()) {
     ATH_MSG_ERROR("The database is set to be input source (2) but the ReadKey is empty.");
    }
    ATH_CHECK(m_readKey.initialize());
  }

  ATH_CHECK(m_writeKey.initialize());
  
  // TO DO: Need to find the 21.9-friendly syntax

  /*if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
    }*/

  return StatusCode::SUCCESS;
}





StatusCode PixelITkOfflineCalibCondAlg::execute_r(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelITkOfflineCalibCondAlg::execute_r()");

  SG::WriteCondHandle<PixelCalib::PixelITkOfflineCalibData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }


  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelCalib::PixelITkOfflineCalibData> writeCdo(std::make_unique<PixelCalib::PixelITkOfflineCalibData>());

  if (m_inputSource==0) {
    ATH_MSG_WARNING("So far do nithing!! return StatusCode::FAILURE");
    return StatusCode::FAILURE;
  }
  else if (m_inputSource==1) {
    ATH_MSG_INFO("read from file");

    PixelCalib::PixelITkOfflineCalibData* calibData = new PixelCalib::PixelITkOfflineCalibData;
    
    PixelCalib::PixelITkClusterErrorData* pced = calibData->getPixelITkClusterErrorData();

    // Find and open the text file
    ATH_MSG_INFO("Load PixelITkErrorData constants from text file");
    std::string fileName = PathResolver::find_file(m_textFileName, "DATAPATH");
    if (fileName.size()==0) { ATH_MSG_WARNING("Input file " << fileName << " not found! Default (hardwired) values to be used!"); }
    else { pced->load(fileName);  }

    ATH_MSG_DEBUG("Get error constants");
    std::vector<float> constants = calibData->getConstants();
    if (constants.size()) { ATH_MSG_VERBOSE("constants are defined"); }
    else                  { ATH_MSG_ERROR("constants size is NULL!!!"); } 


    const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
    const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
    const EventIDRange rangeW{start, stop};

    ATH_MSG_DEBUG("Range of input is " << rangeW);
    
    if (constants.size()) {
      ATH_MSG_DEBUG("Found constants with new-style Identifier key");
      writeCdo->setConstants(constants);
    }

    // TO DO: Need to find the 21.9-friendly syntax

    /*if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record PixelCalib::PixelITkOfflineCalibData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
      return StatusCode::FAILURE;
      }*/

    ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

    if (m_dump!=0) {
      ATH_MSG_DEBUG("Dump the constants to file");
      calibData->dump();
    }
    delete calibData;

  }

  else if (m_inputSource==2) {

    //To be implemented

  }

  return StatusCode::SUCCESS;
}


StatusCode PixelITkOfflineCalibCondAlg::finalize() {
  ATH_MSG_DEBUG("PixelITkOfflineCalibCondAlg::finalize()");
  return StatusCode::SUCCESS;
}

