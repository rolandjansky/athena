///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// \class EventDensityAthAlg 
///   Algorithm which only calls the IEventShapeTool::fillEventShape() method of a tool it owns.
/// \author
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTSHAPETOOLS_EVENTDENSITYATHALG_H
#define EVENTSHAPETOOLS_EVENTDENSITYATHALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "EventShapeInterface/IEventShapeTool.h"
#include "AsgTools/ToolHandle.h"


class EventDensityAthAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  EventDensityAthAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~EventDensityAthAlg(); 


  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  
  /// Default constructor: 
  //  EventDensityAthAlg();
  
  /// Containers
  
  
  ToolHandle<IEventShapeTool> m_densityTool;  
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !EVENTSHAPETOOLS_EVENTDENSITYATHALG_H
