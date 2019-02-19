///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg1.h 
// Header file for class DFlowAlg1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG1_H
#define ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG1_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "SGTools/BuiltinsClids.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// eventinfo
#include "xAODEventInfo/EventInfo.h"

namespace AthEx {

class DFlowAlg1
  : public ::AthReentrantAlgorithm
{ 
public: 
  /// Constructor with parameters: 
  DFlowAlg1( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg1(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute(const EventContext& ctx) const override;
  virtual StatusCode  finalize() override;

 private: 
  DFlowAlg1() = delete;

  SG::ReadHandleKey<xAOD::EventInfo> m_r_evtInfo
  { this, "EvtInfo", "EventInfo", "" };
  SG::WriteHandleKey<int> m_w_int
  { this, "IntFlow", "dflow_int", "" };

}; 

} //> end namespace AthEx
#endif //> !ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG1_H
