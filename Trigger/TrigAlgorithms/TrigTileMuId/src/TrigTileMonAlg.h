// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTILEMUID_TRIGTILEMONALG_H
#define TRIGTILEMUID_TRIGTILEMONALG_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "TrigInterfaces/FexAlgo.h"
#include <string>


namespace HLT {
  class TriggerElement;
}
class MonitorToolBase;

class TrigTileMonAlg : public HLT::FexAlgo {

 public:

  TrigTileMonAlg(std::string name, ISvcLocator * pSvcLocator);

  ~TrigTileMonAlg();

  HLT::ErrorCode hltInitialize();

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* /* inputTE */, HLT::TriggerElement* /* outputTE */);

  HLT::ErrorCode hltFinalize();

 private:

  MonitorToolBase *m_tool; 
};

#endif
