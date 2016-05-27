///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg2_manualViews.h 
// Header file for class DFlowAlg2_manualViews
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHVIEWS_ATHVIEWS_DFLOWALG2_MANUALVIEWS_H
#define ATHVIEWS_ATHVIEWS_DFLOWALG2_MANUALVIEWS_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
//#include "StoreGate/UpdateHandle.h"
#include "AthViews/View.h"

namespace AthViews {

class DFlowAlg2_manualViews
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  DFlowAlg2_manualViews( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg2_manualViews(); 

  // Assignment operator: 
  //DFlowAlg2_manualViews &operator=(const DFlowAlg2_manualViews &alg); 

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
  DFlowAlg2_manualViews();

  /// Containers
  
  // vars
  SG::ReadHandle<int>  m_r_int;
  //SG::UpdateHandle<int> m_rw_int;
  SG::WriteHandle<std::vector<int> > m_ints;
  std::string m_viewName;
  SG::ReadHandle< std::vector< SG::View* > > m_r_views;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews
#endif //> !ATHVIEWS_ATHVIEWS_DFLOWALG2_MANUALVIEWS_H
