/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <utility>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "L1CaloErrorByteStreamTool.h"

namespace LVL1BS {

// Interface ID

static const InterfaceID IID_IL1CaloErrorByteStreamTool(
                                           "L1CaloErrorByteStreamTool", 1, 1);

const InterfaceID& L1CaloErrorByteStreamTool::interfaceID()
{
  return IID_IL1CaloErrorByteStreamTool;
}

// Constructor

L1CaloErrorByteStreamTool::L1CaloErrorByteStreamTool(const std::string& type,
                                                     const std::string& name,
	    			                     const IInterface*  parent)
                          : AthAlgTool(type, name, parent)
{
  declareInterface<L1CaloErrorByteStreamTool>(this);
}

// Destructor

L1CaloErrorByteStreamTool::~L1CaloErrorByteStreamTool()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode L1CaloErrorByteStreamTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode L1CaloErrorByteStreamTool::finalize()
{
  return StatusCode::SUCCESS;
}

// Set ROB status error

void L1CaloErrorByteStreamTool::robError(const uint32_t robid,
                                         const unsigned int err)
{
  if (err && robMap.find(robid) == robMap.end()) {
    robMap.insert(std::make_pair(robid, err));
  }
  return;
}

// Set ROD unpacking error

void L1CaloErrorByteStreamTool::rodError(const uint32_t robid,
                                         const unsigned int err)
{
  if (err && rodMap.find(robid) == rodMap.end()) {
    rodMap.insert(std::make_pair(robid, err));
  }
  return;
}

// Fill vector with accumulated errors and reset

StatusCode L1CaloErrorByteStreamTool::errors(std::vector<unsigned int>*
                                                                 const errColl)
{
  if (!robMap.empty() || !rodMap.empty()) {
    errColl->push_back(robMap.size());
    ErrorMap::const_iterator iter  = robMap.begin();
    ErrorMap::const_iterator iterE = robMap.end();
    for (; iter != iterE; ++iter) {
      errColl->push_back(iter->first);
      errColl->push_back(iter->second);
    }
    robMap.clear();
    iter  = rodMap.begin();
    iterE = rodMap.end();
    for (; iter != iterE; ++iter) {
      errColl->push_back(iter->first);
      errColl->push_back(iter->second);
    }
    rodMap.clear();
  }
  return StatusCode::SUCCESS;
}

} // end namespace
