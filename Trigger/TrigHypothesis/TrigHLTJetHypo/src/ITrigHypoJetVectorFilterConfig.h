/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_ITRIGHYPOJETVECTORFILTERCONFIG_H
#define TRIGHLTJETHYPO_ITRIGHYPOJETVECTORFILTERCONFIG_H

#include "GaudiKernel/IAlgTool.h"
#include "./IHypoJetVectorFilter.h"

using FilterPtr = std::unique_ptr<IHypoJetVectorFilter>;

class ITrigHypoJetVectorFilterConfig : virtual public ::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigHypoJetVectorFilterConfig, 1, 0);
  virtual ~ITrigHypoJetVectorFilterConfig(){};
  
  virtual FilterPtr getHypoJetVectorFilter() const = 0;
};
#endif
