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
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "EventShapeInterface/IEventShapeTool.h"
#include "xAODEventShape/EventShape.h"

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
  StatusCode fillEventShape(xAOD::EventShape* es, const xAOD::IParticleContainer* input) const;
   
protected:
  
  StatusCode fillEventShape(xAOD::EventShape *es , const PseudoJetVector& input  ) const ;
  
private: 
  // DataHandles
  SG::ReadHandleKey<xAOD::EventShape>       m_outconIn;
  SG::WriteHandleKey<xAOD::EventShape>      m_outcon;

  // Properties
  std::string m_jetalg;                     // JetAlg
  float m_jetrad;                           // JetRadius
  ToolHandle<IPseudoJetGetter> m_pjgetter;  // JetInput
  float m_rapmin;                           // RapidityMax
  float m_rapmax;                           // RapidityMax
  std::string m_areadef;                    // AreaDefinition
  float m_vrfact;                           // VoronoiRfact

  // Derived data
  fastjet::JetDefinition m_fjjetdef;
  fastjet::AreaDefinition m_fjareadef;
  fastjet::Selector m_fjselector;
  bool m_useAreaFourMom;

  // For now we use decorators, and test before to overwrite.
  SG::AuxElement::Accessor< float > m_rhoDec; 
  SG::AuxElement::Accessor< float > m_sigmaDec;
  SG::AuxElement::Accessor< float > m_areaDec;

}; 

#endif
