///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg1_manualViews.h 
// Header file for class DFlowAlg1_manualViews
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHVIEWS_ATHVIEWS_DFLOWALG1_MANUALVIEWS_H
#define ATHVIEWS_ATHVIEWS_DFLOWALG1_MANUALVIEWS_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "SGTools/BuiltinsClids.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthViews/View.h"

// eventinfo
#include "EventInfo/EventInfo.h"

namespace AthViews {

class DFlowAlg1_manualViews
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  DFlowAlg1_manualViews( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg1_manualViews(); 

  // Assignment operator: 
  //DFlowAlg1_manualViews &operator=(const DFlowAlg1_manualViews &alg); 

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
  DFlowAlg1_manualViews();

  /// Containers
  
  // vars
  SG::ReadHandle<int> m_r_int;
  SG::WriteHandle<int> m_w_int;
  std::string m_viewName;
  SG::ReadHandle< std::vector< SG::View* > > m_r_views;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews
#endif //> !ATHVIEWS_ATHVIEWS_DFLOWALG1_MANUALVIEWS_H
