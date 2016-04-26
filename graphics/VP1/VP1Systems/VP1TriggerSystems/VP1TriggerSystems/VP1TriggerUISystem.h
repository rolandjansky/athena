/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef VP1TRIGGERUISYSTEM_H
#define VP1TRIGGERUISYSTEM_H

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : VP1TriggerUISystem
 *
 * @brief   : Trigger UI System passing StoreGate and CMDs to VP1TriggerTool (Header)
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/Logger.h"

//Other includes
#include "StoreGate/StoreGateSvc.h"
#include "VP1Base/IVP1System.h"

//Classes
namespace VP1Trig {
  class VP1TriggerTool;
}

class VP1TriggerUISystem : public IVP1System, public virtual VP1Trig::Logger
{
  Q_OBJECT
    
public:
  VP1TriggerUISystem();
  virtual ~VP1TriggerUISystem();
  
  //System mains ---------------------------------->
  void create(StoreGateSvc*);
  void refresh(StoreGateSvc* m_storeGate);
  void erase();
  //-----------------------------------------------<
    
  //Access to trigger tool
  VP1Trig::VP1TriggerTool * m_triggertool;
};
#endif

