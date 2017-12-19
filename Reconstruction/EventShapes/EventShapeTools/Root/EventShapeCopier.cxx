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

  // DataHandles
  ATH_CHECK( m_inputEventShape.initialize() );
  ATH_CHECK( m_outputEventShape.initialize() );
  m_outputEventShapeIn = m_outputEventShape.key();
  ATH_CHECK( m_outputEventShapeIn.initialize() );

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode EventShapeCopier::fillEventShape() const {
  
  ATH_MSG_DEBUG("Begin fillEventShape()");

  // check if not already existing
  auto handle_inOut = SG::makeHandle (m_outputEventShapeIn);
  if ( handle_inOut.isValid() ) {
    ATH_MSG_WARNING( "EventShape with key "<< m_outputEventShapeIn.key() << " already exists. Not overwriting it." );
    return StatusCode::SUCCESS;
  }

  xAOD::EventShape *evs = new xAOD::EventShape();
  std::unique_ptr<const xAOD::EventShape> evs_ptr(evs);

  xAOD::EventShapeAuxInfo* evsaux = new xAOD::EventShapeAuxInfo();
  std::unique_ptr<const xAOD::EventShapeAuxInfo> evsaux_ptr( evsaux);
  evs->setStore( evsaux );

  ATH_CHECK(fillEventShape(evs));  

  SG::WriteHandle<xAOD::EventShape> h_out(m_outputEventShape);
  if ( ! h_out.put(std::move(evs_ptr), std::move(evsaux_ptr)) ) {
    ATH_MSG_WARNING("Unable to write new Jet collection and aux store to event store: " << m_outputEventShape.key());
  } else {
    ATH_MSG_DEBUG("Created new EventShape container: " << m_outputEventShape.key());
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************
StatusCode EventShapeCopier::fillEventShape(xAOD::EventShape *evs) const {
  const xAOD::EventShape * inputES;

  SG::ReadHandle<xAOD::EventShape> handle_in( m_inputEventShape );
  if ( handle_in.isValid() ) {
    inputES = handle_in.cptr();
  } else {
    ATH_MSG_WARNING("Unable to read: " << m_inputEventShape.key());
    return StatusCode::FAILURE;      
  }

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
