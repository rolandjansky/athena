// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthExStoreGateExample/src/ExampleAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Example of using handles for data access.
 */


#ifndef ATHEXSTOREGATEEXAMPLE_EXAMPLEALG_H
#define ATHEXSTOREGATEEXAMPLE_EXAMPLEALG_H


#include "AthExStoreGateExample/MyDataObj.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


// An example algorithm that reads and writes objects from the event store
// using handles.
class ExampleAlg
  : public AthReentrantAlgorithm
{
public:
  ExampleAlg (const std::string& name, ISvcLocator* svcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;

private:
  // Declare the keys used to access the data: one for reading and one
  // for writing.
  SG::ReadHandleKey<MyDataObj> m_readKey;
  SG::WriteHandleKey<MyDataObj> m_writeKey;
};


#endif
