///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IEventShapeTool.h 
// Header file for class IEventShapeTool
// Author: P-A Delsart 
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTSHAPEINTERFACE_IEVENTSHAPETOOL_H
#define EVENTSHAPEINTERFACE_IEVENTSHAPETOOL_H 1


// FrameWork includes
#include "AsgTools/IAsgTool.h"

// Forward declaration
#include "xAODEventShape/EventShape.h"
#include "xAODBase/IParticleContainer.h"

class IEventShapeTool
  : virtual public asg::IAsgTool { 

  ASG_TOOL_INTERFACE(IPseudoJetGetter)

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IEventShapeTool(){};

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////


  /// Builds a default EventShape object, and records it in evt store
  virtual StatusCode fillEventShape() const = 0 ;

  /// Fills the given EventShape object
  virtual StatusCode fillEventShape(xAOD::EventShape *) const =0 ;

  /// Fills the given EventShape object using the given input IParticleContainer
  virtual StatusCode fillEventShape(xAOD::EventShape *, const xAOD::IParticleContainer*) const =0 ;
  

 protected: 

}; 




#endif //> !EVENTSHAPEINTERFACE_IEVENTSHAPETOOL_H
