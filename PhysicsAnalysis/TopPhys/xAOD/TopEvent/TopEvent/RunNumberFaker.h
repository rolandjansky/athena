/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RUNNUMBERFAKER_H_
#define RUNNUMBERFAKER_H_


#include "AsgTools/SgTEvent.h"
#include "AthContainers/AuxElement.h"
#include "TopConfiguration/ConfigurationSettings.h"
#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "xAODEventInfo/EventInfo.h"


namespace top {
namespace {


/** temporarily fake an older (2017) run number when running on new (2018) MC */
class RunNumberFaker {

public:

  explicit RunNumberFaker(asg::SgTEvent * evtStore)
  {
    top::check(evtStore->retrieve(m_eventInfo, "EventInfo"), "failed to retrieve EventInfo");
  }

  explicit RunNumberFaker(Event const & event)
    : m_eventInfo(event.m_info)
  {
  }

  void activate() {
    top::check(!m_active, "activate called multiple times");
    m_active = true;
    m_origRunNumber = m_accessor(*m_eventInfo);
    if (m_origRunNumber >= 348197 && ConfigurationSettings::get()->feature("RunNumberFaker")) {
      m_needcleanup = true;
      m_accessor(*m_eventInfo) = 336832; // see ATR-19057
    }
  }

  ~RunNumberFaker() {
    if (m_needcleanup) {
      m_accessor(*m_eventInfo) = m_origRunNumber;
    }
  }

private:

  typedef unsigned int runnumber_t;

  xAOD::EventInfo const * m_eventInfo{nullptr};
  SG::AuxElement::Decorator<runnumber_t> m_accessor{"RandomRunNumber"};
  bool m_active{false};
  bool m_needcleanup{false};
  runnumber_t m_origRunNumber{0};

};

}}

#endif
