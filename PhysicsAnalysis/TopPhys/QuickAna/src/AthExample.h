/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICKANA_ATHEXAMPLE_H
#define QUICKANA_ATHEXAMPLE_H 1

#include "AthenaBaseComps/AthAlgorithm.h"

#include <GaudiKernel/ToolHandle.h>
#include <QuickAna/Global.h>

namespace ana
{

class AthExample: public ::AthAlgorithm {
 public:
  AthExample( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~AthExample();

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  ToolHandle<IQuickAna> quickAna;

 private:

};
}

#endif //> !QUICKANA_ATHEXAMPLEQA_H
