/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICallBackMuonRPC_Cabling_H
#define ICallBackMuonRPC_Cabling_H
#ifndef LVL1_STANDALONE	
#include "AthenaKernel/IOVSvcDefs.h"	
class ICallBackMuonRPC_Cabling : virtual public IInterface
{
  public:
  // Trigger roads initialization method
  virtual StatusCode initTrigRoadsModel(IOVSVC_CALLBACK_ARGS) = 0;
};
#endif
#endif  //ICallBackMuonRPC_Cabling_H
