// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthExStoreGateExample/src/HandleTestTool2.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Test for cross-component circular dependency warning suppression
 *        of WriteDecorHandleKey.
 */


#ifndef ATHEXSTOREGATEEXAMPLE_HANDLETESTTOOL2_H
#define ATHEXSTOREGATEEXAMPLE_HANDLETESTTOOL2_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "AthExStoreGateExample/MyDataObj.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "IHandleTestTool.h"


namespace AthEx {


class HandleTestTool2 : public extends<AthAlgTool, IHandleTestTool>
{
public:
  using base_class::base_class;


  virtual StatusCode initialize() override;


private:
  SG::WriteDecorHandleKey<MyDataObj> m_wdhKey
  { this, "DecorKey", "obj.decor", "" };
};


} // namespace AthEx


#endif // not ATHEXSTOREGATEEXAMPLE_HANDLETESTTOOL2_H
