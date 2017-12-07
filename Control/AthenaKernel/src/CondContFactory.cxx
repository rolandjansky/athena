/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */


#include "AthenaKernel/CondContFactory.h"
#include "AthenaKernel/CondCont.h"
#include "CxxUtils/checker_macros.h"
#include <sstream>
#include <stdexcept>

using namespace CondContainer;

CondContFactory& CondContFactory::Instance() {
  static CondContFactory factory ATLAS_THREAD_SAFE;
  return factory;
}

void CondContFactory::regMaker(const CLID& key, ICondContMaker* maker) {
  std::lock_guard<std::mutex> guard(m_mapMutex);
  if (m_makers.find(key) != m_makers.end()) {
    //    std::cerr << "multiple makers for CLID " << key << std::endl;
  } else {
    m_makers[key] = maker;
  }
}

SG::DataObjectSharedPtr<DataObject> CondContFactory::Create( const CLID& clid, const std::string& key ) const {
  std::lock_guard<std::mutex> guard(m_mapMutex);
  auto i = m_makers.find(clid);
  if ( i == m_makers.end()) {
    return SG::DataObjectSharedPtr<DataObject>();
  }
  ICondContMaker* maker = i->second;
  return maker->Create(clid,key);
}
