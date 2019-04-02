/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_PILEUPHASHHELPER_H
#define PILEUPCOMPS_PILEUPHASHHELPER_H

/**
    @file PileUpHashHelper.h
    @brief A helper class to compute a hash of pile-up events
    @author Tadej Novak
*/

#include <sstream>

#include <uuid/uuid.h>

#include <xAODEventInfo/EventInfo.h>

class PileUpHashHelper
{
public:
  PileUpHashHelper() {}

  /// Add a plain string to the stream
  void addToHashSource(const std::string &string);

  /// Add subevents to the stream
  void addToHashSource(const xAOD::EventInfo *eventInfo);

  /// Clear the stream
  void clearHashSource();

  /// Get the current hash base
  std::string hashSource() const { return m_stream.str(); }

  /// Calculate the hash
  void calculateHash(uuid_t &hash) const;

  /// Convert uuid_t to xAOD::EventInfo::PileUpMixtureID
  static xAOD::EventInfo::PileUpMixtureID uuidToPileUpMixtureId(const uuid_t &hash);

  /// xAOD::EventInfo::PileUpMixtureID to uuid_t
  static void pileUpMixtureIdToUuid(const xAOD::EventInfo::PileUpMixtureID &mixture,
                                    uuid_t &hash);

  /// Size of individual low/high bits in bytes
  static const uint16_t PILEUP_SIZE_BYTES = 8;

private:
  std::stringstream m_stream;
};

#endif // PILEUPCOMPS_PILEUPHASHHELPER_H
