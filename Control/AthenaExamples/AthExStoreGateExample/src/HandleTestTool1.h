// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthExStoreGateExample/src/HandleTestTool1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Test for cross-component circular dependency warning suppression
 *        of WriteDecorHandleKey.
 */


#ifndef ATHEXSTOREGATEEXAMPLE_HANDLETESTTOOL1_H
#define ATHEXSTOREGATEEXAMPLE_HANDLETESTTOOL1_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "AthExStoreGateExample/MyDataObj.h"
#include "StoreGate/WriteHandleKey.h"
#include "IHandleTestTool.h"


namespace AthEx {


class HandleTestTool1 : public extends<AthAlgTool, IHandleTestTool>
{
public:
  using base_class::base_class;


  virtual StatusCode initialize() override;


private:
  SG::WriteHandleKey<MyDataObj> m_whKey
  { this, "WriteKey", "obj", "" };
};


} // namespace AthEx


#endif // not ATHEXSTOREGATEEXAMPLE_HANDLETESTTOOL1_H
