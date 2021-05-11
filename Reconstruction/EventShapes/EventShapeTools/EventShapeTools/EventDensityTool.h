// EventDensityTool.h    -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EVENTSHAPETOOLS_EVENTDENSITYTOOL_H
#define EVENTSHAPETOOLS_EVENTDENSITYTOOL_H

//////////////////////////////////////////////////////
/// \class EventDensityTool
///
/// A dual-use tool to compute event density using FastJet.
///
/// Properties:
/// - JetAlgorithm: Kt, AntiKt or CamKt
/// - JetRadius: Jet size parameter, e.g. 0.4
/// - JetInput: Psedudojet getter for the inputs
/// - AbsRapidityMin: Minimum |y| for selecting jets.
/// - AbsRapidityMax: Maximum |y| for selecting jets.
/// - AreaDefinition: Voronoi, Active or ActiveFourVector
/// - VoronoiRfact - Multiplicative factor for Voronoi radius
/// - OutputContainer: Output container name
/// - VariableName - Name for the variable used to store rho
///
/// 
///////////////////////////////////////////

#include <string>
#include "fastjet/JetDefinition.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/Selector.hh"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "JetRec/PseudoJetContainer.h"
#include "EventShapeInterface/IEventShapeTool.h"
#include "xAODEventShape/EventShape.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteHandleKey.h"

// Temporarily included until trigger jet-finding properly moves to DH
// Unfortunately, depending on the trigger PJG interface introduces
// a circular dependency, so we have to depend on the basic class and
// rely on the member & property names to discourage use of other PJGs
#include "JetInterface/IPseudoJetGetter.h"

class EventDensityTool :
  public asg::AsgTool,  
  virtual public ::IEventShapeTool { 
  ASG_TOOL_CLASS(EventDensityTool, IEventShapeTool)

public: 

  /// Constructor with parameters: 
  EventDensityTool( const std::string& name);

  /// Destructor: 
  ~EventDensityTool(); 

  /// Initialization.
  StatusCode  initialize();

  /// Action.
  StatusCode fillEventShape() const;
  StatusCode fillEventShape(xAOD::EventShape* es) const;
   
protected:
  
  StatusCode fillEventShape(xAOD::EventShape *es , const PseudoJetVector& input  ) const ;
  
private: 
  // Temporarily included until trigger jet-finding properly moves to DH
  ToolHandle<IPseudoJetGetter> m_trigPJGet{""};

  // DataHandles
  SG::ReadHandleKey<PseudoJetContainer> m_inPJKey{this, "InputContainer", "", "ReadHandleKey for input PseudoJetVector"};
  SG::WriteHandleKey<xAOD::EventShape> m_outEDKey{this, "OutputContainer", "GenericEventDensity", "WriteHandleKey for output EventDensity"};

  // Properties
  std::string m_jetalg;                     // JetAlg
  float m_jetrad;                           // JetRadius
  float m_rapmin;                           // RapidityMax
  float m_rapmax;                           // RapidityMax
  std::string m_areadef;                    // AreaDefinition
  float m_vrfact;                           // VoronoiRfact

  // Derived data
  fastjet::JetDefinition m_fjjetdef;
  fastjet::AreaDefinition m_fjareadef;
  fastjet::Selector m_fjselector;
  bool m_useAreaFourMom;

}; 

#endif
