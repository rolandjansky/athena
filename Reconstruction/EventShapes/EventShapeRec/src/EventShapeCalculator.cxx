/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventShapeRec/EventShapeCalculator.h"
#include "JetEvent/JetCollection.h"

#include "EventShapeEvent/EventShapes.h"
#include "EventShapeEvent/EventShapeStore.h"
#include "EventShapeEvent/EventShapeTypes.h"
#include "AthContainers/ConstDataVector.h"

#include <cmath>

EventShapeCalculator::EventShapeCalculator(const std::string& name,
					   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name,pSvcLocator)
  , m_eventShapeTools()
  , m_forwardTermTool("")
{
  // input data
  declareProperty("InputCollection",m_inputCollection);
  declareProperty("CentralRegionCut",m_cutCentral=2.5);
  
  declareProperty("JetCollection",m_jetCollection="Cone4TowerJets");
  declareProperty("JetRegionCut", m_jetcutCentral=2.0);
  declareProperty("JetEtCut",     m_jetcutEt=25.0);
  
  declareProperty("OutputCollection",m_OutputCollection);
  // declareProperty("EventShapeToolNames",m_eventShapeToolNames);
  // declareProperty("ForwardTermToolName",m_forwardTermToolName="");
  declareProperty("EventShapeTools",m_eventShapeTools);
  declareProperty("ForwardTermTool",m_forwardTermTool);
}

StatusCode EventShapeCalculator::initialize()
{
  StatusCode checkOut = m_eventShapeTools.retrieve();
  if ( checkOut.isFailure() )
    {
      msg(MSG::ERROR) << "cannot allocate tools " << m_eventShapeTools << endreq;
      return checkOut;
    }
  
  for ( tool_store_t::const_iterator tool_iter = m_eventShapeTools.begin();
	tool_iter != m_eventShapeTools.end(); tool_iter++ )
    if (((*tool_iter)->initTool(m_cutCentral)).isFailure() )
      msg(MSG::ERROR) << "initTool() for tool " << tool_iter->name() << " failed" << endreq;
  
  checkOut = m_forwardTermTool.retrieve();
  if ( checkOut.isFailure() )
    {
      msg(MSG::ERROR) << "cannot allocate tool " << m_forwardTermTool << endreq;
      return checkOut;
    }
  if ((m_forwardTermTool->initTool(m_cutCentral)).isFailure() )
    msg(MSG::ERROR) << "initTool() for tool " << m_forwardTermTool->name() << " failed" << endreq;
  
  msg(MSG::INFO) << "List of tools:" << endreq;
  for ( tool_store_t::const_iterator tool_iter = m_eventShapeTools.begin();
	tool_iter != m_eventShapeTools.end(); tool_iter++ )
    msg(MSG::INFO) << ".. " << tool_iter->name() << endreq;
  msg(MSG::INFO) << "Calculation of forward Term:" << endreq;
  msg(MSG::INFO) << ".. " << m_forwardTermTool->name() << endreq;
  return StatusCode::SUCCESS;
}

StatusCode EventShapeCalculator::execute()
{
  // message service
  MsgStream log(messageService(),name());
  if (msgLvl(MSG::INFO))
    msg(MSG::INFO) << "execute" << endreq;
  
  // pre-cuts. highest two energetic jets (detault: Cone4Tower) within eta range
  //  defined by m_cutCentral
  const JetCollection* pJets = 0;
  StatusCode checkOut(evtStore()->retrieve(pJets,m_jetCollection));
  if ( checkOut.isFailure() )
    {
      msg(MSG::ERROR) << "cannot allocate jet collection with key <"
		      << m_jetCollection << ">" << endreq;
      return checkOut;
    }
  
  // jets are sorted, more than 2 ?
  if ( pJets->size()<2 )
    {
      if (msgLvl(MSG::DEBUG))
	msg(MSG::DEBUG) << "Not enough jets in this event" << endreq;
      return StatusCode::SUCCESS;
    }
  
  double et_jet1=0;
  double et_jet2=0;
  
  bool callTools = true;
  
  if ( fabs((*pJets)[0]->et()) < m_jetcutEt )
    {
      if (msgLvl(MSG::DEBUG))
	msg(MSG::DEBUG) << "jet one outside of Et region!" << endreq;
      callTools = false;
    }
  
  if ( fabs((*pJets)[0]->eta()) > m_jetcutCentral ||
       fabs((*pJets)[1]->eta()) > m_jetcutCentral )
    {
      if (msgLvl(MSG::DEBUG))
	msg(MSG::DEBUG) << "jets outside of cut region!" << endreq;
      callTools = false;
    }
  
  et_jet1=(*pJets)[0]->et();
  et_jet2=(*pJets)[1]->et();
  
  //  particles.clear();
  
  StatusCode checkColl = StatusCode::SUCCESS;
  
  // allocate collection
  const INavigable4MomentumCollection* thisColl;
  
  checkColl = evtStore()->retrieve(thisColl,m_inputCollection);
  
  if ( checkColl.isFailure() )
    {
      msg(MSG::ERROR)
	<< "Error retrieving collection with key <"
	<< m_inputCollection << ">" << endreq;
      return StatusCode::FAILURE;
    }
  
  EventShapeStore *ess = new EventShapeStore();
  StatusCode writeOut = evtStore()->record(ess,m_OutputCollection);
  
  ess->insert(EventShapes::JET1_ET, et_jet1);
  ess->insert(EventShapes::JET2_ET, et_jet2);
  ess->insert(EventShapes::CENTRAL_CUT, m_cutCentral);
  ess->insert(EventShapes::JET_ETA_CUT, m_jetcutCentral);
  
  if(callTools)
    {  
      typedef ConstDataVector<EventShapeTypes::es_4vect_vect_t>
        selected_particles_t;
      selected_particles_t selectedParticles(SG::VIEW_ELEMENTS);
      
      std::remove_copy_if(thisColl->begin(),
			  thisColl->end(),
			  std::back_insert_iterator<selected_particles_t>(selectedParticles),
			  etaSelector(m_cutCentral) );
      
      for ( tool_store_t::const_iterator tool_iter = m_eventShapeTools.begin();
	    tool_iter != m_eventShapeTools.end(); tool_iter++ )
	(*tool_iter)->calculate(selectedParticles.asDataVector(),ess);
      
      // this is the only tool to run on the full collection
      selectedParticles.clear();
      std::copy(thisColl->begin(),
		thisColl->end(),
		std::back_insert_iterator<selected_particles_t>(selectedParticles));
      if(bool(m_forwardTermTool))
	m_forwardTermTool->calculate(selectedParticles.asDataVector(),ess);
    }
  
  writeOut = evtStore()->setConst(ess);
  
  if ( writeOut.isFailure() )
    {
      msg(MSG::FATAL)
	<< "Error writing out the EventShapeStore with key <"
	<< m_OutputCollection << ">" << endreq;
      return StatusCode::FAILURE;
    }
  
  return StatusCode::SUCCESS;
}

StatusCode EventShapeCalculator::finalize()
{
  if (msgLvl(MSG::INFO))
    msg(MSG::INFO) << "finalialize" << endreq;
  return StatusCode::SUCCESS;
}
