/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOATTLISTTOOL_H 
#define EVENTINFOATTLISTTOOL_H 

/*****************************************************************************
Name    : EventInfoAttListTool.h
Purpose : Tool to buid the Global Event Tags
*****************************************************************************/

#include "GaudiKernel/DataHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CoralBase/AttributeListSpecification.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "xAODEventInfo/EventInfo.h"

#include <map>
#include <vector>
#include <string>

// forward declarations

/** Interface ID for EventInfoAttListTool */  
static const InterfaceID IID_EventInfoAttListTool("EventInfoAttListTool", 1, 0);

class EventInfoAttListTool : public AthAlgTool  {

public:
  
  /** Standard Constructor */
  EventInfoAttListTool(const std::string& type, 
                   const std::string& name, 
                   const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_EventInfoAttListTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // interface 
  bool isValid();
  const coral::AttributeListSpecification& getAttributeSpecification();
  const AthenaAttributeList getAttributeList(const DataHandle<xAOD::EventInfo> einfo);

protected:

   /** Standard destructor */
   virtual ~EventInfoAttListTool( );

   /** the various components to build their own fragments of tag */
   StatusCode eventTag       (AthenaAttributeList& eventTagCol, const DataHandle<xAOD::EventInfo> eventInfo);

   coral::AttributeListSpecification* m_attribListSpec;

 };

#endif // EVENTINFOATTLISTTOOL_H



