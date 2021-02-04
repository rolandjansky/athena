/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_DUMPDECISIONS_H
#define DECISIONHANDLING_DUMPDECISIONS_H 1

// STL includes
#include <string>
#include "TrigCompositeUtils/TrigCompositeUtils.h"
// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

class DumpDecisions
  : public ::AthReentrantAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  /// Constructor with parameters:
  DumpDecisions( const std::string& name, ISvcLocator* pSvcLocator );

  // Athena algorithm's Hooks
  virtual StatusCode initialize() override;
  virtual StatusCode execute( const EventContext& ctx ) const override;

 private: 

  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_decisionKeys{ this, "Decisions", {}, "Input Decisions to dump" };

};


#endif //> !DECISIONHANDLING_DUMPDECISIONS_H
