// EventDensityTool.h    -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EVENTSHAPETOOLS_EVENTSHAPECOPIER_H
#define EVENTSHAPETOOLS_EVENTSHAPECOPIER_H

//////////////////////////////////////////////////////
/// \class EventShapeCopier
/// \author P-A Delsart
/// 
/// A dual-use tool to copy EventShape objects
///
/// The intent of this tool is primarily to rename EventShape objects produced
/// in the very first DxAOD files before the final naming scheme was chosen.
/// 
/// In RootCore analysis : the easiest is to simply call the static "renameEventDensities()" 
/// function before attempting to access the "Kt4/6LCTopoEventShape" objects.
///
/// In Athena : schedule an athena alg by :
///     from EventShapeTools.EventDensityConfig import configEventShapeCopierAlg
///     topSequence += configEventShapeCopierAlg("LCTopo")
///
///////////////////////////////////////////

#include "AsgTools/AsgTool.h"
#include "EventShapeInterface/IEventShapeTool.h"

class EventShapeCopier :
  public asg::AsgTool,  
  virtual public ::IEventShapeTool { 
  ASG_TOOL_CLASS(EventShapeCopier, IEventShapeTool)

public: 

  /// Constructor with parameters: 
  EventShapeCopier( const std::string& name);

  /// Destructor: 
  ~EventShapeCopier(); 

  /// Initialization.
  StatusCode  initialize();

  /// Action.
  StatusCode fillEventShape() const;
  StatusCode fillEventShape(xAOD::EventShape* es) const;
  StatusCode fillEventShape(xAOD::EventShape* es, const xAOD::IParticleContainer* input) const;

  /// This function is only a convenience function for use in Standalone Analysis
  /// it avoids the need of configuring the tool by copying pre-determined EventShape
  /// written with old convention in early D3PD
  /// IMPORTANT : This function does nothing in Athena. Instead use the configEventShapeCopierAlg("LCTopo") function
  /// to schedule an Athena algorithm (see python/EventDensityConfig).
  void renameEventDensities();
protected:
  // DataHandles
  SG::ReadHandleKey<xAOD::EventShape>       m_outputEventShapeIn;
  SG::ReadHandleKey<xAOD::EventShape>       m_inputEventShape;
  SG::WriteHandleKey<xAOD::EventShape>      m_outputEventShape;
  
  //std::string m_inputEventShape;
  //std::string m_outputEventShape;
  std::string m_eventDensityName;

}; 

#endif
