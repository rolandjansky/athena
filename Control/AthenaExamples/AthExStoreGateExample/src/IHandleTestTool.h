// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthExStoreGateExample/src/IHandleTestTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Test for cross-component circular dependency warning suppression
 *        of WriteDecorHandleKey.
 */


#ifndef ATHEXSTOREGATEEXAMPLE_IHANDLETESTTOOL_H
#define ATHEXSTOREGATEEXAMPLE_IHANDLETESTTOOL_H


#include "GaudiKernel/IInterface.h"


namespace AthEx {


class IHandleTestTool
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID (IHandleTestTool, 1, 0);
};


} // namespace AthEx

#endif // not ATHEXSTOREGATEEXAMPLE_IHANDLETESTTOOL_H
