/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_ATHVIEWS_VIEWMERGEALG_H
#define ATHVIEWS_ATHVIEWS_VIEWMERGEALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthViews/AthViewAlgorithm.h"
#include "SGTools/BuiltinsClids.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthViews/View.h"

namespace AthViews {

class ViewMergeAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ViewMergeAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ViewMergeAlg(); 

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
  ViewMergeAlg();

  /// Containers
  
  // vars
  SG::WriteHandle< std::vector<int> > m_w_ints;
  SG::ReadHandle< std::vector<int> > m_r_ints;
  SG::ReadHandle< std::vector< SG::View* > > m_r_views;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews
#endif //> !ATHVIEWS_ATHVIEWS_VIEWMERGEALG_H
