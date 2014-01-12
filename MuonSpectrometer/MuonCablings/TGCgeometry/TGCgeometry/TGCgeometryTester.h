/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGS_TGCGEOMETRYTESTER_H
#define MUONCABLINGS_TGCGEOMETRYTESTER_H

#include "GaudiKernel/Algorithm.h"

class TGCgeometrySvc;

class TGCgeometryTester : public Algorithm
{
public:
    TGCgeometryTester(const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:
    TGCgeometrySvc* m_pTGCgeometrySvc;
};

#endif //MUONCABLINGS_TGCGEOMETRYTESTER_H

