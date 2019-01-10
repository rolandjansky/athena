/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// -*- mode: c++ -*-

/** 
@file TestTools/InitGaudiGoogleTest.h
@author S. Kluth
@date July 2016
@brief Gaudi initialisation fixture base class for googletest based unit tests.
Test fixture classes for tests which need a fully configured Gaudi should inherit
from this class, and then provide further Gaudi configuration in their SetUp()
method if needed.
*/

#ifndef GOOGLETESTTOOLS_INITGAUDIGOOGLETEST_H
#define GOOGLETESTTOOLS_INITGAUDIGOOGLETEST_H

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ISvcManager.h"

#include "gtest/gtest.h"

class IAppMgrUI;

namespace Athena_test {

  class InitGaudiGoogleTest : public ::testing::Test {
  public:

    /**
    @brief Ctor parameter: 
    @param level controls MessageSvc
    */

    InitGaudiGoogleTest( MSG::Level level=MSG::ALWAYS );

    /** @brief dtor */

    virtual ~InitGaudiGoogleTest();

    /** @brief public members are visible in tests */

    IAppMgrUI* theApp;
    SmartIF<IProperty> propMgr;
    SmartIF<ISvcLocator> svcLoc;
    SmartIF<ISvcManager> svcMgr;

  private:

    MSG::Level m_msgLevel;
 
 };

}

#endif
