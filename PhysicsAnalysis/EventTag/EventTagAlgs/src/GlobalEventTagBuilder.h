/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_GLOBALEVENTTAGBUILDER_H
#define EVENTTAGALGS_GLOBALEVENTTAGBUILDER_H

/*****************************************************************************
Name    : GlobalEventTagBuilder.h
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

  @author K. Assamagan, K. Karr

  This algorithm reads in Global Event Data,
  stores this data as event tags in an attribute list object,
  and records the attribute list object in the TDS so that
  it may retrieved further downstream and added to an event tag 
  collection along with a reference to the data header object.
*/

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"
#include "EventTagUtils/GlobalEventTagTool.h"

class GlobalEventTagBuilder : public AthAlgorithm, public TagBuilderBase {
public:

  /// Standard constructor.
  GlobalEventTagBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  
  /// Destructor.
  ~GlobalEventTagBuilder();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  
  /// Attribute list name
  std::string m_attributeListName;

  /// Global Event Tag Tool
  ToolHandle<GlobalEventTagTool> m_globalEventTagTool;
};

#endif // EVENTTAGALGS_GLOBALEVENTTAGBUILDER_H
