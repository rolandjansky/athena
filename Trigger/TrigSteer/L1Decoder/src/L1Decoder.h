/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1Decoder_L1Decoder_h
#define L1Decoder_L1Decoder_h

#include "AthenaBaseComps/AthAlgorithm.h"

class L1Decoder : public AthAlgorithm {
public:
  L1Decoder(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  //  RHandle<> m_caloRoIs;
  // can demand objects 
};



#endif
