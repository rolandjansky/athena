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
  SG::WriteHandleKey< std::vector< SG::View* > > m_w_views;
  SG::WriteHandleKey<int> m_w_int;
  std::vector< std::string > m_algorithmNameSequence;
  std::string m_algPoolName;
  std::string m_viewBaseName;
  int m_viewNumber;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews

#endif //> !ATHVIEWS_ATHVIEWS_VIEWMAKEALG_H
