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
#include "GaudiKernel/IScheduler.h"

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
  ServiceHandle< IScheduler > m_scheduler{ this, "Scheduler", "AvalancheSchedulerSvc", "The Athena scheduler" };
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_trigRoIs { this, "InputRoICollection", "input_rois", "Collection of RoIs to split into views" };
  SG::WriteHandleKey< std::vector< SG::View* > > m_w_views { this, "AllViews", "all_views", "Output view collection" };
  SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_viewRoIs { this, "OutputRoICollection", "output_rois", "RoI collection to use inside views" };
  Gaudi::Property< std::string > m_viewBaseName { this, "ViewBaseName", "", "Name to use for all views - number will be appended" };
  Gaudi::Property< std::string > m_viewNodeName { this, "ViewNodeName", "", "Name of the CF node to attach views to" };
  Gaudi::Property< bool > m_viewFallThrough { this, "ViewFallThrough", true, "Set whether views may access StoreGate directly to retrieve data" };

}; 

/////////////////////////////////////////////////////////////////// 
// I/O operators
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews

#endif //> !ATHVIEWS_ATHVIEWS_ROICOLLECTIONTOVIEWS_H
