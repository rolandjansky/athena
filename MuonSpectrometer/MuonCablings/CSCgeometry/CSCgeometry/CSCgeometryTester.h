/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGS_CSCGEOMETRYTESTER_H
#define MUONCABLINGS_CSCGEOMETRYTESTER_H

#include "GaudiKernel/Algorithm.h"

class CSCgeometrySvc;

class CSCgeometryTester : public Algorithm
{
 public:
  CSCgeometryTester(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  CSCgeometrySvc* m_pCSCgeometrySvc;
};

#endif //MUONCABLINGS_CSCGEOMETRYTESTER_H

