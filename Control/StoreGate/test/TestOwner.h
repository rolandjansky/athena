// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file StoreGate/test/TestOwner.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Helper for testing auto-declaring constructors.
 *
 * This is a class that can be used as the owner of
 * an auto-declaring constructor for the purpose of unit tests.
 */


#ifndef STOREGATE_TESTOWNER_H
#define STOREGATE_TESTOWNER_H


#include "StoreGate/VarHandleKeyProperty.h"
#include "GaudiKernel/PropertyHolder.h"
#include "GaudiKernel/IProperty.h"


class TestOwnerBase
  : virtual public IProperty, virtual public INamedInterface
{
public:
  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface( const InterfaceID& /*ti*/, void** /*pp*/ ) override
  { std::abort(); }
  virtual const std::string& name() const override { std::abort(); }

};
class TestOwner
  : public PropertyHolder<TestOwnerBase>
{
};


#endif // not STOREGATE_TESTOWNER_H
