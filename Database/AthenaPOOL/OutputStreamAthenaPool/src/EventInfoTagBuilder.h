/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include <AthenaBaseComps/AthAlgorithm.h>
#include <GaudiKernel/ToolHandle.h>
#include <PersistentDataModel/AthenaAttributeList.h>
#include <StoreGate/ReadHandleKey.h>
#include <StoreGate/WriteHandleKey.h>
#include <xAODEventInfo/EventInfo.h>

#include "EventInfoAttListTool.h"

class EventInfoTagBuilder : public AthAlgorithm
{
public:

  /// Standard constructor.
  EventInfoTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor.
  ~EventInfoTagBuilder() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  /// Global Event Tag Tool
  ToolHandle<EventInfoAttListTool> m_tool{this, "Tool", "EventInfoAttListTool/EventInfoAttListTool", "EventInfoAttListTool used"};

  SG::ReadHandleKey<xAOD::EventInfo> m_evtKey{this, "EventInfoKey", "EventInfo", "xAOD::EventInfo ReadHandleKey"};
  SG::ReadHandleKey<AthenaAttributeList> m_inputAttList{this, "InputList", "Input", "Input Athena attribute list ReadHandleKey"};
  SG::WriteHandleKey<AthenaAttributeList> m_attributeListName{this, "AttributeList", "SimpleTag", "Output Athena attribute list WriteHandleKey"};

  Gaudi::Property<bool> m_propInput{this, "PropagateInput", true, "Propagate input attribute list to the output"};
  Gaudi::Property<std::string> m_filter{this, "FilterString", "", "Filter input attribute list when propagating to the output"};

};

#endif // EVENTTAGALGS_EVENTINFOTAGBUILDER_H
