// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXSTOREGATEEXAMPLE_READPILEUPDATA_H
#define ATHEXSTOREGATEEXAMPLE_READDPILEUPATA_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
class PileUpMergeSvc;

/////////////////////////////////////////////////////////////////////////////

class ReadPileUpData:public AthAlgorithm {
public:
  ReadPileUpData (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  PileUpMergeSvc* p_mergeSvc;
};


#endif // not ATHEXSTOREGATEEXAMPLE_READDPILEUPATA_H







