/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RAW2DIGI_RAW2DIGI_H
#define RAW2DIGI_RAW2DIGI_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!


#include "AFP_Raw2Digi/IAFP_Raw2DigiTool.h"

#include <iostream>
#include <string>

class AFP_Raw2Digi : public ::AthAlgorithm {
public:
  AFP_Raw2Digi(const std::string &name, ISvcLocator *pSvcLocator);

  /// Does nothing
  virtual ~AFP_Raw2Digi();

  virtual StatusCode initialize();

  /// Executes commands in #m_digitool
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  ToolHandle<IAFP_Raw2DigiTool> m_DigiTool;
};

#endif //> !RAW2DIGI_RAW2DIGI_H
