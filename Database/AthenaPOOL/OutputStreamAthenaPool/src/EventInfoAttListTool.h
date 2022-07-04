/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOATTLISTTOOL_H 
#define EVENTINFOATTLISTTOOL_H 

/*****************************************************************************
Name    : EventInfoAttListTool.h
Purpose : Tool to buid the Global Event Tags
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODEventInfo/EventInfo.h" //typedef

#include <string>
#include <memory> //unique_ptr

class AthenaAttributeList;
class EventInfo;
namespace coral{
 class AttributeListSpecification;
}

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
  StatusCode initialize() override;
  StatusCode finalize() override;

  // interface 
  bool isValid();
  const coral::AttributeListSpecification& getAttributeSpecification();
  const AthenaAttributeList getAttributeList(const xAOD::EventInfo& einfo);
  const AthenaAttributeList getAttributeList(const EventInfo& einfo);

  std::unique_ptr<AthenaAttributeList> getAttributeListPtr(const xAOD::EventInfo& einfo);

protected:

  /** the various components to build their own fragments of tag */
  StatusCode eventTag       (AthenaAttributeList& eventTagCol, 
                             const xAOD::EventInfo& eventInfo);
  StatusCode eventTag       (AthenaAttributeList& eventTagCol, 
                             const EventInfo& eventInfo);

  coral::AttributeListSpecification* m_attribListSpec{};

};

#endif // EVENTINFOATTLISTTOOL_H
