/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1AthenaPtrs                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1AthenaPtrs.h"

StoreGateSvc * VP1AthenaPtrs::m_eventStore = 0;
StoreGateSvc * VP1AthenaPtrs::m_detectorStore = 0;
ISvcLocator * VP1AthenaPtrs::m_serviceLocator = 0;
IToolSvc * VP1AthenaPtrs::m_toolSvc = 0;

//____________________________________________________________________
void VP1AthenaPtrs::setPointers( StoreGateSvc * eventStore,
				 StoreGateSvc * detectorStore,
				 ISvcLocator * serviceLocator,
				 IToolSvc * toolSvc )
{
  VP1AthenaPtrs::m_eventStore = eventStore;
  VP1AthenaPtrs::m_detectorStore = detectorStore;
  VP1AthenaPtrs::m_serviceLocator = serviceLocator;
  VP1AthenaPtrs::m_toolSvc = toolSvc;
}
