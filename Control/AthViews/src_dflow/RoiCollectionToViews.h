/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_ATHVIEWS_ROICOLLECTIONTOVIEWS_H
#define ATHVIEWS_ATHVIEWS_ROICOLLECTIONTOVIEWS_H

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthViews/View.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthContainers/ConstDataVector.h"

namespace AthViews {

class RoiCollectionToViews
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  RoiCollectionToViews( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~RoiCollectionToViews(); 

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
  RoiCollectionToViews();

  /// Containers
  
  // vars
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_trigRoIs;
  SG::WriteHandleKey< std::vector< SG::View* > > m_w_views;
  SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_viewRoIs;
  std::vector< std::string > m_algorithmNameSequence;
  std::string m_algPoolName;
  std::string m_viewBaseName;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews

#endif //> !ATHVIEWS_ATHVIEWS_ROICOLLECTIONTOVIEWS_H
