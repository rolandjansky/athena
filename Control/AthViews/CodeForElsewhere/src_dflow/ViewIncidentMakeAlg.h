/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_ATHVIEWS_VIEWINCIDENTMAKEALG_H
#define ATHVIEWS_ATHVIEWS_VIEWINCIDENTMAKEALG_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthViews/AthViewAlgorithm.h"
#include "SGTools/BuiltinsClids.h"
#include "AthViews/View.h"
#include "StoreGate/WriteHandle.h"

namespace AthViews {

class ViewIncidentMakeAlg
  : public ::AthViewAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ViewIncidentMakeAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ViewIncidentMakeAlg(); 

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
  ViewIncidentMakeAlg();

  /// Containers
  
  // vars
  SG::WriteHandle< std::vector< SG::View* > > m_w_views;
  SG::WriteHandle<int> m_w_int;
  std::vector< std::string > m_viewNames;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews

#endif //> !ATHVIEWS_ATHVIEWS_VIEWINCIDENTMAKEALG_H
