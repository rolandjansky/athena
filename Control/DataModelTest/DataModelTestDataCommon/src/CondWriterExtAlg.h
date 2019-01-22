/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DATAMODELTESTDATACOMMON_CONDWRITEREXTALG_H
#define DATAMODELTESTDATACOMMON_CONDWRITEREXTALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IIOVSvc.h"

#include <string>
#include <map>

namespace DMTest {

/**
 * @class CondWriterExtAlg
 * @brief Alg to write conditions data via an external script
 **/
class CondWriterExtAlg : public AthAlgorithm {
public:
  CondWriterExtAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:
  Gaudi::Property<std::string> m_attrListKey{this, "AttrListKey", "/DMTest/TestAttrList"};
  Gaudi::Property<std::map<int,std::string>> m_cmd{this, "Commands", {}, "Commands to be executed on LB change"};

  ServiceHandle<IIOVSvc> m_iovSvc;
  ServiceHandle<IIOVDbSvc> m_iovDbSvc;
};

} // namespace DMTest

#endif // DATAMODELTESTDATACOMMON_CONDWRITEREXTALG_H
