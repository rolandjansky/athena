/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_EVENTINFOTAGBUILDER_H
#define EVENTTAGALGS_EVENTINFOTAGBUILDER_H

/*****************************************************************************
Name    : EventInfoTagBuilder.h
Package : offline/PhysicsAnalysis/EventTag/EventTagAlgs
Author  : Ketevi A. Assamagan, Kristo Karr
Created : July 2004
Updated : February 2006

DESCRIPTION:

Retrieves event tag data from an AOD file, stores this data in an attribute 
list object and records the attribute list object in the TDS for subsequent 
inclusion in an event tag database.

*****************************************************************************/

/**
   @class EventTagBuilder

   @brief Algorithm to store event tag information in an attribute list object.

*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventInfoAttListTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "GaudiKernel/ToolHandle.h"

class EventInfoTagBuilder : public AthAlgorithm {
public:

  /// Standard constructor.
  EventInfoTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  /// Destructor.
  ~EventInfoTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  
  /// Global Event Tag Tool
  ToolHandle<EventInfoAttListTool> m_tool;

  SG::ReadHandleKey<xAOD::EventInfo> m_evtKey;
  SG::ReadHandleKey<AthenaAttributeList> m_inputAttList;
  SG::WriteHandleKey<AthenaAttributeList> m_attributeListName;
  bool m_propInput;
  std::string m_filter;

};

#endif // EVENTTAGALGS_EVENTINFOTAGBUILDER_H
