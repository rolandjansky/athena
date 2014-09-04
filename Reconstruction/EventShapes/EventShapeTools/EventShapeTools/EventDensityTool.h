///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventDensityTool.h 
// Header file for class EventDensityTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTSHAPETOOLS_EVENTDENSITYTOOL_H
#define EVENTSHAPETOOLS_EVENTDENSITYTOOL_H 1

// STL includes
#include <string>


// EventShapeInterface includes
#include "EventShapeInterface/IEventShapeTool.h"

#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"

#include "JetInterface/IPseudoJetGetter.h"
#include "JetInterface/IFastJetInterfaceTool.h"

//////////////////////////////////////////////////////
/// \class EventDensityTool
///
/// A dual-use tool to compute event density using FastJet.
///
/// THIS IS A PROTOTYPE
/// It has limitations :
///  - it can overwrite values in pre-existing EventShape objects
///  - it saves values under names independent of the eta range used for
///   the calculation ! 
/// 
///  These limitations must be solved before any serious usage.
///////////////////////////////////////////
class EventDensityTool :
  public asg::AsgTool,  
  virtual public ::IEventShapeTool
{ 
  ASG_TOOL_CLASS(EventDensityTool, IEventShapeTool)
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  EventDensityTool( const std::string& name);

  /// Destructor: 
  virtual ~EventDensityTool(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();


  virtual StatusCode fillEventShape() const ;
  virtual StatusCode fillEventShape(const xAOD::IParticleContainer*, const xAOD::EventShape *) const ;

   
protected:
  
  virtual StatusCode fillEventShape(const PseudoJetVector& , const xAOD::EventShape *) const ;
  
private: 


  ToolHandle<IPseudoJetGetter> m_pjGetter;
  ToolHandle<IFastJetInterfaceTool> m_fastJetTool;
  
  std::string m_evtShapeName;
  std::vector<float> m_etaRange;
  bool m_useAbsEta;
  bool m_useAreaFourMom;

  std::string m_varBaseName;
  //xAOD::EventShape::EventDensityID m_shapeId;
  
  

  // For now we use decorators, and test before to overwrite.
  SG::AuxElement::Decorator< float > m_rhoDec; 
  SG::AuxElement::Decorator< float > m_sigmaDec;
  SG::AuxElement::Decorator< float > m_areaDec;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !EVENTSHAPETOOLS_EVENTDENSITYTOOL_H
