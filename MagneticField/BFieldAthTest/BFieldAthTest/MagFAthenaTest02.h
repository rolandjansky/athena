/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  30/03/00
//
// The MagFAthenaTest02 class
// is an Algorithm class to test MagFieldAccess
//
#ifndef MAGFATHENATEST02
#define MAGFATHENATEST02

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "BFieldAth/MagFieldAthena.h"


class IMagFieldAthenaSvc;
class MagFAthenaTest02:public Algorithm {
public:
    MagFAthenaTest02(const std::string& name, ISvcLocator* pSvcLocator);

public:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
   void Dumpy02();

};

#endif

