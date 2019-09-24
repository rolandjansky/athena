/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MYNEWPACKAGE_EXAMPLEALG_STEP1_H
#define MYNEWPACKAGE_EXAMPLEALG_STEP1_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

/** An example algorithm that reads and writes objects from the event store
 using handles.*/
class ExampleAlg_step1
  : public AthReentrantAlgorithm
{
public:
  ExampleAlg_step1 (const std::string& name, ISvcLocator* svcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute (const EventContext& ctx) const override;
private:
};
#endif
