/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_ATHVIEWS_VIEWSUBGRAPHALG_H
#define ATHVIEWS_ATHVIEWS_VIEWSUBGRAPHALG_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthViews/View.h"
#include "StoreGate/WriteHandleKey.h"
#include "GaudiKernel/IScheduler.h"

namespace AthViews {

class ViewSubgraphAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ViewSubgraphAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ViewSubgraphAlg(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  ViewSubgraphAlg();

  /// Containers
  
  // vars
  ServiceHandle< IScheduler > m_scheduler { this, "Scheduler", "AvalancheSchedulerSvc", "The Athena scheduler" };
  SG::WriteHandleKey< std::vector< SG::View* > > m_w_views { this, "AllViews", "all_views", "All views" };
  Gaudi::Property< std::vector< std::string > > m_algorithmNameSequence { this, "AlgorithmNameSequence", std::vector< std::string >(), "Names of algorithms to run in the views (DEPRECATED)" };
  Gaudi::Property< std::string > m_algPoolName { this, "AlgPoolName", "", "Name for the algorithm pool service to use with the views (DEPRECATED)" };
  Gaudi::Property< std::string > m_viewBaseName { this, "ViewBaseName", "", "Name to use for all views - number will be appended" };
  Gaudi::Property< std::string > m_viewNodeName { this, "ViewNodeName", "", "Name of CF node to attach views to" };
  Gaudi::Property< int > m_viewNumber { this, "ViewNumber", 0, "Total number of views to make" };

  // Deliberately don't declare this handle correctly, to test the ViewDataVerifier
  //SG::WriteHandleKey< int > m_w_int { this, "ViewStart", "view_start", "A number to start off the view" };
  SG::WriteHandleKey< int > m_w_int;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews

#endif //> !ATHVIEWS_ATHVIEWS_VIEWMAKEALG_H
