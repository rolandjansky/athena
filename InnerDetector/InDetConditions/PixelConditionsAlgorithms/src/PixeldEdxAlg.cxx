/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixeldEdxAlg.h"
#include "GaudiKernel/EventIDRange.h"

#include "PathResolver/PathResolver.h"

#include <cstdint>
#include <istream>
#include <fstream>

PixeldEdxAlg::PixeldEdxAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthAlgorithm(name, pSvcLocator),
  m_pixelID(nullptr) { }

StatusCode PixeldEdxAlg::initialize() {
  ATH_MSG_DEBUG("PixeldEdxAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKey.initialize(m_readfromcool));
  ATH_CHECK(m_writeKey.initialize());
  if (m_readfromcool) {
    if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode PixeldEdxAlg::execute() {
  ATH_MSG_DEBUG("PixeldEdxAlg::execute()");

  SG::WriteCondHandle<PixeldEdxData> writeHandle(m_writeKey);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixeldEdxData> writeCdo(std::make_unique<PixeldEdxData>());

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
//  const EventIDRange rangeW{start, stop};
  EventIDRange rangeW{start, stop};

  int         fit_type;
  std::string fun_type;
  std::string bb_type;

  if (m_readfromcool) {
    SG::ReadCondHandle<AthenaAttributeList> readHandle(m_readKey);
    const AthenaAttributeList* readCdo = *readHandle; 
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range
    ATH_MSG_DEBUG("Size of AthenaAttributeList " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());

    // Get the validitiy range
    if (not readHandle.range(rangeW)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_DEBUG("Range of input is " << rangeW);

    // Get constants from string in DB.
    std::string mystring=(*readCdo)["data"].data<std::string>();
    std::istringstream mystringstream(mystring);

    if (mystringstream.eof() || mystringstream.fail()) {
      ATH_MSG_ERROR("Failed to read DB contents. Check DB.");
      return StatusCode::FAILURE;
    }

    mystringstream >> fit_type;
    mystringstream >> fun_type;
    mystringstream >> bb_type;

    writeCdo -> setFunctionType(fun_type);
    writeCdo -> setBetheBlochType(bb_type);
    writeCdo -> setMinimumdEdxForMass(m_mindedxformass);

    if (fit_type==0) {
      for (int i=0;i<5;i++) {
        for (int j=0;j<9;j++) {
          double param;
          mystringstream >> param;
          writeCdo -> setPar(i,param);
        }
      }
      writeCdo -> setPosNeg(false);
    } 
    else {
      for (int i=0;i<2*5;i++) {
        for (int j=0;j<9;j++) {
          double param;
          mystringstream >> param;
          writeCdo -> setPar(i,param);
        }
      }
      writeCdo -> setPosNeg(true);
    }
  }
  else {
    std::string file_name = PathResolver::find_file(m_filename,"DATAPATH");
    if (file_name.size()==0) {
      ATH_MSG_ERROR("PixeldEdx input file " << m_filename << " not found.");
      return StatusCode::FAILURE;
    }
    std::ifstream input(file_name.c_str());
    if (!input.good()) {
      ATH_MSG_ERROR("Cannot open " << file_name);
      return StatusCode::FAILURE;
    }
    input >> fit_type;
    input >> fun_type;
    input >> bb_type;

    writeCdo -> setFunctionType(fun_type);
    writeCdo -> setBetheBlochType(bb_type);
    writeCdo -> setMinimumdEdxForMass(m_mindedxformass);

    if (fit_type==0) {
      for (int i=0;i<5;i++) {
        for (int j=0;j<9;j++) {
          double param;
          input >> param;
          writeCdo -> setPar(i,param);
        }
      }
      writeCdo -> setPosNeg(false);
    } 
    else {
      for (int i=0;i<2*5;i++) {
        for (int j=0;j<9;j++) {
          double param;
          input >> param;
          writeCdo -> setPar(i,param);
        }
      }
      writeCdo -> setPosNeg(true);
    }
    input.close();
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixeldEdxData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

