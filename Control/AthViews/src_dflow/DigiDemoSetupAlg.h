/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_ATHVIEWS_DIGIDEMOSETUPALG_H
#define ATHVIEWS_ATHVIEWS_DIGIDEMOSETUPALG_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthViews/View.h"
#include "StoreGate/WriteHandleKey.h"
#include "GaudiKernel/IScheduler.h"

namespace AthViews {


class DigiDemoSetupAlg
  : public ::AthAlgorithm
{ 
 public: 
  /// Constructor with parameters: 
  DigiDemoSetupAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DigiDemoSetupAlg(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

  /// Default constructor: 
  DigiDemoSetupAlg();

  /// Containers
  
  // vars
  ServiceHandle< IScheduler > m_scheduler { this, "Scheduler", "AvalancheSchedulerSvc", "The Athena scheduler" };
  ServiceHandle< IProxyDict > m_digiStore { this, "DigiStore", "digi_store", "The store for pileup events" };
  SG::WriteHandleKey< ViewContainer > m_w_views { this, "AllViews", "all_views", "All views" };
  SG::WriteHandleKey< std::vector< int > > m_w_ints { this, "DigiData", "digi_data", "Something to store for each pileup event" };
  Gaudi::Property< std::string > m_viewBaseName { this, "ViewBaseName", "", "Name to use for all views - number will be appended" };
  Gaudi::Property< std::string > m_viewNodeName { this, "ViewNodeName", "", "Name of CF node to attach views to" };
  Gaudi::Property< int > m_viewNumber { this, "ViewNumber", 0, "Total number of views to make" };
}; 


} //> end namespace AthViews

#endif //> !ATHVIEWS_ATHVIEWS_DIGIDEMOSETUPALG_H
