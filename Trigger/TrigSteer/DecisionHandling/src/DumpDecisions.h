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

  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionKey{ this, "Decisions", "Unspecified", "Input Decisions to dump" };
  Gaudi::Property<size_t> m_verbosityLevel{ "VerbosityLevel", 3, "3 - tries to print as much possible, 2 - only list of objects and their decisions, 1 - only list of active objects" };

};


#endif //> !DECISIONHANDLING_DUMPDECISIONS_H
