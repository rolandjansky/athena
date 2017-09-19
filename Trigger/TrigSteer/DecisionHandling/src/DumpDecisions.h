/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DECISIONHANDLING_DUMPDECISIONS_H
#define DECISIONHANDLING_DUMPDECISIONS_H 1

// STL includes
#include <string>
#include "DecisionHandling/TrigCompositeUtils.h"
// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"



class DumpDecisions
  : public ::AthReentrantAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  DumpDecisions( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DumpDecisions(); 

  // Athena algorithm's Hooks
  StatusCode initialize() override;
  StatusCode execute_r( const EventContext& ctx ) const override;
  StatusCode finalize() override;
 private: 

  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionKey{ this, "Decisions", "Unspecified", "Input Decisions to dump" };
  Gaudi::Property<size_t> m_verbosityLevel{ "VerbosityLevel", 3, "3 - tries to print as much possible, 2 - only list of objects and their decisions, 1 - only list of active objets" };

  /// Default constructor: 
  DumpDecisions();


  

}; 


#endif //> !DECISIONHANDLING_DUMPDECISIONS_H
