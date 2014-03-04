/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1ToolAccessHelper               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/VP1ToolAccessHelper.h"
#include "VP1Base/IVP1System.h"

//____________________________________________________________________
VP1ToolAccessHelper::VP1ToolAccessHelper(IToolSvc* ts)
  : VP1HelperClassBase(0,"VP1ToolAccessHelper"),m_toolsvc(ts)
{
  if (!m_toolsvc)
    message("ERROR: Received null toolSvc pointer!");
}

//____________________________________________________________________
VP1ToolAccessHelper::VP1ToolAccessHelper(IVP1System* sys)
  : VP1HelperClassBase(sys,"VP1ToolAccessHelper"),m_toolsvc((sys?sys->toolSvc():0))
{
  if (!sys)
    message("ERROR: Received null system (and thus also null toolSvc) pointer!");
  else if (!m_toolsvc)
    message("ERROR: Could not get toolSvc pointer from system!");
}
