/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventShapeCopier.cxx 

#include "EventShapeTools/EventShapeCopier.h"

#include "xAODEventShape/EventShape.h"
#include "xAODEventShape/EventShapeAuxInfo.h"

//**********************************************************************

EventShapeCopier::EventShapeCopier(const std::string& name)
: asg::AsgTool(name),
  m_inputEventShape("LCTopoEventShape"), m_outputEventShape(""), m_eventDensityName("") {
  declareProperty("InputEventShape",    m_inputEventShape  );
  declareProperty("OutputEventShape",    m_outputEventShape  );
  declareProperty("EventDensityName",    m_eventDensityName  );
  
}

//**********************************************************************

EventShapeCopier::~EventShapeCopier() {}

//**********************************************************************

StatusCode EventShapeCopier::initialize() {

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode EventShapeCopier::fillEventShape() const {
  
  ATH_MSG_DEBUG("Begin fillEventShape()");

  if ( evtStore()->contains<xAOD::EventShape>(m_outputEventShape) ){
    ATH_MSG_WARNING( "EventShape with key "<< m_outputEventShape << " already exists. " );
    return StatusCode::SUCCESS;      
  }

  xAOD::EventShape *evs = new xAOD::EventShape();
  xAOD::EventShapeAuxInfo* aux = new xAOD::EventShapeAuxInfo();
  evs->setStore( aux );
  ATH_CHECK( evtStore()->record( aux, m_outputEventShape + "Aux." ) );
  ATH_CHECK( evtStore()->record( evs, m_outputEventShape ) );
  ATH_MSG_DEBUG("Created new EventShape container: " << m_outputEventShape);

  return fillEventShape(evs);  
}

//**********************************************************************
StatusCode EventShapeCopier::fillEventShape(xAOD::EventShape *evs) const {
  const xAOD::EventShape * inputES;
  ATH_CHECK( evtStore()->retrieve(inputES, m_inputEventShape) ); 
  
  evs->auxdata<float> ("Density")      = inputES->auxdata<float>( m_eventDensityName ) ;
  evs->auxdata<float> ("DensitySigma") = inputES->auxdata<float>( m_eventDensityName+"Sigma" ) ;
  evs->auxdata<float> ("DensityArea")  = inputES->auxdata<float>( m_eventDensityName+"Area" ) ;

  return StatusCode::SUCCESS;      
}


StatusCode EventShapeCopier::fillEventShape(xAOD::EventShape *evs, const xAOD::IParticleContainer*) const {
  // Ignore IParticleContainer argument.

  return fillEventShape( evs);
}
  
//**********************************************************************

#ifdef XAOD_ANALYSIS
void EventShapeCopier::renameEventDensities(){
  setProperty("InputEventShape", "LCTopoEventShape").ignore();
  setProperty("OutputEventShape", "Kt4LCTopoEventShape").ignore();
  setProperty("EventDensityName", "DensityForJetsR4").ignore();
  
  fillEventShape().ignore();

  setProperty("InputEventShape", "LCTopoEventShape").ignore();
  setProperty("OutputEventShape", "Kt6LCTopoEventShape").ignore();
  setProperty("EventDensityName", "DensityForJetsR6").ignore();    

  fillEventShape().ignore();
}
#else
// In Athena, do nothing. Use an Athena alg as defined in python/EventDensityConfig.py
void EventShapeCopier::renameEventDensities(){}
#endif
