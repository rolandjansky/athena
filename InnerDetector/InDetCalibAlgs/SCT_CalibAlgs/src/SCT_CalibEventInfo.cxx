/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibEventInfo.h
 * Implementation file for the SCT_CalibEventInfo class
 * @author Shaun Roe
 **/

#include "SCT_CalibEventInfo.h"
#include "SCT_CalibUtilities.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "CoralBase/TimeStamp.h"

#include <limits>

namespace {
  const int INTMIN{std::numeric_limits<int>::min()};
  const int INTMAX{std::numeric_limits<int>::max()};
  const long long oneBillion{1000000000LL};
}

SCT_CalibEventInfo::SCT_CalibEventInfo(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_timeStampBegin{INTMAX},
  m_tsBeginString{""},
  m_tsEndString{""},
  m_timeStampEnd{INTMIN},
  m_duration{0},
  m_LBBegin{INTMAX},
  m_LBEnd{INTMIN},
  m_numLB{0},
  m_source{"UNKNOWN"},
  m_runNumber{0},
  m_lumiBlock{0},
  m_timeStamp{0},
  m_bunchCrossing{0},
  m_counter{0}
{
}

StatusCode
SCT_CalibEventInfo::initialize() {
  ATH_MSG_INFO("Initialize of evtInfo in " << PACKAGE_VERSION);
  return StatusCode::SUCCESS;
}

StatusCode
SCT_CalibEventInfo::finalize() {
  return StatusCode::SUCCESS;
}

void
SCT_CalibEventInfo::setTimeStamp(const int begin, const int end) {
  m_timeStampBegin = begin;
  m_timeStampEnd = end;
  m_duration = m_timeStampEnd-m_timeStampBegin;
  m_tsBeginString = toUtc(begin);
  m_tsEndString = toUtc(end);
}

void
SCT_CalibEventInfo::setTimeStamp(const std::string& begin, const std::string& end) {
  int ibegin{std::stoi(begin)};
  int iend{std::stoi(end)};
  setTimeStamp(ibegin, iend);
}

void
SCT_CalibEventInfo::setTimeStamp(const int ts) {
  m_timeStamp = ts;
}

void
SCT_CalibEventInfo::getTimeStamps(int& begin, int& end) const {
  begin = m_timeStampBegin;
  end = m_timeStampEnd;
}

void
SCT_CalibEventInfo::getTimeStamps(std::string& begin, std::string& end) const {
  begin = m_tsBeginString;
  end = m_tsEndString;
}

int
SCT_CalibEventInfo::timeStamp() const {
  return m_timeStamp;
}

int SCT_CalibEventInfo::duration() const {
  return m_duration;
}

void
SCT_CalibEventInfo::setSource(const std::string source) {
  m_source = source;
}

void
SCT_CalibEventInfo::setLumiBlock(const int begin, const int end) {
  m_LBBegin = begin;
  m_LBEnd = end;
  m_numLB = end-begin+1;
}

void
SCT_CalibEventInfo::setLumiBlock(const int lb) {
  m_lumiBlock = lb;
}

void
SCT_CalibEventInfo::getLumiBlock(int& begin, int& end) const {
  begin = m_LBBegin;
  end = m_LBEnd;
}

int SCT_CalibEventInfo::lumiBlock() const {
  return m_lumiBlock;
}

int
SCT_CalibEventInfo::numLumiBlocks() const {
  return m_numLB;
}

void
SCT_CalibEventInfo::setRunNumber(const int rn) {
  m_runNumber = rn;
}

int
SCT_CalibEventInfo::runNumber() const {
  return m_runNumber;
}

void
SCT_CalibEventInfo::setCounter(const int counterVal) {
  m_counter = counterVal;
}

void
SCT_CalibEventInfo::incrementCounter() {
  ++m_counter;
}

int
SCT_CalibEventInfo::counter() const {
  return m_counter;
}

void
SCT_CalibEventInfo::setBunchCrossing(const int bc) {
  m_bunchCrossing = bc;
}

std::string
SCT_CalibEventInfo::toUtc(const int timestamp) const {
  coral::TimeStamp::ValueType nsTime{timestamp*oneBillion};
  coral::TimeStamp utc{nsTime};
  return utc.toString();
}
