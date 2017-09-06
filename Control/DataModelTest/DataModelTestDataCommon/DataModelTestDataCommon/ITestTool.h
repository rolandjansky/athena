// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file DataModelTestDataCommon/ITestTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Base class for testing SG access from tools.
 */


#ifndef DATAMODELTESTDATACOMMON_ITESTTOOL_H
#define DATAMODELTESTDATACOMMON_ITESTTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"


namespace DMTest {


class ITestTool : virtual public IAlgTool
{
public:
  DeclareInterfaceID (ITestTool, 1, 0);

  virtual StatusCode doit (const EventContext& ctx) const = 0;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATACOMMON_ITESTTOOL_H
