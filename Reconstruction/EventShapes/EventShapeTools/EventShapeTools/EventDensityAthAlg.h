///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include <AnaAlgorithm/AnaReentrantAlgorithm.h>

#include "EventShapeInterface/IEventShapeTool.h"
#include "AsgTools/ToolHandle.h"


class EventDensityAthAlg : public EL::AnaReentrantAlgorithm { 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Can't use "using ctor" because of incompatiblity with pyroot in AnalysisBase
  EventDensityAthAlg(const std::string & n, ISvcLocator* l) : EL::AnaReentrantAlgorithm(n,l) {}

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext&) const override;
  virtual StatusCode  finalize() override;

 private:   
  ToolHandle<IEventShapeTool> m_densityTool{this,"EventDensityTool",""};
}; 

#endif //> !EVENTSHAPETOOLS_EVENTDENSITYATHALG_H
