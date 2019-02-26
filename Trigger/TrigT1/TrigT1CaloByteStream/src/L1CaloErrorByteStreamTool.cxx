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


StatusCode L1CaloErrorByteStreamTool::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

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
  if (err && m_robMap.find(robid) == m_robMap.end()) {
    m_robMap.insert(std::make_pair(robid, err));
  }
  return;
}

// Set ROD unpacking error

void L1CaloErrorByteStreamTool::rodError(const uint32_t robid,
                                         const unsigned int err)
{
  if (err && m_rodMap.find(robid) == m_rodMap.end()) {
    m_rodMap.insert(std::make_pair(robid, err));
  }
  return;
}

// Fill vector with accumulated errors and reset

StatusCode L1CaloErrorByteStreamTool::errors(std::vector<unsigned int>*
                                                                 const errColl)
{
  if (!m_robMap.empty() || !m_rodMap.empty()) {
    errColl->push_back(m_robMap.size());
    ErrorMap::const_iterator iter  = m_robMap.begin();
    ErrorMap::const_iterator iterE = m_robMap.end();
    for (; iter != iterE; ++iter) {
      errColl->push_back(iter->first);
      errColl->push_back(iter->second);
    }
    m_robMap.clear();
    iter  = m_rodMap.begin();
    iterE = m_rodMap.end();
    for (; iter != iterE; ++iter) {
      errColl->push_back(iter->first);
      errColl->push_back(iter->second);
    }
    m_rodMap.clear();
  }
  return StatusCode::SUCCESS;
}

} // end namespace
