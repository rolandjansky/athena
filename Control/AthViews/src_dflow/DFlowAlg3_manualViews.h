///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg3_manualViews.h 
// Header file for class DFlowAlg3_manualViews
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHVIEWS_ATHVIEWS_DFLOWALG3_MANUALVIEWS_H
#define ATHVIEWS_ATHVIEWS_DFLOWALG3_MANUALVIEWS_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/UpdateHandle.h"
#include "AthViews/View.h"

namespace AthViews {

class DFlowAlg3_manualViews
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  DFlowAlg3_manualViews( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg3_manualViews(); 

  // Assignment operator: 
  //DFlowAlg3_manualViews &operator=(const DFlowAlg3_manualViews &alg); 

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
  DFlowAlg3_manualViews();

  /// Containers
  
  // vars
  SG::ReadHandle<int>  m_r_int;
  SG::ReadHandle<std::vector<int> > m_r_ints;
  //SG::UpdateHandle<std::vector<int> > m_rw_ints;
  std::string m_viewName;
  SG::ReadHandle< std::vector< SG::View* > > m_r_views;
  SG::WriteHandle<int> m_w_dflowDummy;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews
#endif //> !ATHVIEWS_ATHVIEWS_DFLOWALG3_MANUALVIEWS_H
