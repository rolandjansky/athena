/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSPLITTERALGS_EVENTSPLITTER_H
#define EVENTSPLITTERALGS_EVENTSPLITTER_H

#include <inttypes.h>
#include <string>
#include <vector>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventTagAlgs/TagBuilderBase.h"

class StoreGateSvc;
class TagAthenaAttributeList;

class EventSplitter : public AthAlgorithm, public TagBuilderBase
{
public:
  EventSplitter (const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  void setSuccess(bool f);
  
  // Name for attribute in tag (absent if not specified)
  TagAthenaAttributeList* m_attribList;
  std::string m_attrName;

  //! key for CTP decision
  std::string  m_KeyCTP_Decision;

  //! key for HLT
  std::string m_KeyHLTWord;

  //! MaxN of M items (MaxN <= M)
  enum {MaxN = 4};

  //! bit mask for CTP decision
  std::vector<std::vector<uint32_t> > m_MaskCTP; //  8 x 32bit = 0-255bit

  //! bit mask for Trigger Type Word 
  uint32_t m_MaskTTW[MaxN];

  //! bit mask for HLT info
  uint32_t m_MaskHLT[MaxN];

  //! flag to pass all
  uint32_t m_PassAll;
};

#endif


