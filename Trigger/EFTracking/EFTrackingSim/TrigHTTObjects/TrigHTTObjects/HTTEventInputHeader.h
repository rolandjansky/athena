/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_HTTEVENTINPUTHEADER_H
#define TRIGHTTOBJECTS_HTTEVENTINPUTHEADER_H

#include <TObject.h>
#include <vector>
#include <iostream>
#include <sstream>

#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTObjects/HTTOptionalEventInfo.h"


class HTTEventInfo;
class HTTOptionalEventInfo;

class HTTEventInputHeader : public TObject
{
public:

  virtual ~HTTEventInputHeader();

  void newEvent(HTTEventInfo const& event) { reset(); m_event = event; }
  void setOptional(HTTOptionalEventInfo const& optional) { m_optional = optional; }
  void reset();//reset per event variables


  HTTEventInfo const& event()            const { return m_event; }
  HTTOptionalEventInfo const& optional() const { return m_optional; }

  //  handling hits
  const std::vector<HTTHit>& hits() const { return m_Hits; }
  int  nHits()                      const { return m_Hits.size(); }
  void addHit(HTTHit const& s) { m_Hits.push_back(s); }
  void clearHits() { m_Hits.clear(); }
  void reserveHits(size_t size) { m_Hits.reserve(size); }


private:
  HTTEventInfo                 m_event;
  HTTOptionalEventInfo         m_optional;
  std::vector<HTTHit>          m_Hits;


  ClassDef(HTTEventInputHeader, 3);
};

std::ostream& operator<<(std::ostream&, const HTTEventInputHeader&);
#endif // TRIGHTTOBJECTS_HTTEVENTINPUTHEADER_H
