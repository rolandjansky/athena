/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : VP1TriggerUISystem
 *
 * @brief   : Trigger UI System passing StoreGate and CMDs to VP1TriggerTool
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/


//Local includes
#include "VP1TriggerSystems/VP1TriggerUISystem.h"
#include "VP1TriggerSystems/VP1TriggerTool.h"


//System Constructor
//_____________________________________________________________________________________________
VP1TriggerUISystem::VP1TriggerUISystem()
  : Logger("VP1TriggerUISystem"),
    IVP1System("VP1TriggerUISystem",
	       "Trigger UI System passing StoreGate and CMDs to VP1TriggerTool",
	       "Manuel Proissl, mproissl@cern.ch")
{
  //Initialize VP1TriggerTool
  log_info("Initializing VP1Trig::VP1TriggerTool");
  m_triggertool = new VP1Trig::VP1TriggerTool();
}


//System Destructor
//_____________________________________________________________________________________________
VP1TriggerUISystem::~VP1TriggerUISystem()
{
  log_verbose("destructor");
  delete m_triggertool;
}


//Initialize GUI (on Plugin level only)
//_____________________________________________________________________________________________
void VP1TriggerUISystem::create(StoreGateSvc*) {log_verbose("create()");}


//Load Trigger Data from StoreGate (after new event loaded)
//_____________________________________________________________________________________________
void VP1TriggerUISystem::refresh(StoreGateSvc* m_storeGate)
{
  log_verbose("refresh()");
  m_triggertool->process(m_storeGate);
}


//Clear System (before new event loaded)
//_____________________________________________________________________________________________
void VP1TriggerUISystem::erase()
{
  log_verbose("erase()");
  m_triggertool->clear();
}
