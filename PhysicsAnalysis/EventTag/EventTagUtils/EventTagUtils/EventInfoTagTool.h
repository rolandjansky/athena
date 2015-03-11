/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOTAGTOOL_H 
#define EVENTINFOTAGTOOL_H 

/*****************************************************************************
Name    : EventInfoTagTool.h
Package : offline/PhysicsAnalysis/EventTag/EventTagUtils
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : Tool to buid the Global Event Tags
*****************************************************************************/

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "xAODEventInfo/EventInfo.h"

#include <map>
#include <vector>
#include <string>

// forward declarations
class ILumiBlockMuTool;

/** Interface ID for EventInfoTagTool */  
static const InterfaceID IID_EventInfoTagTool("EventInfoTagTool", 1, 0);

class EventInfoTagTool : public AthAlgTool, virtual public IIncidentListener  {

public:
  
  /** Standard Constructor */
  EventInfoTagTool(const std::string& type, const std::string& name, const IInterface* 
parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_EventInfoTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::vector<std::pair<std::string,AthenaAttributeType> >& attrMap);
  virtual StatusCode attributeSpecification(std::vector<AthenaAttributeSpecification>& attrMap);
  virtual StatusCode execute(TagFragmentCollection& eventTagCol);  
  virtual StatusCode finalize();
  virtual StatusCode stop();
  /** Incident service handle listening  */
  virtual void handle(const Incident& incident);

protected:

   /** Standard destructor */
   virtual ~EventInfoTagTool( );

   /** the various components to build their own fragments of tag */
   StatusCode eventTag       (TagFragmentCollection& eventTagCol, const DataHandle<xAOD::EventInfo> eventInfo);
   StatusCode eventTagFlags  (TagFragmentCollection& eventTagCol, const DataHandle<xAOD::EventInfo> eventInfo);
   StatusCode eventExtrasTag (TagFragmentCollection& eventTagCol, const DataHandle<xAOD::EventInfo> eventInfo);

   // LumiBlockMuTool
   ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;

   unsigned int m_runNumber,m_condRunNumber,m_lumiBlock;
   unsigned long long m_eventNumber; 

   /** Properties */
   bool m_includeEventFlag;
   bool m_includeExtras;
   bool m_isMC;
   unsigned int m_weightSum;

 };

#endif // EVENTINFOTAGTOOL_H



