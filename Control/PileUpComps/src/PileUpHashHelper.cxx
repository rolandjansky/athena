/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#include <CxxUtils/MD5.h>

#include "PileUpHashHelper.h"

void PileUpHashHelper::addToHashSource(const std::string &string)
{
  m_stream << string;
}

void PileUpHashHelper::addToHashSource(const xAOD::EventInfo *eventInfo)
{
  const std::vector<xAOD::EventInfo::SubEvent> &subEventsSource = eventInfo->subEvents();
  std::vector<std::reference_wrapper<const xAOD::EventInfo::SubEvent>> subEvents(subEventsSource.begin(), subEventsSource.end());

  std::stable_sort(subEvents.begin(), subEvents.end(), [](const xAOD::EventInfo::SubEvent &a, const xAOD::EventInfo::SubEvent &b) {
    return (a.ptr()->bcid() < b.ptr()->bcid()) ||
      ((a.ptr()->bcid() == b.ptr()->bcid()) && (a.type() < b.type())) ||
      ((a.ptr()->bcid() == b.ptr()->bcid()) && (a.type() == b.type()) && (a.ptr()->eventNumber() < b.ptr()->eventNumber()));
  });

  // Generate subevents info string
  bool first = true;
  for (const xAOD::EventInfo::SubEvent &subevent : subEvents) {
    if (subevent.type() == xAOD::EventInfo::Signal) {
      continue;
    }

    if (first) {
      first = false;
    } else {
      m_stream << ";";
    }

    m_stream << (static_cast<int32_t>(subevent.ptr()->bcid()) - static_cast<int32_t>(eventInfo->bcid())) << "_" << subevent.type() << "_" << subevent.ptr()->eventNumber();
  }
}

void PileUpHashHelper::clearHashSource()
{
  m_stream.str(std::string());
}

void PileUpHashHelper::calculateHash(uuid_t &hash) const
{
  std::string sourceStr = m_stream.str();

  MD5 md5Hash(sourceStr);
  md5Hash.uuid_digest(hash);
}

xAOD::EventInfo::PileUpMixtureID PileUpHashHelper::uuidToPileUpMixtureId(const uuid_t &hash)
{
  xAOD::EventInfo::PileUpMixtureID mixture{};

  for (uint16_t i = 0; i < PILEUP_SIZE_BYTES; i++) {
    mixture.lowBits |= (static_cast<unsigned long long>(hash[i]) << (i * sizeof(unsigned char) * CHAR_BIT));
    mixture.highBits |= (static_cast<unsigned long long>(hash[i + PILEUP_SIZE_BYTES]) << (i * sizeof(unsigned char) * CHAR_BIT));
  }

  return mixture;
}

void PileUpHashHelper::pileUpMixtureIdToUuid(const xAOD::EventInfo::PileUpMixtureID &mixture,
                                             uuid_t &hash)
{
  for (uint16_t i = 0; i < PILEUP_SIZE_BYTES; i++) {
    hash[i] |= (mixture.lowBits >> (i * sizeof(unsigned char) * CHAR_BIT));
    hash[i + PILEUP_SIZE_BYTES] |= (mixture.highBits >> (i * sizeof(unsigned char) * CHAR_BIT));
  }
}
