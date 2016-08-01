/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReweightUtils includes
#include "ReweightUtils/WeightsAgregator.h"

#include "xAODParticleEvent/CompositeParticle.h"


WeightsAgregator::WeightsAgregator( const std::string& name )
  : WeightToolBase( name ),
    m_eventInfoName("EventInfo"),
    m_toolArray() {
  declareProperty("Ingredients",   m_toolArray,     "List of helpers to offload some tasks to" );
  declareProperty("EventInfoName", m_eventInfoName, "Name of the xAOD::EventInfo object");
}


StatusCode WeightsAgregator::initialize() {
  ATH_MSG_DEBUG ("Initializing " << name() << "...");
  ATH_CHECK(WeightToolBase::initialize());

  // Retrieve the needed weight tools
  ATH_CHECK(m_toolArray.retrieve());

  return StatusCode::SUCCESS;
}



// returns: the value that was calculated from the xAOD::IParticle composite built event
double WeightsAgregator::evaluate( const xAOD::IParticle* part ) const
{
  ATH_MSG_DEBUG ("Evaluating " << name() << "...");

  double weight=1.0;
  for ( const auto& wToolHandle : m_toolArray ){
    const IWeightTool* wtool = &(*wToolHandle);
    ATH_MSG_DEBUG ("  In WeightsAgregator::evaluate(), having tool to deal with called " << wtool->name());
    if( const IObservableTool* obstool = dynamic_cast< const IObservableTool* >( wtool ) )
      weight *= obstool->evaluate(part);
  }

  ATH_MSG_DEBUG ("Got weight in WeightsAgregator::evaluate() = " << weight << ".");
  return weight;
}


double WeightsAgregator::getWeight() const {
 //Retrieveing eventInfo
  const xAOD::EventInfo* evtInfo;
  StatusCode sc = evtStore()->retrieve( evtInfo, m_eventInfoName );
  if(sc.isFailure() || !evtInfo) {
    ATH_MSG_ERROR (" EventInfo could not be retrieved !!");
    return 0.;
  }

  // don't do anything for data
  if(!evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)){
    ATH_MSG_DEBUG (" Returning weight=1. for data.");
    return 1.;
  }

  float weight=1.;
  ToolHandleArray< IWeightTool >::const_iterator itPriv = m_toolArray.begin(), itPrivEnd = m_toolArray.end();
  for (  ; itPriv != itPrivEnd; ++itPriv ) {
    /*if (!(*itPriv))
      ATH_MSG_ERROR("tool " << (*itPriv).typeAndName() << " failed it's task ... stop!");*/
    weight *= (*itPriv)->getWeight();
  }

  //Decorate EventInfo with weight for later use
  this->addDecoration(evtInfo, weight);


  ATH_MSG_DEBUG ("        " << name() << " returning weight= " << weight << ".");
  return weight;
}
