// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthExStoreGateExample/src/HandleTestAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Test for cross-component circular dependency warning suppression
 *        of WriteDecorHandleKey.
 */


#ifndef ATHEXSTOREGATEEXAMPLE_HANDLETESTALG_H
#define ATHEXSTOREGATEEXAMPLE_HANDLETESTALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "IHandleTestTool.h"


namespace AthEx {


class HandleTestAlg
  : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;


private:
  ToolHandle<IHandleTestTool> m_tool1
  { this, "Tool1", "AthEx::HandleTestTool1", "" };
  ToolHandle<IHandleTestTool> m_tool2
  { this, "Tool2", "AthEx::HandleTestTool2", "" };
};


} // namespace AthEx


#endif // not ATHEXSTOREGATEEXAMPLE_HANDLETESTALG_H
