/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelOfflineCalibCondAlg.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/EventIDRange.h"
#include "PathResolver/PathResolver.h"
#include <memory>
#include <sstream>

PixelOfflineCalibCondAlg::PixelOfflineCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_condSvc("CondSvc", name)
{
  declareProperty("InputSource",m_inputSource=2,"Source of data: 0 (none), 1 (text file), 2 (database)");
  declareProperty("PixelClusterErrorDataFile", m_textFileName1="PixelClusterErrorData.txt","Read constants from this file"); 
  declareProperty("PixelClusterOnTrackErrorDataFile", m_textFileName2="PixelClusterOnTrackErrorData.txt","Read constants from this file"); 
  declareProperty("PixelChargeInterpolationDataFile", m_textFileName3="PixelChargeInterpolationData.txt","Read constants from this file"); 
  declareProperty("DumpConstants", m_dump=0, "Dump constants to text file"); 
}

StatusCode PixelOfflineCalibCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelOfflineCalibCondAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKey.initialize());

  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelOfflineCalibCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelOfflineCalibCondAlg::execute()");

  SG::WriteCondHandle<PixelCalib::PixelOfflineCalibData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelCalib::PixelOfflineCalibData> writeCdo(std::make_unique<PixelCalib::PixelOfflineCalibData>());

  if (m_inputSource==0) {
    ATH_MSG_WARNING("So far do nithing!! return StatusCode::FAILURE");
    return StatusCode::FAILURE;
  }
  else if (m_inputSource==1) {
    ATH_MSG_INFO("read from file");

    PixelCalib::PixelOfflineCalibData* calibData = new PixelCalib::PixelOfflineCalibData;

    PixelCalib::PixelClusterErrorData* pced = calibData->getPixelClusterErrorData();
    PixelCalib::PixelChargeInterpolationParameters* pcip = calibData->getPixelChargeInterpolationParameters();
    PixelCalib::PixelClusterOnTrackErrorData* pcoted = calibData->getPixelClusterOnTrackErrorData();

    // Find and open the text file
    ATH_MSG_INFO("Load PixelErrorData constants from text file");
    std::string file_name1 = PathResolver::find_file(m_textFileName1, "DATAPATH");
    if (file_name1.size()==0) { ATH_MSG_WARNING("Input file " << file_name1 << " not found! Default (hardwired) values to be used!"); }
    else { pced->Load(file_name1);  }

    ATH_MSG_INFO("Load PixelClusterOnTrackErrorData constants from text file");
    std::string file_name2 = PathResolver::find_file(m_textFileName2, "DATAPATH");
    if (file_name2.size()==0) { ATH_MSG_WARNING("Input file " << file_name2 << " not found! Default (hardwired) values to be used!"); }
    else { pcoted->Load(file_name2); }

    ATH_MSG_INFO("Load PixelChargeInterpolationData constants from text file");
    std::string file_name3 = PathResolver::find_file(m_textFileName3, "DATAPATH");
    if (file_name3.size()==0) { ATH_MSG_WARNING("Input file " << file_name3 << " not found! Default (hardwired) values to be used!"); }
    else { pcip->Load(file_name3);  }

    // First constants are info on the number of bins of parametrizations
    ATH_MSG_DEBUG("Get error constants");
    std::vector<float> constants = calibData->GetConstants();
    if (constants.size()) { ATH_MSG_VERBOSE("constants are defined"); }
    else                  { ATH_MSG_ERROR("constants size is NULL!!!"); } 

    const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
    const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
    const EventIDRange rangeW{start, stop};

    ATH_MSG_DEBUG("Range of input is " << rangeW);

    if (constants.size()) {
      ATH_MSG_DEBUG("Found constants with new-style Identifier key");
      writeCdo->SetConstants(constants);
    }

    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record PixelCalib::PixelOfflineCalibData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

    if (m_dump!=0) {
      ATH_MSG_DEBUG("Dump the constants to file");
      calibData->Dump();
    }
    delete calibData;

  }
  else if (m_inputSource==2) {
    SG::ReadCondHandle<DetCondCFloat> readHandle{m_readKey, ctx};
    const DetCondCFloat* readCdo{*readHandle}; 
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range
    EventIDRange rangeW;
    if (not readHandle.range(rangeW)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Size of DetCondCFloat " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << rangeW);

    std::vector<float> constants;
    for (int i=0; i<readCdo->size(); i++) { constants.push_back(readCdo->get(Identifier(1),i)); }

    if (constants.size()>0) {
      ATH_MSG_DEBUG("Found constants with new-style Identifier key");
      writeCdo->SetConstants(constants);
    }
    else {
      Identifier key;
      key.set_literal(1);

      std::vector<float> const2;
      for (int i=0; i<readCdo->size(); i++) { const2.push_back(readCdo->get(key.set_literal(i+1),i)); }

      if (const2.size()>0) {
        ATH_MSG_DEBUG("Found constants with old-style Identifier key");
        writeCdo->SetConstants(const2);
      }
      else {
        ATH_MSG_ERROR("Could not get the constants!");
        return StatusCode::FAILURE;
      }
    }
    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record PixelCalib::PixelOfflineCalibData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelOfflineCalibCondAlg::finalize() {
  ATH_MSG_DEBUG("PixelOfflineCalibCondAlg::finalize()");
  return StatusCode::SUCCESS;
}

