/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//
#ifndef ALIGNCONDATHTEST
#define ALIGNCONDATHTEST

#include "GaudiKernel/Algorithm.h"

#include "MuonCondSvc/MuonAlignmentDbSvc.h"
/////////////////////////////////////////////////////////////////////////////

class AlignCondAthTest:public Algorithm {
public:
  AlignCondAthTest (const std::string& name, ISvcLocator* pSvcLocator);

public:
///////////////////////////////////

   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
///////////////////////////////////

   MuonCalib::MuonAlignmentDbSvc* p_MuonAlignmentDbSvc; 
};

#endif

