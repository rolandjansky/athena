// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsTestAlg.h
 *
 * @brief Class to test SCT_ByteStreamErrorsTool (header)
 *
 * @author Susumu.Oda@cern.ch
 **/

#ifndef SCT_ByteStreamErrorsTestAlg_H
#define SCT_ByteStreamErrorsTestAlg_H 

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

class SCT_ByteStreamErrorsTestAlg : public AthReentrantAlgorithm {
 public:
  SCT_ByteStreamErrorsTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ByteStreamErrorsTestAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  ToolHandle<ISCT_ByteStreamErrorsTool> m_bsErrTool{this, "ByteStreamErrorsTool", "SCT_ByteStreamErrorsTool", "Tool to retrieve SCT bytestream errors"};
};

#endif // SCT_ByteStreamErrorsTestAlg_H
