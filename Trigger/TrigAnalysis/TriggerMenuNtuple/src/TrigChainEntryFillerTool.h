/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigChainEntryFillerTool_h__
#define __TrigChainEntryFillerTool_h__
/*
  TrigChainEntryFillerTool.h
*/
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

  class TrigChainEntryFillerTool : public BlockFillerTool<ChainEntry> {
  public:
    TrigChainEntryFillerTool(const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent);

    StatusCode book();
    StatusCode fill(const ChainEntry& x);

  private:
    int* mChainCounter;
    std::vector<int>* mRoITypes;
    std::vector<int>* mRoIIndices;
  };

}

#endif // __TrigChainEntryFillerTool_h__
