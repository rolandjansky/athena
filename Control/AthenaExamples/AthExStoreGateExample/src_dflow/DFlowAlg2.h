///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg2.h 
// Header file for class DFlowAlg2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG2_H
#define ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG2_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

namespace AthEx {

class DFlowAlg2
  : public ::AthReentrantAlgorithm
{ 
public: 
  /// Constructor with parameters: 
  DFlowAlg2( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg2(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute (const EventContext& ctx) const override;
  virtual StatusCode  finalize() override;

private: 
  DFlowAlg2() = delete;

  SG::ReadHandleKey<int>  m_r_int
  { this, "RIntFlow", "dflow_int", "" };
  SG::WriteHandleKey<int> m_w_int
  { this, "WIntFlow", "dflow_int2", "" };
  SG::WriteHandleKey<std::vector<int> > m_ints
  { this, "IntsFlow", "dflow_ints", "" };
}; 


} //> end namespace AthEx
#endif //> !ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG2_H
