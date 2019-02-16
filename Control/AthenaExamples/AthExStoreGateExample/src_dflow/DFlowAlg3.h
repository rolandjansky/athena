///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg3.h 
// Header file for class DFlowAlg3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG3_H
#define ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG3_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

namespace AthEx {

class DFlowAlg3
  : public ::AthReentrantAlgorithm
{ 
public: 
  /// Constructor with parameters: 
  DFlowAlg3( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg3(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute (const EventContext& ctx) const override;
  virtual StatusCode  finalize() override;


private: 
  DFlowAlg3() = delete;

  SG::ReadHandleKey<int>  m_r_int
  { this, "RIntFlow", "dflow_int2", "" };
  SG::ReadHandleKey<std::vector<int> > m_r_ints
  { this, "RIntsFlow", "dflow_ints", "" };
  SG::WriteHandleKey<std::vector<int> > m_w_ints
  { this, "WIntsFlow", "dflow_ints2", "" };

}; 


} //> end namespace AthEx
#endif //> !ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG3_H
