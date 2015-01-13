/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//*************************************************************************
//*                                                                       *
//*  class  PileUpTruthExample                                            *
//*                                                                       *
//*  Uses PileUpType to returns iterators on the pileup type particles:
//*  - the  signal particles
//*  - the  in-time Minimum bias particles
//*  - the signal+in-time Minimum bias particles 
//*  - the out-of-time Minimum bias particles
//*  - the cavern background particles          
//*  - the Selector function MUST have the operator() implemented - for example: isGenStable
//*  
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
// Date: February 2008                                                    *
//*************************************************************************

#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;

class PileUpTruthExample : public AthAlgorithm {
public:
        PileUpTruthExample(const std::string& name, ISvcLocator* pSvcLocator);
        ~PileUpTruthExample(); 
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

private:
  // McEventCollection key 
  std::string m_key; 

};

